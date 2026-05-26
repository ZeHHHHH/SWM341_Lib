#include "SWM341.h"


/* Use asymmetric center alignment mode, keep duty cycle unchanged and realize phase shift function */


int main(void)
{
	PWM_InitStructure  PWM_initStruct;
	
	SystemInit();
	
	PORT_Init(PORTM, PIN1,  PORTM_PIN1_PWM0A,   0);
	PORT_Init(PORTM, PIN4,  PORTM_PIN4_PWM0AN,  0);
	PORT_Init(PORTM, PIN2,  PORTM_PIN2_PWM0B,   0);
	PORT_Init(PORTM, PIN5,  PORTM_PIN5_PWM0BN,  0);
	
	PORT_Init(PORTM, PIN3,  PORTM_PIN3_PWM1A,   0);
	PORT_Init(PORTM, PIN6,  PORTM_PIN6_PWM1AN,  0);
	PORT_Init(PORTD, PIN9,  PORTD_PIN9_PWM1B,   0);
	PORT_Init(PORTD, PIN8,  PORTD_PIN8_PWM1BN,  0);
	
	PWM_initStruct.Mode = PWM_ASYM_CENTER_ALIGNED;
	PWM_initStruct.Clkdiv = 4;					// F_PWM = 20M/4 = 5M
	PWM_initStruct.Period = 10000;				// 5M/(10000 + 10000)= 250Hz
	PWM_initStruct.HdutyA =  2500;				// (2500 + 2500)/(10000 + 10000) = 25%
	PWM_initStruct.HdutyA2 = 2500;
	PWM_initStruct.DeadzoneA = 50;				// 50/5M = 10us
	PWM_initStruct.IdleLevelA = 0;
	PWM_initStruct.IdleLevelAN= 0;
	PWM_initStruct.OutputInvA = 0;
	PWM_initStruct.OutputInvAN= 0;
	PWM_initStruct.HdutyB =  7500;				// (7500 + 7500)/(10000 + 10000) = 75%
	PWM_initStruct.HdutyB2 = 7500;
	PWM_initStruct.DeadzoneB = 50;				// 50/5M = 10us
	PWM_initStruct.IdleLevelB = 0;
	PWM_initStruct.IdleLevelBN= 0;
	PWM_initStruct.OutputInvB = 0;
	PWM_initStruct.OutputInvBN= 0;
	PWM_initStruct.UpOvfIE    = 0;
	PWM_initStruct.DownOvfIE  = 0;
	PWM_initStruct.UpCmpAIE   = 0;
	PWM_initStruct.DownCmpAIE = 0;
	PWM_initStruct.UpCmpBIE   = 0;
	PWM_initStruct.DownCmpBIE = 0;
	PWM_Init(PWM0, &PWM_initStruct);
	PWM_Init(PWM1, &PWM_initStruct);
	
	PWM_IntEn(PWM1, PWM_IT_OVF_DOWN);
	NVIC_EnableIRQ(PWM1_IRQn);
	
	PWM_Start(PWM0_MSK|PWM1_MSK);
	
	while(1==1)
	{
	}
}


void PWM1_Handler(void)
{
	static int dir = 0;
	static int n = 0;
	
	if(PWM_IntStat(PWM1, PWM_IT_OVF_DOWN))
	{
		PWM_IntClr(PWM1, PWM_IT_OVF_DOWN);
		
		if(++n == 2)
			n = 0;
		else
			return;
		
		PWMG->RELOADEN = 0x00;		// working register reload disable and ensure that all registers are updated at same time.
		
		if(dir == 0)
		{
			if(PWM1->CMPA2 == 250)
			{
				PWM1->CMPA = 2500;
				PWM1->CMPA2 = 2500;
				PWM1->CMPB = 7500;
				PWM1->CMPB2 = 7500;
				
				dir = 1;
			}
			else
			{
				PWM1->CMPA += 250;		// increase high level duration of the first half cycle
				PWM1->CMPA2 -= 250;		// decrease high level duration of the second half cycle, the high level shifts to the right
				PWM1->CMPB += 750;
				PWM1->CMPB2 -= 750;
			}
		}
		else
		{
			if(PWM1->CMPA == 250)
			{
				PWM1->CMPA = 2500;
				PWM1->CMPA2 = 2500;
				PWM1->CMPB = 7500;
				PWM1->CMPB2 = 7500;
				
				dir = 0;
			}
			else
			{
				PWM1->CMPA -= 250;		// decrease high level duration of the first half cycle
				PWM1->CMPA2 += 250;		// increase high level duration of the second half cycle, the high level shifts to the left
				PWM1->CMPB -= 750;
				PWM1->CMPB2 += 750;
			}
		}
		
		PWMG->RELOADEN = 0x3F;
	}
}
