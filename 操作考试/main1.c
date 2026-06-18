#include "headfile.h"
/*
拨动开关SW，读取SW[2:0]，根据其值显示到LED[7:0]。例如拨动SW=101,LED点亮5个。
按L，点亮数+1;按R，点亮数-1；超过8回到1，减小超过1回到8。
采用中断实现。
*/

void My_ISR() __attribute__((interrupt_handler));
void PushBtnHandler();
void SwitchHandler();

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

    Xil_Out32(intc_IER, \
        XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK|\
        XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK);  //使能中断
    Xil_Out32(intc_MER, XIN_INT_MASTER_ENABLE_MASK|XIN_INT_HARDWARE_ENABLE_MASK);
    microblaze_enable_interrupts();  //使能处理器中断
    xil_printf("\r\n=== init Ready ===\r\n");
    return 0;
}

int main(){
    init();
    Xil_Out8(led_data,tableLED[led_count]);
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
    Xil_Out32(intc_IAR, status);  //清除中断
}

static u8 led_count = 1;
void SwitchHandler() {
    u16 sw = Xil_In16(Switch_data);  //读取开关状态
    led_count = sw&0x07;
    Xil_Out8(led_data,tableLED[led_count]);
    Xil_Out32(Switch_ISR, Xil_In32(Switch_ISR));  //清除GPIO0中断
}

void PushBtnHandler() {
    u8 button;
    switch (Xil_In8((Btn_data)))
        {
        case 1://c
            button = 1;
            break;
        case 2://u
            button = 2;
            break;
        case 4://l
            if(led_count > 8){
                led_count = 1;
                Xil_Out8(led_data,tableLED[led_count]);
            } 
            else{
                led_count++;
                Xil_Out8(led_data,tableLED[led_count]);
            }
            button = 3;
            break;
        case 16://d
            if(led_count < 1){
                led_count = 8;
                Xil_Out8(led_data,tableLED[led_count]);
            }
            else{
                led_count--;
                Xil_Out8(led_data,tableLED[led_count]);
            }
            button = 4;
            break;
        case 8://r
            button = 5;
            break;
        default://0
            button = 0;
            break;
        }
    Xil_Out32(Btn_ISR, Xil_In32(Btn_ISR));  //清除GPIO2中断
}
