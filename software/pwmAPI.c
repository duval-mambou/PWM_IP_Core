/************************************************************
**        PWM API SOURCE                                   **
************************************************************/
#include <stdio.h>
#include "pwmAPI.h"


/************************************************************
    PWM_Init: PWM modulator initialization
        BaseAddress: peripheral base address
        Period: period, an integer value in ms
		DutyCycle: duty cycle, an integer value in %
		ClockFrequency: frequency of the clock driving the pwm
		PWM_ic_id: PWM interrupt controller ID
		PWM_irq: PWM irq number
		isr: interrupt service routine
        Return: Success <=> 0, Error <=> -1
************************************************************/
char PWM_Init(unsigned long BaseAddress, unsigned long PeriodValue, unsigned char DutyCycleValue, unsigned long ClockFrequency, long PWM_ic_id, long PWM_irq, alt_isr_func isr){
    volatile unsigned long NewPeriod;
	volatile unsigned long NewDutyCycle;
	
	//Modulator stopped (if not) before initializing the parameters
	if(PWM_Stop(BaseAddress)){
		printf("Error : Function PWM_Init => PWM_Stop failed.\n");
		return((char)-1);
	}
	//Calculation of Period and Duty cycle
	if (DutyCycleValue > 100){
	    printf("Error : Function PWM_Init => Duty Cycle greater than 100.\n");
		return((char)-1);
	}
	else{
		NewPeriod = (ClockFrequency/1000)*PeriodValue;
		NewDutyCycle = (NewPeriod/100)*DutyCycleValue;
	}	
	//Write the new parameters and verify
	IOWR_PWM_PERIOD(BaseAddress, NewPeriod);
	IOWR_PWM_DCYCLE(BaseAddress, NewDutyCycle);
	if ((IORD_PWM_PERIOD(BaseAddress) != NewPeriod) || (IORD_PWM_DCYCLE(BaseAddress) != NewDutyCycle)){
	    printf("Error : Function PWM_Init => Parameters not correctly initialized.\n");
		return((char)-1);
	}
	if(PWM_irq != -1){
		if(alt_ic_isr_register(PWM_ic_id, PWM_irq, isr, NULL, NULL)){
			printf("Error : Function PWM_Init => Cannot install interruption.\n");
			return((char)-1);
		}
	}
	//Success
	return((char)0);
}

/************************************************************
    PWM_State: returns the PWM modulator state
        BaseAddress: peripheral base address
        Return: OFF state <=> 0 / ON state <=> 1
************************************************************/
char PWM_State(unsigned long BaseAddress){
	return((char)(IORD_PWM_STATUS(BaseAddress) & PWM_STATUS_PWMSTATE_MSK) >> PWM_STATUS_PWMSTATE_OFST);
}

/************************************************************
    PWM_PulseState: returns the PWM modulator output state
        BaseAddress: peripheral base address
        Return: output state OFF <=> 0 / output state ON <=> 1
************************************************************/
char PWM_PulseState(unsigned long BaseAddress){
	return((char)(IORD_PWM_STATUS(BaseAddress) & PWM_STATUS_PULSESTATE_MSK) >> PWM_STATUS_PULSESTATE_OFST);
}

/************************************************************
    PWM_IsIrqPending: returns the interrupt state
        BaseAddress: peripheral base address
        Return: irq pending <=> 1 else 0
        WARNING: irq flag (STATUS register) is cleared when
        this function returns 1
************************************************************/
char PWM_IsIrqPending(unsigned long BaseAddress){
	return((char)(IORD_PWM_STATUS(BaseAddress) & PWM_STATUS_IRQRQ_MSK) >> PWM_STATUS_IRQRQ_OFST);
}

/************************************************************
    PWM_Start: starts the PWM modulator
        BaseAddress: peripheral base address
        Return: Success <=> 0 / Error <=> -1
************************************************************/
char PWM_Start(unsigned long BaseAddress){
	IOWR_PWM_SETCTRL(BaseAddress, PWM_CTRL_START_MSK);
	if ((IORD_PWM_STATUS(BaseAddress) & PWM_STATUS_PWMSTATE_MSK) !=  PWM_STATUS_PWMSTATE_MSK){
	    printf("Error : Function PWM_Start => Cannot start PWM.\n");
		return((char)-1);
	}
	//Success
	return((char)0);
}

/************************************************************
    PWM_Stop: stops the PWM modulator
        BaseAddress: peripheral base address
        Return: Success <=> 0 / Error <=> -1
************************************************************/
char PWM_Stop(unsigned long BaseAddress){
	IOWR_PWM_CLRCTRL(BaseAddress, PWM_CTRL_START_MSK);
	if ((IORD_PWM_STATUS(BaseAddress) & PWM_STATUS_PWMSTATE_MSK) ==  PWM_STATUS_PWMSTATE_MSK){
	    printf("Error : Function PWM_Stop => Cannot stop PWM.\n");
		return((char)-1);
	}
	//Success
	return((char)0);
}

/************************************************************
    PWM_IrqEnable: Enables PWM modulator interruptions
        BaseAddress: peripheral base address
        Return: void
************************************************************/
void PWM_IrqEnable(unsigned long BaseAddress){
	IOWR_PWM_SETCTRL(BaseAddress, PWM_CTRL_IRQEN_MSK);
}

/************************************************************
    PWM_IrqDisable: Disables PWM modulator interruptions
        BaseAddress: peripheral base address
        Return: void
************************************************************/
void PWM_IrqDisable(unsigned long BaseAddress){
	IOWR_PWM_CLRCTRL(BaseAddress, PWM_CTRL_IRQEN_MSK);
}

/************************************************************
    PWM_WritePeriod: Writes the PWM modulator period
        BaseAddress: peripheral base address
        ClockFrequency: frequency of the clock driving the pwm
        Period: period, an integer value in ms
        Return: Success <=> 0 / Error <=> -1
************************************************************/
char PWM_WritePeriod(unsigned long BaseAddress, unsigned long ClockFrequency, unsigned long PeriodValue){
    volatile unsigned long NewPeriod;
	volatile unsigned long NewDutyCycle;
	volatile unsigned long IsPWMRunning;

	if((IORD_PWM_STATUS(BaseAddress) & PWM_STATUS_PWMSTATE_MSK ) == PWM_STATUS_PWMSTATE_MSK ){
		IsPWMRunning = 1;
	}
	else{
		IsPWMRunning = 0;
	}

	NewPeriod = (ClockFrequency/1000)*PeriodValue;
	NewDutyCycle = (NewPeriod/100)*((100*IORD_PWM_DCYCLE(BaseAddress))/IORD_PWM_PERIOD(BaseAddress));

	//Modulator stopped before modifying the parameters
	if(IsPWMRunning == 1){
		if(PWM_Stop(BaseAddress) != 0){
			printf("Error : Function PWM_WritePeriod => PWM_Stop failed.\n");
			return((char)-1);
		}
	}
	//Writes the new parameters and verify
	IOWR_PWM_PERIOD(BaseAddress, NewPeriod);
	IOWR_PWM_DCYCLE(BaseAddress, NewDutyCycle);
	if ((IORD_PWM_PERIOD(BaseAddress) != NewPeriod) || (IORD_PWM_DCYCLE(BaseAddress) != NewDutyCycle)){
	    printf("Error : Function PWM_WritePeriod => Cannot modify parameters.\n");
		return((char)-1);
	}
	//Starts the modulator if necessary
	if(IsPWMRunning == 1){
		if(PWM_Start(BaseAddress) != 0){
			printf("Error : Function PWM_WritePeriod => PWM_Start failed.\n");
			return((char)-1);
		}
	}
	//Success
	return((char)0);
}

/************************************************************
    PWM_ReadPeriod: Reads the PWM modulator period
        BaseAddress: peripheral base address
        ClockFrequency: frequency of the clock driving the pwm
        Return: period, an integer value in ms
************************************************************/
unsigned long PWM_ReadPeriod(unsigned long BaseAddress, unsigned long ClockFrequency){
	return((unsigned long)((IORD_PWM_PERIOD(BaseAddress)*1000.0)/ClockFrequency+0.5));
}

/************************************************************
    PWM_WriteDutyCycle: Writes the PWM modulator duty cycle
        BaseAddress: peripheral base address
        DutyCycle: duty cycle, an integer value in %
        Return: Success <=> 0 / Error <=> -1
************************************************************/
char PWM_WriteDutyCycle(unsigned long BaseAddress, unsigned char DutyCycleValue){
	volatile unsigned long NewDutyCycle;
	
	//New duty cycle calculation
	if (DutyCycleValue > 100){
	    printf("Error : Function PWM_WriteDutyCycle => DutyCycleValue greater than 100.\n");
		return((char)-1);
	}
	else{
		NewDutyCycle = (IORD_PWM_PERIOD(BaseAddress)*DutyCycleValue)/100;
	}

	//Writes the new parameter and verify
	IOWR_PWM_DCYCLE(BaseAddress, NewDutyCycle);
	if (IORD_PWM_DCYCLE(BaseAddress) != NewDutyCycle){
	    printf("Error : Function PWM_WriteDutyCycle => Cannot modify parameter.\n");
		return((char)-1);
	}
	//Success
	return((char)0);
}

/************************************************************
    PWM_ReadDutyCycle: Reads the PWM modulator duty cycle
        BaseAddress: peripheral base address
		Return: duty cycle, an integer value in %
************************************************************/
unsigned long PWM_ReadDutyCycle(unsigned long BaseAddress){
	return((unsigned long)(100.0*IORD_PWM_DCYCLE(BaseAddress)/IORD_PWM_PERIOD(BaseAddress) + 0.5));
}

/************************************************************
    PWM_WriteNbCycles: Writes the number of cycles that generate an interruption
        BaseAddress: peripheral base address
        NbCycles: number of cycles, an integer value
        Return: Success <=> 0 / Error <=> -1
************************************************************/
char PWM_WriteNbCycles(unsigned long BaseAddress, unsigned char NbCycles){
	IOWR_PWM_NBCYCLES(BaseAddress, NbCycles);
	if ((IORD_PWM_NBCYCLES(BaseAddress) & PWM_NBCYCLES_MSK) != NbCycles){
	    printf("Error : Function PWM_WriteNbCycles => Cannot write number of cycles for interruption.\n");
		return((char)-1);
	}
	//Success
	return((char)0);
}

/************************************************************
    PWM_ReadNbCycles: Reads the number of cycles that generate an interruption
        BaseAddress: peripheral base address
		Return: number of cycles, an integer value
************************************************************/
unsigned long PWM_ReadNbCycles(unsigned long BaseAddress){
	return((unsigned long)IORD_PWM_NBCYCLES(BaseAddress) & PWM_NBCYCLES_MSK);
}
