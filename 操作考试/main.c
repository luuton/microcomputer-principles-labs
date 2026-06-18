/*
初始数码管低两位显示0，按下按键u后显示按键u按下的次数，大于20时清0，按下L清零。
中断实现，注意按键消抖
*/

#include "xgpio.h"
#include "xintc.h"
#include "xtmrctr.h"
#include "xparameters.h"
#include "mb_interface.h"

#define Timer_TCSR      XPAR_AXI_TIMER_0_BASEADDR+XTC_TCSR_OFFSET
#define Timer_TLR       XPAR_AXI_TIMER_0_BASEADDR+XTC_TLR_OFFSET
#define Timer_TCR       XPAR_AXI_TIMER_0_BASEADDR+XTC_TCR_OFFSET

#define intc_ISR        XPAR_AXI_INTC_0_BASEADDR+XIN_ISR_OFFSET
#define intc_IER        XPAR_AXI_INTC_0_BASEADDR+XIN_IER_OFFSET
#define intc_IAR        XPAR_AXI_INTC_0_BASEADDR+XIN_IAR_OFFSET
#define intc_MER        XPAR_AXI_INTC_0_BASEADDR+XIN_MER_OFFSET

#define Btn_data        XPAR_AXI_GPIO_2_BASEADDR+XGPIO_DATA_OFFSET
#define Btn_ISR         XPAR_AXI_GPIO_2_BASEADDR+XGPIO_ISR_OFFSET
#define Btn_IER         XPAR_AXI_GPIO_2_BASEADDR+XGPIO_IER_OFFSET
#define Btn_GIER        XPAR_AXI_GPIO_2_BASEADDR+XGPIO_GIE_OFFSET
#define Switch_data     XPAR_AXI_GPIO_0_BASEADDR+XGPIO_DATA_OFFSET
#define Switch_ISR      XPAR_AXI_GPIO_0_BASEADDR+XGPIO_ISR_OFFSET
#define Switch_IER      XPAR_AXI_GPIO_0_BASEADDR+XGPIO_IER_OFFSET
#define Switch_GIER     XPAR_AXI_GPIO_0_BASEADDR+XGPIO_GIE_OFFSET

#define led_data        XPAR_AXI_GPIO_0_BASEADDR+XGPIO_DATA2_OFFSET
#define segment_data    XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA2_OFFSET
#define anode_data      XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA_OFFSET

// 数码管段码表（共阳极，段选低有效）: bit0=a, bit1=b, ..., bit6=g, bit7=dp
static const u8 seg7_table[10] = {
	0xC0, // 0: abcdef
	0xF9, // 1: bc
	0xA4, // 2: abdeg
	0xB0, // 3: abcdg
	0x99, // 4: bcfg
	0x92, // 5: acdfg
	0x82, // 6: acdefg
	0xF8, // 7: abc
	0x80, // 8: abcdefg
	0x90  // 9: abcfg
};
static const u16 pos[2] = {0xfffe, 0xfffd};  //数码管位选信号,只需要低两位

#define Reset_Value 99999   // 100000-1: 递减到0共N+1拍, 99999+1=100000拍=1ms@100MHz
u8 seg_buf[2];
u8 u_count = 0;

void My_ISR() __attribute__((interrupt_handler));
void PushBtnHandler();
void TimerHandler();

int init(){
    Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_TRI_OFFSET, 0x0);  //配置GPIO1 ch1数码管段选信号为输出
    Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_TRI2_OFFSET, 0x0);  //配置GPIO1 ch2数码管位选信号为输出
    Xil_Out8(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_TRI_OFFSET, 0x1f);  //按键gpio输入 控制低五位

    Xil_Out32(Btn_IER, XGPIO_IR_CH1_MASK);                  //允许GPIO2 ch1中断
    Xil_Out32(Btn_GIER, XGPIO_GIE_GINTR_ENABLE_MASK);       //允许GPIO2中断输出

    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR) &~XTC_CSR_ENABLE_INT_MASK);  //停止计数器
    Xil_Out32(Timer_TLR, Reset_Value);                                      //预置计数器初值
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR) | XTC_CSR_LOAD_MASK);        //装载计数器初值
    Xil_Out32(Timer_TCSR, (Xil_In32(Timer_TCSR)&~XTC_CSR_LOAD_MASK)\
    |XTC_CSR_ENABLE_TMR_MASK|XTC_CSR_ENABLE_INT_MASK|XTC_CSR_AUTO_RELOAD_MASK|XTC_CSR_DOWN_COUNT_MASK);  //使能计数器中断

    Xil_Out32(intc_IER, \
        XPAR_AXI_TIMER_0_INTERRUPT_MASK|\
        XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK);  //使能中断
    Xil_Out32(intc_MER, XIN_INT_MASTER_ENABLE_MASK|XIN_INT_HARDWARE_ENABLE_MASK);
    microblaze_enable_interrupts();  //使能处理器中断
    xil_printf("\r\n=== init Ready ===\r\n");
    return 0;
}

int main(){
    init();
    //初始化seg_buf为0
    for(int i=0;i<2;i++) {
        seg_buf[i] = seg7_table[0];
    }
    while(1){
        xil_printf("ucount = %d\r\n",u_count);
        for(int i=0;i<2;i++){
            xil_printf("segbuf[%d]=%x\r\n",i,seg_buf[i]);
        }
    }
    return 0;
}

void My_ISR(void) {
    u32 status = Xil_In32(intc_ISR);
    if((status & XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK)==XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK) {  //GPIO2中断
        PushBtnHandler();
    }
    if((status & XPAR_AXI_TIMER_0_INTERRUPT_MASK)==XPAR_AXI_TIMER_0_INTERRUPT_MASK) {  //定时器中断
        TimerHandler();
    }
    Xil_Out32(intc_IAR, status);  //清除中断
}


void PushBtnHandler() {
    switch (Xil_In8((Btn_data)))
        {
        case 2://u
            //延时消抖
            for(int i=0;i<Reset_Value;i++); //1ms
            if(u_count == 20)               //重置计数
                u_count = 0;
            else u_count++;                 //正常计数
            break;
        case 4://l
            for(int i=0;i<Reset_Value;i++);
            u_count = 0;                    //重置计数
            break;
        default://0
            break;
        }
    Xil_Out32(Btn_ISR, Xil_In32(Btn_ISR));  //清除GPIO2中断
}

u8 Timercount = 0;
void TimerHandler() {
    seg_buf[0] = seg7_table[u_count % 10];  //取十位，查表，存入seg_buf
    seg_buf[1] = seg7_table[u_count / 10];  //取个位，查表
    // seg_buf与pos一一对应
    Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA2_OFFSET, seg_buf[Timercount]);    //输出数码管段选信号
    Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA_OFFSET, pos[Timercount]);    //输出数码管位选信号
    if(Timercount==1)   //分别显示两位
        Timercount = 0;
    else Timercount++;
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR));  //清除定时器中断
}