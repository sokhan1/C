// if you would like to use this code, you will need an S32K144 micro-processor board and LCD SCREEN

#include "device_registers.h"           // Device header
#include "clocks_and_modes.h"
#include "lcd1602A.h"
#include <stdio.h>

int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */

unsigned int i = 0;

void LPIT0_init (uint32_t delay);

void WDOG_disable (void){
	  WDOG->CNT=0xD928C520;     /* Unlock watchdog */
	  WDOG->TOVAL=0x0000FFFF;   /* Maximum timeout value */
	  WDOG->CS = 0x00002100;    /* Disable watchdog */
	}


void delay_ms (uint32_t ms){
	 LPIT0_init(ms);           /* Initialize PIT0 for delay time, timeout  */
   while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK)) {} /* Wait for LPIT0 CH0 Flag */
   lpit0_ch0_flag_counter++;         /* Increment LPIT0 timeout counter */
               LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}




void LPIT0_init (uint32_t delay){
   uint32_t timeout;
   /*!
    * LPIT Clocking:
    * ==============================
    */
  PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
  PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs       */

  /*!
   * LPIT Initialization:
   */
  LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;  /* DBG_EN-0: Timer chans stop in Debug mode */
                                        /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
                                        /* SW_RST=0: SW reset does not reset timer chans, regs */
                                        /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */

  timeout=delay * 40000;
  LPIT0->TMR[0].TVAL = timeout;      /* Chan 0 Timeout period: 40M clocks */
  LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
                              /* T_EN=1: Timer channel is enabled */
                              /* CHAIN=0: channel chaining is disabled */
                              /* MODE=0: 32 periodic counter mode */
                              /* TSOT=0: Timer decrements immediately based on restart */
                              /* TSOI=0: Timer does not stop after timeout */
                              /* TROT=0 Timer will not reload on trigger */
                              /* TRG_SRC=0: External trigger soruce */
                              /* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}



void PORT_init (void)
{
	 /*
   * ===============PORT-D SEGMENT=====================
   */

	//=========================================
	//port D 15, 14, 13 - RS, RW, ENABLE
	//port D 12, 11, 10, 9 - Transmission port(DB7, 6, 5, 4)
	//=========================================

    //PTD->PDDR |= 0xFE00;
    PTD->PDDR |= 1<<9 | 1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<14 | 1<<15;


    PCC->PCCn[PCC_PORTD_INDEX] &= ~PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_PCS(0x001);
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_FTM2_INDEX]  &= ~PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_FTM2_INDEX]  |= (PCC_PCCn_PCS(1)| PCC_PCCn_CGC_MASK);		//Clock = 80MHz

    //Pin mux
    PORTD->PCR[9]= PORT_PCR_MUX(1);
    PORTD->PCR[10]= PORT_PCR_MUX(1);
    PORTD->PCR[11]= PORT_PCR_MUX(1);
    PORTD->PCR[12]= PORT_PCR_MUX(1);
    PORTD->PCR[13]= PORT_PCR_MUX(1);
    PORTD->PCR[14]= PORT_PCR_MUX(1);
    PORTD->PCR[15]= PORT_PCR_MUX(1);

    //Output set(set 4bit, 2line - 0b 0010 0101 000x xxxx)

}



int main(void)
{

    WDOG_disable();        /* Disable WDGO*/
    SOSC_init_8MHz();      /* Initialize system oscilator for 8 MHz xtal */
    SPLL_init_160MHz();    /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
    NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
    SystemCoreClockUpdate();
    delay_ms(20);
    PORT_init();            /* Configure ports */


	char msg_array1[15]={0x48, 0x45, 0x4C, 0x4C, 0x4F, 0x5F, 0x57,0x4F,0x52, 0x4C, 0x44, 0x5F,0x00};// 1 row text-char
	lcdinit();        /* Initialize LCD1602A module*/

	delay_ms(200);

		//text-char output
		while(msg_array1[i] != '\0'){
			lcdcharinput(msg_array1[i]); // 1(first) row text-char send to LCD module
			delay_ms(800);
			i++;
		}



	//Lcd off, LCD display clear
	delay_ms(2000);
	lcdinput(0x08);	//lcd display off
	delay_ms(400);
	lcdinput(0x01);	//Clear display
	delay_ms(200);

	return 0;
}
