#include "xparameters.h"
#include "xgpio.h"
#include "xintc.h"
#include "xtmrctr.h"
#include "xuartlite_l.h"
#include "mb_interface.h"

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

#define uart1_REG       XPAR_AXI_UARTLITE_1_BASEADDR+XUL_CONTROL_REG_OFFSET
#define uart1_RX_FIFO   XPAR_AXI_UARTLITE_1_BASEADDR+XUL_RX_FIFO_OFFSET
#define uart1_TX_FIFO   XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET
#define uart1_STATUS    XPAR_AXI_UARTLITE_1_BASEADDR+XUL_STATUS_REG_OFFSET
#define uart2_REG       XPAR_AXI_UARTLITE_2_BASEADDR+XUL_CONTROL_REG_OFFSET
#define uart2_RX_FIFO   XPAR_AXI_UARTLITE_2_BASEADDR+XUL_RX_FIFO_OFFSET
#define uart2_TX_FIFO   XPAR_AXI_UARTLITE_2_BASEADDR+XUL_TX_FIFO_OFFSET
#define uart2_STATUS    XPAR_AXI_UARTLITE_2_BASEADDR+XUL_STATUS_REG_OFFSET

#define Reset_Value 100000

void My_ISR() __attribute__((interrupt_handler));
void PushBtnHandler();
void SwitchHandler();
void seg7_TimerHandler();
void UART1_Handler();
void UART2_Handler();

char segcode[6] = {0x00,0xc6,0xc1,0xc7,0xa1,0xce};   //c,u,l,d,r
short pos = 0xff7f;  //数码管位选信号
u8 button_rev = 0;  //按键状态

// UART2 接收两字节状态机 (板A UART1 TX → 板B UART2 RX, 接收16位开关值)
volatile u8 uart2_rx_state = 0;   // 0=等待低字节, 1=等待高字节
volatile u8 uart2_rx_low = 0;     // 暂存低字节

// 打印标志和数据缓冲 (ISR 写入, 主循环读出并清除)
volatile int flag_btn       = 0;    // 按钮中断触发
volatile u8  flag_btn_val   = 0;    // 按钮编号
volatile int flag_switch    = 0;    // 开关中断触发
volatile u16 flag_switch_val = 0;   // 开关值
volatile int flag_isr_debug  = 0;   // ISR 调试状态触发
volatile int flag_isr_val    = 0;   // ISR 状态寄存器值
volatile u8 flag_sent_low   = 0;   // 发送低字节触发
volatile u8 flag_sent_low_val = 0; // 发送的低字节
volatile u8 flag_sent_high  = 0;   // 发送高字节触发
volatile u8 flag_sent_high_val = 0; // 发送的高字节
volatile u8 flag_uart1_intr = 0; // UART1 中断触发
volatile u8 flag_uart2_intr = 0; // UART2 中断触发
volatile u8 flag_uart1_rev  = 0; // UART1 接收触发
volatile u8 flag_uart1_rev_val = 0; // UART1 接收的按键值
volatile u8 flag_uart2_rev  = 0; // UART2 接收触发
volatile u16 flag_uart2_rev_val = 0; // UART2 接收的开关值

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

    Xil_Out32(uart1_REG, XUL_CR_ENABLE_INTR|XUL_CR_FIFO_RX_RESET|XUL_CR_FIFO_TX_RESET);  //使能uart1中断,重置uart1的收发FIFO
    Xil_Out32(uart2_REG, XUL_CR_ENABLE_INTR|XUL_CR_FIFO_RX_RESET|XUL_CR_FIFO_TX_RESET);  //使能uart2中断

    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR) &~XTC_CSR_ENABLE_INT_MASK);  //停止计数器
    Xil_Out32(Timer_TLR, Reset_Value);                                      //预置计数器初值
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR) | XTC_CSR_LOAD_MASK);        //装载计数器初值
    Xil_Out32(Timer_TCSR, (Xil_In32(Timer_TCSR)&~XTC_CSR_LOAD_MASK)\
    |XTC_CSR_ENABLE_TMR_MASK|XTC_CSR_ENABLE_INT_MASK|XTC_CSR_AUTO_RELOAD_MASK|XTC_CSR_DOWN_COUNT_MASK);  //使能计数器中断

    Xil_Out32(intc_IER, \
        XPAR_AXI_TIMER_0_INTERRUPT_MASK|\
        XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK|\
        XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK|\
        XPAR_AXI_UARTLITE_1_INTERRUPT_MASK|\
        XPAR_AXI_UARTLITE_2_INTERRUPT_MASK);  //使能中断
    Xil_Out32(intc_MER, XIN_INT_MASTER_ENABLE_MASK|XIN_INT_HARDWARE_ENABLE_MASK);
    microblaze_enable_interrupts();  //使能处理器中断
    xil_printf("\r\n=== init Ready ===\r\n");
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
        if (flag_sent_low) {
            xil_printf("Sent low byte: %x\r\n", flag_sent_low_val);
            flag_sent_low = 0;
        }
        if (flag_sent_high) {
            xil_printf("Sent high byte: %x\r\n", flag_sent_high_val);
            flag_sent_high = 0;
        }
        if (flag_uart1_intr) {
            xil_printf("UART1 Interrupt!\r\n");
            flag_uart1_intr = 0;
        }
        if (flag_uart2_intr) {
            xil_printf("UART2 Interrupt!\r\n");
            flag_uart2_intr = 0;
        }
        if (flag_uart1_rev) {
            xil_printf("UART1 Received button: %d\r\n", flag_uart1_rev_val);
            flag_uart1_rev = 0;
        }
        if (flag_uart2_rev) {
            xil_printf("UART2 Received switch: %x\r\n", flag_uart2_rev_val);
            flag_uart2_rev = 0;
        }
    }
    return 0;
}

void My_ISR(void) {
    u32 status = Xil_In32(intc_ISR);
    
    if (status != XPAR_AXI_TIMER_0_INTERRUPT_MASK) {
        flag_isr_val   = status;
        flag_isr_debug = 1;
    }
    
    if(((status & XPAR_AXI_UARTLITE_2_INTERRUPT_MASK)==XPAR_AXI_UARTLITE_2_INTERRUPT_MASK)) {  //uart2中断
        UART2_Handler();
    }
    if(((status & XPAR_AXI_UARTLITE_1_INTERRUPT_MASK)==XPAR_AXI_UARTLITE_1_INTERRUPT_MASK)) {  //uart1中断
        UART1_Handler();
    }
    
    if((status & XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK)==XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK) {  //GPIO0中断
        SwitchHandler();
    }
    if((status & XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK)==XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK) {  //GPIO2中断
        PushBtnHandler();
    }
    if((status & XPAR_AXI_TIMER_0_INTERRUPT_MASK)==XPAR_AXI_TIMER_0_INTERRUPT_MASK) {  //定时器中断
        seg7_TimerHandler();
    }
    Xil_Out32(intc_IAR, status);  //清除中断
}

void PushBtnHandler(){
    u8 button;
    switch (Xil_In8((XPAR_AXI_GPIO_2_BASEADDR+XGPIO_DATA_OFFSET)))
        {
        case 1:button = 1;break;
        case 2:button = 2;break;
        case 4:button = 3;break;
        case 16:button = 4;break;
        case 8:button = 5;break;
        default:button = 0;break;
        }
        if((Xil_In32(uart2_STATUS)&XUL_SR_TX_FIFO_EMPTY)==XUL_SR_TX_FIFO_EMPTY){
            Xil_Out8(uart2_TX_FIFO, button);  //将按键状态通过uart2发送出去
        }
    flag_btn_val = button;
    flag_btn     = 1;
    Xil_Out32(Btn_ISR, Xil_In32(Btn_ISR));  //清除GPIO2中断
}

void SwitchHandler(){
    u16 sw = Xil_In16(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_DATA_OFFSET);  //读取开关状态 
    flag_switch_val = sw;
    flag_switch     = 1;

    /*注意！uart一次只能发送八位，而开关有16个——分两次发送，中间等待FIFO空*/
    if((Xil_In32(uart1_STATUS)&XUL_SR_TX_FIFO_EMPTY)==XUL_SR_TX_FIFO_EMPTY) {  //uart1发送FIFO为空
        Xil_Out8(uart1_TX_FIFO, sw & 0xff);            //发送低八位
        flag_sent_low_val = sw & 0xff;
        flag_sent_low = 1;
        while(!(Xil_In32(uart1_STATUS) & XUL_SR_TX_FIFO_EMPTY));  //等待FIFO空
        Xil_Out8(uart1_TX_FIFO, (sw >> 8) & 0xff);     //发送高八位
        flag_sent_high_val = (sw >> 8) & 0xff;
        flag_sent_high = 1;
    }
    Xil_Out32(Switch_ISR, Xil_In32(Switch_ISR));  //清除GPIO0中断
}

int counter = 0;
void seg7_TimerHandler(){
    // xil_printf("Timer Interrupt! button: %d\r\n", button);
    Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA2_OFFSET, segcode[button_rev]);    //根据开关状态输出数码管段选信号
    Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA_OFFSET, pos);                 //输出数码管位选信号
    pos = pos >> 1;  //位选信号循环右移
    counter++;
    if(counter==8) {
        counter = 0;
        pos = 0xff7f;  //重置位选信号
    }
    Xil_Out32(Timer_TCSR, Xil_In32(Timer_TCSR));  //清除定时器中断
}

void UART1_Handler(){
    u32 status = Xil_In32(uart1_STATUS);
    flag_uart1_intr = 1;
    
    if(status&XUL_SR_RX_FIFO_VALID_DATA) {  //接收FIFO非空
        u8 data = (u8)Xil_In32(uart1_RX_FIFO);  //读取接收到的按键值
        flag_uart1_rev_val = data;
        flag_uart1_rev = 1;
        button_rev = data & 0x7;                 //用低3位更新数码管方向
    }
    Xil_Out32(uart1_REG, 0x13);
}

void UART2_Handler(){
    //用while在一次中断中读完
    flag_uart2_intr = 1;
    while(Xil_In32(uart2_STATUS) & XUL_SR_RX_FIFO_VALID_DATA) {
        u8 data = (u8)Xil_In32(uart2_RX_FIFO);
        if(uart2_rx_state == 0) {                //收到低字节
            uart2_rx_low = data;
            uart2_rx_state = 1;                  //等待高字节
        } else {                                 //收到高字节，拼装完整16位开关值
            u16 sw = uart2_rx_low | ((u16)data << 8);
            flag_uart2_rev = 1;
            flag_uart2_rev_val = sw;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_DATA2_OFFSET, sw);
            uart2_rx_state = 0;
        }
    }
    Xil_Out32(uart2_REG, 0x13);
}
