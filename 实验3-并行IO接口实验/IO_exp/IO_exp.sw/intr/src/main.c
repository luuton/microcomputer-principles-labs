#include "xparameters.h"
#include "xgpio.h"
#include "xintc.h"
#include "xtmrctr.h"
#include "mb_interface.h"
#include "xil_printf.h"

#define Timer_TCSR      XPAR_AXI_TIMER_0_BASEADDR+XTC_TCSR_OFFSET
#define Timer_TLR       XPAR_AXI_TIMER_0_BASEADDR+XTC_TLR_OFFSET
#define Timer_TCR       XPAR_AXI_TIMER_0_BASEADDR+XTC_TCR_OFFSET

#define intc_ISR        XPAR_AXI_INTC_0_BASEADDR+XIN_ISR_OFFSET
#define intc_IER        XPAR_AXI_INTC_0_BASEADDR+XIN_IER_OFFSET
#define intc_IAR        XPAR_AXI_INTC_0_BASEADDR+XIN_IAR_OFFSET
#define intc_MER        XPAR_AXI_INTC_0_BASEADDR+XIN_MER_OFFSET

#define Btn_ISR         XPAR_AXI_GPIO_2_BASEADDR+XGPIO_ISR_OFFSET
#define Btn_IER         XPAR_AXI_GPIO_2_BASEADDR+XGPIO_IER_OFFSET
#define Btn_GIER        XPAR_AXI_GPIO_2_BASEADDR+XGPIO_GIE_OFFSET
#define Switch_ISR      XPAR_AXI_GPIO_0_BASEADDR+XGPIO_ISR_OFFSET
#define Switch_IER      XPAR_AXI_GPIO_0_BASEADDR+XGPIO_IER_OFFSET
#define Switch_GIER     XPAR_AXI_GPIO_0_BASEADDR+XGPIO_GIE_OFFSET

#define Reset_Value 10000

void My_ISR() __attribute__((interrupt_handler));
void PushBtnHandler();
void SwitchHandler();
void seg7_TimerHandler();

char segcode[6] = {0x00,0xc6,0xc1,0xc7,0xa1,0xce};   //c,u,l,d,r
short pos = 0xff7f;  //数码管位选信号
u8 button = 0;  //按键状态

// 打印标志和数据缓冲 (ISR 写入, 主循环读出并清除)
volatile int flag_btn       = 0;    // 按钮中断触发
volatile u8  flag_btn_val   = 0;    // 按钮编号
volatile int flag_switch    = 0;    // 开关中断触发
volatile u16 flag_switch_val = 0;   // 开关值
volatile int flag_isr_debug  = 0;   // ISR 调试状态触发
volatile int flag_isr_val    = 0;   // ISR 状态寄存器值

int init(){
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_TRI_OFFSET, 0xffff);   //配置GPIO0 ch1(开关)为输入
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_TRI2_OFFSET, 0x0000);  //配置GPIO0 ch2(LED)为输出
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_TRI_OFFSET, 0x0);      //配置GPIO1 ch1数码管段选信号为输出
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_TRI2_OFFSET, 0x0);     //配置GPIO1 ch2数码管位选信号为输出
    Xil_Out32(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_TRI_OFFSET, 0x1f);     //按键gpio输入 控制低五位

    Xil_Out32(Btn_IER, XGPIO_IR_CH1_MASK);                  //允许GPIO2 ch1中断
    Xil_Out32(Btn_GIER, XGPIO_GIE_GINTR_ENABLE_MASK);       //允许GPIO2中断输出
    Xil_Out32(Switch_IER, XGPIO_IR_CH1_MASK);               //允许GPIO0 ch1中断
    Xil_Out32(Switch_GIER, XGPIO_GIE_GINTR_ENABLE_MASK);    //允许GPIO0全局中断

    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR) &~XTC_CSR_ENABLE_INT_MASK);  //停止计数器
    Xil_Out32(Timer_TLR, Reset_Value);                                      //预置计数器初值
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR) | XTC_CSR_LOAD_MASK);        //装载计数器初值
    Xil_Out32(Timer_TCSR, (Xil_In32(Timer_TCSR)&~XTC_CSR_LOAD_MASK)\
    |XTC_CSR_ENABLE_TMR_MASK|XTC_CSR_ENABLE_INT_MASK|XTC_CSR_AUTO_RELOAD_MASK|XTC_CSR_DOWN_COUNT_MASK);  //使能计数器中断

    Xil_Out32(intc_IER, XPAR_AXI_TIMER_0_INTERRUPT_MASK|XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK|XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK);  //使能中断
    Xil_Out32(intc_MER, XIN_INT_MASTER_ENABLE_MASK|XIN_INT_HARDWARE_ENABLE_MASK);
    microblaze_enable_interrupts();  //使能处理器中断
    return 0;
}


int main(){
    init();
    while(1){
        // 按钮中断打印
        if (flag_btn) {
            xil_printf("Button Interrupt! button: %d\r\n", flag_btn_val);
            flag_btn = 0;
        }
        // 开关中断打印
        if (flag_switch) {
            xil_printf("Switch Interrupt! Switch state: %x\r\n", flag_switch_val);
            flag_switch = 0;
        }
        // 调试：非定时器的 ISR 状态打印
        if (flag_isr_debug) {
            xil_printf("ISR status: %x\r\n", flag_isr_val);
            flag_isr_debug = 0;
        }
    }
    return 0;
}

void My_ISR(void) {
    int status = Xil_In32(intc_ISR);

    // 非定时器中断时，设置调试打印标志
    if (status != XPAR_AXI_TIMER_0_INTERRUPT_MASK) {
        flag_isr_val   = status;
        flag_isr_debug = 1;
    }

    if (status & XPAR_AXI_TIMER_0_INTERRUPT_MASK) {          // 定时器中断
        seg7_TimerHandler();
    }
    if (status & XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK) {        // GPIO0 开关中断
        SwitchHandler();
    }
    if (status & XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK) {        // GPIO2 按键中断
        PushBtnHandler();
    }

    Xil_Out32(intc_IAR, status);  // 清除所有已处理中断
}

void PushBtnHandler(){
    switch (Xil_In32(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_DATA_OFFSET) & 0x1F)
        {
        case 1:button = 1;break;
        case 2:button = 2;break;
        case 4:button = 3;break;
        case 16:button = 4;break;
        case 8:button = 5;break;
        default:button = 0;break;
        }
    // 设置标志，由主循环打印
    flag_btn_val = button;
    flag_btn     = 1;

    Xil_Out32(Btn_ISR, Xil_In32(Btn_ISR));  //清除GPIO2中断
}

void SwitchHandler(){
    u16 sw = (u16)(Xil_In32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_DATA_OFFSET) & 0xFFFF);  //读取开关状态
    // 设置标志，由主循环打印
    flag_switch_val = sw;
    flag_switch     = 1;

    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_DATA2_OFFSET, sw);   //将开关状态输出到LED
    Xil_Out32(Switch_ISR, Xil_In32(Switch_ISR));  //清除GPIO0中断
}

int counter = 0;
void seg7_TimerHandler(){
    // xil_printf("Timer Interrupt! button: %d\r\n", button);
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA2_OFFSET, segcode[button]);    //根据开关状态输出数码管段选信号
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA_OFFSET, pos);                 //输出数码管位选信号
    pos = pos >> 1;  //位选信号循环右移
    counter++;
    if(counter==8) {
        counter = 0;
        pos = 0xff7f;  //重置位选信号
    }
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR));  //清除定时器中断
}