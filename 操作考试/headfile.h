#ifndef _headfile_h_
#define _headfile_h_

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
static const u8 segcode[6] = {0x00,0xc6,0xc1,0xc7,0xa1,0xce};   //c,u,l,d,r
static const u16 pos[8] = {0xff7f, 0xffbf, 0xffdf, 0xffef, 0xfff7, 0xfffb, 0xfffd, 0xfffe};  //数码管位选信号,左到右
static const u8 tableLED[9] = {
        0x00,  // 00000000
        0x01,  // 00000001
        0x03,  // 00000011
        0x07,  // 00000111
        0x0f,  // 00001111
        0x1f,  // 00011111
        0x3f,  // 00111111
        0x7f,  // 01111111
        0xff   // 11111111
    };
#endif