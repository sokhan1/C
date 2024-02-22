// you should use s32k144, 7-segment and ADC
// Lighting LEDs via ADC
// For example, if you change the value of ADC to 0~625, the first LED will be turned on
#include "device_registers.h"
#include "clocks_and_modes.h"
#include "ADC.h"

void PORT_init (void)
{
	/*!
	 * Pins definitions
	 * ===================================================
	 *
	 * Pin number        | Function
	 * ----------------- |------------------
	 * PTD0              | GPIO [BLUE LED]
	 * PTD15             | GPIO [RED LED]
	 * PTD16	     | GPIO [GREEN LED]
	 */
	  PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK;   /* Enable clock for PORTD */
	  PTD->PDDR |= 1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8;	/* Port D1~3:  Data Direction = output */
	  PTD->PSOR |= 1<<1|1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8;
	  PORTD->PCR[1]  = PORT_PCR_MUX(1);	/* Port D1: MUX = GPIO  */
	  PORTD->PCR[2]  = PORT_PCR_MUX(1);	/* Port D2: MUX = GPIO  */
	  PORTD->PCR[3]  = PORT_PCR_MUX(1);	/* Port D3: MUX = GPIO  */
	  PORTD->PCR[4]  = PORT_PCR_MUX(1);	/* Port D1: MUX = GPIO  */
	  PORTD->PCR[5]  = PORT_PCR_MUX(1);	/* Port D2: MUX = GPIO  */
	  PORTD->PCR[6]  = PORT_PCR_MUX(1);	/* Port D3: MUX = GPIO  */
	  PORTD->PCR[7]  = PORT_PCR_MUX(1);	/* Port D3: MUX = GPIO  */
	  PORTD->PCR[8]  = PORT_PCR_MUX(1);	/* Port D3: MUX = GPIO  */
}

void WDOG_disable (void)
{
  WDOG->CNT=0xD928C520;     /* Unlock watchdog 		*/
  WDOG->TOVAL=0x0000FFFF;   /* Maximum timeout value 	*/
  WDOG->CS = 0x00002100;    /* Disable watchdog 		*/
}

int main(void)
{
  uint32_t adcResultInMv=0;	/*< ADC0 Result in miliVolts */

	/*!
	 * Initialization:
	 * =======================
	 */
  WDOG_disable();        /* Disable WDOG												*/
  SOSC_init_8MHz();      /* Initialize system oscillator for 8 MHz xtal 				*/
  SPLL_init_160MHz();    /* Initialize SPLL to 160 MHz with 8 MHz SOSC 				*/
  NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash*/
  PORT_init();		     /* Init  port clocks and gpio outputs 						*/
  ADC_init();            /* Init ADC resolution 12 bit									*/

	/*!
	 * Infinite for:
	 * ========================
	 */
	  for(;;)
	  {
		convertAdcChan(13);                   /* Convert Channel AD13 to pot on EVB 	*/
		while(adc_complete()==0){}            /* Wait for conversion complete flag 	*/
		adcResultInMv = read_adc_chx();       /* Get channel's conversion results in mv */

		if (adcResultInMv > 4375) {           /* If result > 3.75V 		*/
		  PTD->PCOR |= 1<<8;
			PTD->PSOR |= 1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<1;
		}
		else if (adcResultInMv > 3750) {      /* If result > 2.50V 		*/
		  PTD->PCOR |= 1<<7;
			PTD->PSOR |= 1<<2|1<<3|1<<4|1<<5|1<<6|1<<1|1<<8;
		}
		else if (adcResultInMv >3125) {       /* If result > 1.25V 		*/
		  PTD->PCOR |= 1<<6;
			PTD->PSOR |= 1<<2|1<<3|1<<4|1<<5|1<<1|1<<7|1<<8;
		}
		else if (adcResultInMv >2500){
		  PTD->PCOR |= 1<<5;
			PTD->PSOR |= 1<<2|1<<3|1<<4|1<<1|1<<6|1<<7|1<<8;
		}
		else if (adcResultInMv > 1875) {           /* If result > 3.75V 		*/
		  PTD->PCOR |= 1<<4;
			PTD->PSOR |= 1<<2|1<<3|1<<1|1<<5|1<<6|1<<7|1<<8;;
		}
		else if (adcResultInMv > 1250) {      /* If result > 2.50V 		*/
		  PTD->PCOR |= 1<<3;
			PTD->PSOR |= 1<<2|1<<1|1<<4|1<<5|1<<6|1<<7|1<<8;
		}
		else if (adcResultInMv >625) {       /* If result > 1.25V 		*/
		  PTD->PCOR |= 1<<2;
			PTD->PSOR |= 1<<1|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8;
		}
		else{
		  PTD->PCOR |= 1<<1;
			PTD->PSOR |= 1<<2|1<<3|1<<4|1<<5|1<<6|1<<7|1<<8;
		}

	  }
}
