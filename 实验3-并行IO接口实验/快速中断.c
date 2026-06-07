#include "xparameters.h"
#include "xgpio.h"
#include "xintc.h"
#include "xtmrctr.h"
#include "mb_interface.h"
#include "xil_printf.h"

#define Timer_TCSR      XPAR_AXI_TIMER_0_BASEADDR+XTC_TCSR_OFFSET
#define Timer_TLR       XPAR_AXI_TIMER_0_BASEADDR+XTC_TLR_OFFSET
#define Timer_TCR       XPAR_AXI_TIMER_0_BASEADDR+XTC_TCR_OFFSET

#define Btn_ISR         XPAR_AXI_GPIO_2_BASEADDR+XGPIO_ISR_OFFSET
#define Btn_IER         XPAR_AXI_GPIO_2_BASEADDR+XGPIO_IER_OFFSET
#define Btn_GIER        XPAR_AXI_GPIO_2_BASEADDR+XGPIO_GIE_OFFSET
#define Switch_ISR      XPAR_AXI_GPIO_0_BASEADDR+XGPIO_ISR_OFFSET
#define Switch_IER      XPAR_AXI_GPIO_0_BASEADDR+XGPIO_IER_OFFSET
#define Switch_GIER     XPAR_AXI_GPIO_0_BASEADDR+XGPIO_GIE_OFFSET

#define Reset_Value 10000

// 快速中断：每个处理函数直接对应一个中断源，无需手动调度
// 各 ISR 必须有 void 签名，并声明 fast_interrupt 属性
void PushBtnHandler(void) __attribute__((fast_interrupt));
void SwitchHandler(void) __attribute__((fast_interrupt));
void seg7_TimerHandler(void) __attribute__((fast_interrupt));

XIntc intc;  // 中断控制器实例

char segcode[6] = {0x00,0xc6,0xc1,0xc7,0xa1,0xce};   //c,u,l,d,r
short pos = 0xff7f;  //数码管位选信号
u8 button = 0;  //按键状态

// 打印标志和数据缓冲 (ISR 写入, 主循环读出并清除)
volatile int flag_btn       = 0;    // 按钮中断触发
volatile u8  flag_btn_val   = 0;    // 按钮编号
volatile int flag_switch    = 0;    // 开关中断触发
volatile u16 flag_switch_val = 0;   // 开关值

int init(){
    // ===== 外设 GPIO 配置 =====
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_TRI_OFFSET, 0xffff);   //配置GPIO0 ch1(开关)为输入
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_TRI2_OFFSET, 0x0000);  //配置GPIO0 ch2(LED)为输出
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_TRI_OFFSET, 0x0);      //配置GPIO1 ch1数码管段选信号为输出
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_TRI2_OFFSET, 0x0);     //配置GPIO1 ch2数码管位选信号为输出
    Xil_Out32(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_TRI_OFFSET, 0x1f);     //按键gpio输入 控制低五位

    // 使能 GPIO 外设本身的中断
    Xil_Out32(Btn_IER, XGPIO_IR_CH1_MASK);                  //允许GPIO2 ch1中断
    Xil_Out32(Btn_GIER, XGPIO_GIE_GINTR_ENABLE_MASK);       //允许GPIO2中断输出
    Xil_Out32(Switch_IER, XGPIO_IR_CH1_MASK);               //允许GPIO0 ch1中断
    Xil_Out32(Switch_GIER, XGPIO_GIE_GINTR_ENABLE_MASK);    //允许GPIO0全局中断

    // ===== 定时器配置 =====
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR) &~XTC_CSR_ENABLE_INT_MASK);  //停止计数器
    Xil_Out32(Timer_TLR, Reset_Value);                                      //预置计数器初值
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR) | XTC_CSR_LOAD_MASK);        //装载计数器初值
    Xil_Out32(Timer_TCSR, (Xil_In32(Timer_TCSR)&~XTC_CSR_LOAD_MASK)\
    |XTC_CSR_ENABLE_TMR_MASK|XTC_CSR_ENABLE_INT_MASK|XTC_CSR_AUTO_RELOAD_MASK|XTC_CSR_DOWN_COUNT_MASK);  //使能计数器中断

    // ===== 中断控制器：快速中断模式 =====
    // INTC 负责自动向量分发——每个中断源直接跳转到对应处理函数，无需手动调度
    XIntc_Initialize(&intc, XPAR_AXI_INTC_0_DEVICE_ID);

    // 将各处理函数注册为快速中断（无参数、无回调，硬件直接向量跳转）
    XIntc_ConnectFastHandler(&intc,
        XPAR_AXI_INTC_0_AXI_TIMER_0_INTERRUPT_INTR,
        (XFastInterruptHandler)seg7_TimerHandler);
    XIntc_ConnectFastHandler(&intc,
        XPAR_AXI_INTC_0_AXI_GPIO_0_IP2INTC_IRPT_INTR,
        (XFastInterruptHandler)SwitchHandler);
    XIntc_ConnectFastHandler(&intc,
        XPAR_AXI_INTC_0_AXI_GPIO_2_IP2INTC_IRPT_INTR,
        (XFastInterruptHandler)PushBtnHandler);

    XIntc_Start(&intc, XIN_REAL_MODE);  // 启动 INTC（硬件模式）

    // 使能各中断源
    XIntc_Enable(&intc, XPAR_AXI_INTC_0_AXI_TIMER_0_INTERRUPT_INTR);
    XIntc_Enable(&intc, XPAR_AXI_INTC_0_AXI_GPIO_0_IP2INTC_IRPT_INTR);
    XIntc_Enable(&intc, XPAR_AXI_INTC_0_AXI_GPIO_2_IP2INTC_IRPT_INTR);

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
    }
    return 0;
}

// ==================== 各中断服务函数 ====================
// 以下函数由 INTC 硬件直接向量跳转，无需统一调度函数
// fast_interrupt 属性使编译器生成更精简的现场保存/恢复代码

void PushBtnHandler(void){
    switch (Xil_In32(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_DATA_OFFSET) & 0x1F)
        {
        case 1:button = 1;break;
        case 2:button = 2;break;
        case 4:button = 3;break;
        case 16:button = 4;break;
        case 8:button = 5;break;
        default:button = 0;break;
        }
    flag_btn_val = button;
    flag_btn     = 1;
}

void SwitchHandler(void){
    u16 sw = (u16)(Xil_In32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_DATA_OFFSET) & 0xFFFF);
    flag_switch_val = sw;
    flag_switch     = 1;

    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_DATA2_OFFSET, sw);   //将开关状态输出到LED
}
int counter = 0;
void seg7_TimerHandler(void){
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA2_OFFSET, segcode[button]);
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA_OFFSET, pos);
    pos = pos >> 1;
    counter++;
    if(counter==8) {
        counter = 0;
        pos = 0xff7f;
    }
}
