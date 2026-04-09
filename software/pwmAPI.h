/************************************************************
**        PWM API HEADER                                   **
************************************************************/


#ifndef __PWM_API_H__
#define __PWM_API_H__

#include "pwm_regs.h"
#include "sys/alt_irq.h"        // interrupts


/************************************************************
    PWM_ISR_INSTALL: this macro installs the PWM Interrupt Service Routine
        name: instance name used in QSys (UPPER CASE LETTERS) (it will be extended with _ISR)
        cb: callback function
************************************************************/
#define PWM_ISR_INSTALL(name, cb)							\
	void name##_ISR(void* context){							\
		IORD_PWM_STATUS(name##_BASE);						\
		cb();												\
    }

/************************************************************
	PWM_INSTANCE_INIT: this macro initializes the pwm instance "name"
		name: instance name used in QSys (UPPER CASE LETTERS)
		period: period, an integer value in ms
		dutycycle: duty cycle, an integer value in %
        Return: Success <=> 0, Error <=> -1
************************************************************/
#define PWM_INSTANCE_INIT(name, period, dutycycle)			\
	PWM_Init(												\
		name##_BASE    /*BaseAddress*/,						\
		period,												\
		dutycycle,											\
		name##_FREQ    /*ClockFrequency*/,					\
		name##_IRQ_INTERRUPT_CONTROLLER_ID   /*PWM_ic_id*/,	\
		name##_IRQ     /*PWM_irq*/,							\
		name##_ISR     /*PWM_isr*/							\
	);

	
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
char PWM_Init(unsigned long /*BaseAddress*/, unsigned long /*Period*/, unsigned char /*DutyCycle*/, unsigned long /*ClockFrequency*/, long /*PWM_ic_id*/, long /*PWM_irq*/, alt_isr_func /*isr*/);


/************************************************************
    PWM_State: returns the PWM modulator state
        BaseAddress: peripheral base address
        Return: OFF state <=> 0 / ON state <=> 1
************************************************************/
char PWM_State(unsigned long /*BaseAddress*/);


/************************************************************
    PWM_PulseState: returns the PWM modulator output state
        BaseAddress: peripheral base address
        Return: output state OFF <=> 0 / output state ON <=> 1
************************************************************/
char PWM_PulseState(unsigned long /*BaseAddress*/);


/************************************************************
    PWM_IsIrqPending: returns the interrupt state
        BaseAddress: peripheral base address
        Return: irq pending <=> 1 else 0
        WARNING: irq flag (STATUS register) is cleared when
        this function returns 1
************************************************************/
char PWM_IsIrqPending(unsigned long /*BaseAddress*/);


/************************************************************
    PWM_Start: starts the PWM modulator
        BaseAddress: peripheral base address
        Return: Success <=> 0 / Error <=> -1
************************************************************/
char PWM_Start(unsigned long /*BaseAddress*/);


/************************************************************
    PWM_Stop: stops the PWM modulator
        BaseAddress: peripheral base address
        Return: Success <=> 0 / Error <=> -1
************************************************************/
char PWM_Stop(unsigned long /*BaseAddress*/);


/************************************************************
    PWM_IrqEnable: Enables PWM modulator interruptions
        BaseAddress: peripheral base address
        Return: void
************************************************************/
void PWM_IrqEnable(unsigned long /*BaseAddress*/);


/************************************************************
    PWM_IrqDisable: Disables PWM modulator interruptions
        BaseAddress: peripheral base address
        Return: void
************************************************************/
void PWM_IrqDisable(unsigned long /*BaseAddress*/);


/************************************************************
    PWM_WritePeriod: Writes the PWM modulator period
        BaseAddress: peripheral base address
        ClockFrequency: frequency of the clock driving the pwm
        Period: period, an integer value in ms
        Return: Success <=> 0 / Error <=> -1
************************************************************/
char PWM_WritePeriod(unsigned long /*BaseAddress*/, unsigned long /*ClockFrequency*/, unsigned long /*Period*/);


/************************************************************
    PWM_ReadPeriod: Reads the PWM modulator period
        BaseAddress: peripheral base address
        ClockFrequency: frequency of the clock driving the pwm
        Return: period, an integer value in ms
************************************************************/
unsigned long PWM_ReadPeriod(unsigned long /*BaseAddress*/, unsigned long /*ClockFrequency*/);


/************************************************************
    PWM_WriteDutyCycle: Writes the PWM modulator duty cycle
        BaseAddress: peripheral base address
        DutyCycle: duty cycle, an integer value in %
        Return: Success <=> 0 / Error <=> -1
************************************************************/
char PWM_WriteDutyCycle(unsigned long /*BaseAddress*/, unsigned char /*DutyCycle*/);


/************************************************************
    PWM_ReadDutyCycle: Reads the PWM modulator duty cycle
        BaseAddress: peripheral base address
		Return: duty cycle, an integer value in %
************************************************************/
unsigned long PWM_ReadDutyCycle(unsigned long /*BaseAddress*/);


/************************************************************
    PWM_WriteNbCycles: Writes the number of cycles that generate an interruption
        BaseAddress: peripheral base address
        NbCycles: number of cycles, an integer value
        Return: Success <=> 0 / Error <=> -1
************************************************************/
char PWM_WriteNbCycles(unsigned long /*BaseAddress*/, unsigned char /*NbCycles*/);


/************************************************************
    PWM_ReadNbCycles: Reads the number of cycles that generate an interruption
        BaseAddress: peripheral base address
		Return: number of cycles, an integer value
************************************************************/
unsigned long PWM_ReadNbCycles(unsigned long /*BaseAddress*/);

#endif /* __PWM_API_H__ */
