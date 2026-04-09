/****************************************************
**  PWM                                            **
**   32 bits device for Altera/Avalon interface    **
*****************************************************

*****************************************************
**  Register mapping                               **
*****************************************************
*   Register      |  Reg Num  |  Access  |  Reset   *
******************|***********|**********|***********
*   Status        |    0      |  RO      |  0       *
*   SetCtrl       |    0      |  WO      |see note 1*
*   Period        |    1      |  R/W     |0xffffffff*
*   DCycle        |    2      |  R/W     |  0       *
*   NbCycles      |    3      |  R/W     |0xffff    *
*   ClrCtrl       |    4      |  WO      |see note 1*
****************************************************/
//note 1: internal Ctrl register is initialized at 0


#ifndef __PWM_REGS_H__
#define __PWM_REGS_H__

#include <io.h>

#define	STATUS		0
#define	SETCTRL		0
#define	PERIOD		1
#define	DCYCLE	    2
#define NBCYCLES    3
#define CLRCTRL     4


/****************************************************
**        Access Macros                            **
****************************************************/
//STATUS Register, 0, RO
#define	IORD_PWM_STATUS(base)			IORD(base, STATUS)

//SETCTRL Register, 0, WO
#define	IOWR_PWM_SETCTRL(base, donnee)	IOWR(base, SETCTRL, donnee)

//PERIOD Register, 1, R/W
#define	IORD_PWM_PERIOD(base)			IORD(base, PERIOD)
#define	IOWR_PWM_PERIOD(base, donnee)	IOWR(base, PERIOD, donnee)

//DCYCLE Register, 2, R/W
#define	IORD_PWM_DCYCLE(base)			IORD(base, DCYCLE)
#define	IOWR_PWM_DCYCLE(base, donnee)	IOWR(base, DCYCLE, donnee)

//NBCYCLES Register, 3, R/W
#define	IORD_PWM_NBCYCLES(base)			IORD(base, NBCYCLES)
#define	IOWR_PWM_NBCYCLES(base, donnee)	IOWR(base, NBCYCLES, donnee)

//CLRCTRL Register, 4, WO
#define	IOWR_PWM_CLRCTRL(base, donnee)	IOWR(base, CLRCTRL, donnee)


/****************************************************
**        Maks & Offsets                           **
****************************************************/
//STATUS Register, RO
#define PWM_STATUS_PWMSTATE_MSK         (0x00000001)
#define PWM_STATUS_PWMSTATE_OFST        (0)
#define PWM_STATUS_PULSESTATE_MSK       (0x00000002)
#define PWM_STATUS_PULSESTATE_OFST      (1)
#define PWM_STATUS_IRQRQ_MSK            (0x00000004)
#define PWM_STATUS_IRQRQ_OFST           (2)

//CTRL Register, WO
#define	PWM_CTRL_START_MSK              (0x00000001)
#define	PWM_CTRL_START_OFST             (0)
#define	PWM_CTRL_IRQEN_MSK              (0x00000002)
#define	PWM_CTRL_IRQEN_OFST             (1)

////NBCYCLES Register, R/W
#define PWM_NBCYCLES_MSK                (0xffff)
#define PWM_NBCYCLES_OFST               (0)

#endif /* __PWM_REGS_H__ */
