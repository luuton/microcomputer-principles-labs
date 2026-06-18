#include "headfile.h"
/*
初始时，八位数码管从左到右显示2026，没有数字的数码管显示-，
按R，显示数据整体向右移动，每两秒移动一格。
当6移动到最右端时，下次移动2026整体移动到最左端，
即2026不分开显示，移动过程中不显示数字的部分依然显示-，
按L，数字停止滚动，再按R，数字再次向右移动，滚动周期精确为1s，以中断形式实现。
*/
#define Reset_Value 99999   // 100000-1: 递减到0共N+1拍, 99999+1=100000拍=1ms@100MHz

void My_ISR() __attribute__((interrupt_handler));
void PushBtnHandler();
void SwitchHandler();
void TimerHandler();

int init(){
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_TRI_OFFSET, 0xffff);   //配置GPIO0 ch1(开关)为输入
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_TRI2_OFFSET, 0x0000);  //配置GPIO0 ch2(LED)为输出
    Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_TRI_OFFSET, 0x0);  //配置GPIO1 ch1数码管段选信号为输出
    Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_TRI2_OFFSET, 0x0);  //配置GPIO1 ch2数码管位选信号为输出
    Xil_Out8(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_TRI_OFFSET, 0x1f);  //按键gpio输入 控制低五位

    Xil_Out32(Btn_IER, XGPIO_IR_CH1_MASK);                  //允许GPIO2 ch1中断
    Xil_Out32(Btn_GIER, XGPIO_GIE_GINTR_ENABLE_MASK);       //允许GPIO2中断输出
    Xil_Out32(Switch_IER, XGPIO_IR_CH1_MASK);               //允许GPIO0 ch1中断
    Xil_Out32(Switch_GIER, XGPIO_GIE_GINTR_ENABLE_MASK);    //允许GPIO0全局中断

    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR) &~XTC_CSR_ENABLE_INT_MASK);  //停止计数器
    Xil_Out32(Timer_TLR, Reset_Value);                                      //预置计数器初值
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR) | XTC_CSR_LOAD_MASK);        //装载计数器初值
    Xil_Out32(Timer_TCSR, (Xil_In32(Timer_TCSR)&~XTC_CSR_LOAD_MASK)\
    |XTC_CSR_ENABLE_TMR_MASK|XTC_CSR_ENABLE_INT_MASK|XTC_CSR_AUTO_RELOAD_MASK|XTC_CSR_DOWN_COUNT_MASK);  //使能计数器中断

    Xil_Out32(intc_IER, \
        XPAR_AXI_TIMER_0_INTERRUPT_MASK|\
        XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK|\
        XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK);  //使能中断
    Xil_Out32(intc_MER, XIN_INT_MASTER_ENABLE_MASK|XIN_INT_HARDWARE_ENABLE_MASK);
    microblaze_enable_interrupts();  //使能处理器中断
    xil_printf("\r\n=== init Ready ===\r\n");
    return 0;
}

int main(){
    init();
    while(1){
    }
    return 0;
}

void My_ISR(void) {
    u32 status = Xil_In32(intc_ISR);
    
    if((status & XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK)==XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK) {  //GPIO0中断
        SwitchHandler();
    }
    if((status & XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK)==XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK) {  //GPIO2中断
        PushBtnHandler();
    }
    if((status & XPAR_AXI_TIMER_0_INTERRUPT_MASK)==XPAR_AXI_TIMER_0_INTERRUPT_MASK) {  //定时器中断
        TimerHandler();
    }
    Xil_Out32(intc_IAR, status);  //清除中断
}

const u8 seg2026_buf[4] = {0xA4,0xC0,0xA4,0x82};  //存2026
u8 seg_p = 0xf0;
void SwitchHandler() {
    Xil_Out32(Switch_ISR, Xil_In32(Switch_ISR));  //清除GPIO0中断
}

u8 r_flag = 0;
u8 l_flag = 0;
void PushBtnHandler() {
    switch (Xil_In8((Btn_data)))
        {
        case 1://c
            break;
        case 2://u
            break;
        case 4://l
            if(r_flag) r_flag = 0;
            else l_flag = 1;
            break;
        case 16://d
            break;
        case 8://r
            if(l_flag) l_flag = 0;
            else r_flag = 1;
            break;
        default://0
            break;
        }
    Xil_Out32(Btn_ISR, Xil_In32(Btn_ISR));  //清除GPIO2中断
}
u8 counter = 0;
u8 counter2026 = 0;
u32 tick = 0;
void TimerHandler() {
    if(l_flag){
        if(tick >= 1000){       //1000 x 1ms = 1s
            tick = 0;
            if(seg_p == 0xf0) seg_p = 0x0f;
            else seg_p = seg_p << 1; //左移填0
        }
        else tick++;
    }
    if(r_flag){
        if(tick >= 1000){
            tick = 0;
            if(seg_p == 0x0f) seg_p = 0xf0;
            else seg_p = seg_p >> 1; //右移填0
        }
        else tick++;
    }

    if((seg_p >> counter)&0x01){    //判断'counter'位是否为1
        Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA2_OFFSET, seg2026_buf[counter2026]);    //根据开关状态输出数码管段选信号
        counter2026++;
        if(counter2026==3) counter2026 = 0; //输出最后一位后重置
    }
    else Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA2_OFFSET, 0xfd);      //填入-
    Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA_OFFSET, pos[counter]);    //输出数码管位选信号
    counter++;
    if(counter==8) counter = 0;
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR));  //清除定时器中断
}