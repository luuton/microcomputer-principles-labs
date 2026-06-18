#include "headfile.h"

/*
点击按键C,8位LED灯（LED[7:0]),显示固定初值11110000，其余LED熄灭
点击按键L，LED以频率1Hz向左循环移动，如（11110000变为11100001）
再次点击按键L，LED以频率1Hz向右循环移动；再次点击按键L，停止移动并保持当前位置显示
要求以中断方式实现。向左循环移动：LED到达最左端后，下次左移将回到最右端；向右循环同理
*/

#define Reset_Value 99999   // 100000-1: 递减到0共N+1拍, 99999+1=100000拍=1ms@100MHz

void My_ISR() __attribute__((interrupt_handler));
void PushBtnHandler();
void TimerHandler();

u8 led_buf = 0;

int init(){
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_TRI2_OFFSET, 0x0000);  //配置GPIO0 ch2(LED)为输出
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
    while(1){
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

u8 r_flag = 0;
u8 l_flag = 0;
void PushBtnHandler() {
    switch (Xil_In8((Btn_data)))
        {
        case 1://c
            for(int i=0;i<1000;i++);        //简单延时消抖
            led_buf = 0xf0;                 //重置为11110000
            Xil_Out16(led_data,led_buf);    //更新
            break;
        case 4://l
            for(int i=0;i<1000;i++);
            if(l_flag) l_flag = 0;  //再次按下重置标志位
            else {
                l_flag = 1;         //左移标志置1
                r_flag = 0;         //右移标志置0
            }
            break;
        case 8://r
            for(int i=0;i<1000;i++);
            if(r_flag) r_flag = 0;
            else {
                r_flag = 1;
                l_flag = 0;
            }
            break;
        default://0
            break;
        }
    Xil_Out32(Btn_ISR, Xil_In32(Btn_ISR));  //清除GPIO2中断
}
u16 tick = 0;
void TimerHandler() {
    if(tick >= 1000) {  //1000 x 1ms = 1s
        tick = 0;
        if(l_flag){                                     //左移标志位为1时
            if(tick >= 1000){                           //计数1s
                tick = 0;                               //重置计数
                if((led_buf&0x80) == 1)                 //最高位为1
                    led_buf = (led_buf << 1) | 0x01;    //左移一位并最低位置1
                else if((led_buf&0x80) == 0)            //最高为0
                    led_buf = (led_buf << 1) | 0x00;    //左移一位并最低位置0
                Xil_Out16(led_data,led_buf);            //更新led显示
        }
        else tick++;
        
    }
        if(r_flag){
            if(tick >= 1000){
                tick = 0;
                if((led_buf&0x01) == 1)
                    led_buf = (led_buf >> 1) | 0x80;
                else if((led_buf&0x01) == 0)
                    led_buf = (led_buf >> 1) | 0x00;
                Xil_Out16(led_data,led_buf);
            }
            else tick++;
        }

    }
    
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR));  //清除定时器中断
}