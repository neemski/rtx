/*****************************************************************
 *
 *                           E&CE 354 RTX
 *                   (c)1998, All Rights Reserved
 *
 *                  Chris McKillop and  Craig Stout
 *                  [cdmckill,castout]@uwaterloo.ca
 *
 *****************************************************************/

/*  $Id: rtx_inc.h,v 1.1 2010/05/03 21:32:25 yqhuang Exp $ */

/******************************************************************
 *
 *  $Source: /usr/local/CVS_Repository/ece354/manual/m5307v2/shared/rtx_inc.h,v $
 *  $Author: yqhuang $
 *  $Date: 2010/05/03 21:32:25 $
 *
 *  Purpose:   Base include file for the RTX
 *
 *  $Log: rtx_inc.h,v $
 *  Revision 1.1  2010/05/03 21:32:25  yqhuang
 *  *** empty log message ***
 *
 *
 *  [LOG REMOVED]
 *
 ******************************************************************/

#if !defined( RTX_BASE_H__ )
#define RTX_BASE_H__

#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)



/******************************************************************
 *                      CONSTANTS
 ******************************************************************/

/*
 * Data Types by Size
 */
#define SINT32  signed long int
#define UINT32  unsigned long int
#define SINT16  signed short int
#define UINT16  unsigned short int
#define SINT8   signed char
#define UINT8   unsigned char
#define CHAR    signed char
#define BYTE    unsigned char
#define VOID    void
#define BOOLEAN signed long int

#define ESC                 0x1B
#define BKSP                '\b'

#define CR                  '\r' //0XD
#define LF                  '\n' //)xA
#if !defined( TRUE )
#define TRUE 1
#endif

#if !defined( FALSE )
#define FALSE 0
#endif

#if !defined( NULL )
#define NULL 0
#endif

#ifdef _GDB_
#define DISABLE_INTERRUPTS() asm("move.w  #0x2600, %sr")
#else
#define DISABLE_INTERRUPTS() asm("move.w  #0x2700, %sr")
#endif
/*
 * Coldfire system defines
 */
#define RTX_COLDFIRE_MBAR    (BYTE *)(0xF0000000)
#define SIM_IMR              *( (UINT32 *)( RTX_COLDFIRE_MBAR + 0x44 ) )

/*
 * Coldfire Timer Defines
 */
#define TIMER0_TMR  *( (UINT16 *)( RTX_COLDFIRE_MBAR + 0x140 ) )
#define TIMER0_TRR  *( (UINT16 *)( RTX_COLDFIRE_MBAR + 0x144 ) )
#define TIMER0_TCN  *( (UINT16 *)( RTX_COLDFIRE_MBAR + 0x14C ) )
#define TIMER0_ICR  *( RTX_COLDFIRE_MBAR + 0x04d )
#define TIMER0_TER  *( RTX_COLDFIRE_MBAR + 0x151 )

#define TIMER1_TMR  *( (UINT16 *)(RTX_COLDFIRE_MBAR + 0x180 ) )
#define TIMER1_TRR  *( (UINT16 *)(RTX_COLDFIRE_MBAR + 0x184 ) )
#define TIMER1_TCN  *( (UINT16 *)(RTX_COLDFIRE_MBAR + 0x18C ) )
#define TIMER1_ICR  *( RTX_COLDFIRE_MBAR + 0x04e )
#define TIMER1_TER  *( RTX_COLDFIRE_MBAR + 0x191 )

/*
 * Coldfire Serial Defines
 */
#define SERIAL1_IMR_MASK	0x1 << 13
#define SERIAL1_BASE  	RTX_COLDFIRE_MBAR + 0x200
#define SERIAL1_ICR     *( RTX_COLDFIRE_MBAR + 0x51 )
#define SERIAL1_UCR     *( SERIAL1_BASE + 0x8 )
#define SERIAL1_UBG1    *( SERIAL1_BASE + 0x18 )
#define SERIAL1_UBG2    *( SERIAL1_BASE + 0x1C )
#define SERIAL1_UCSR    *( SERIAL1_BASE + 0x04 )
#define SERIAL1_UMR     *( SERIAL1_BASE + 0x00 )
#define SERIAL1_IVR     *( SERIAL1_BASE + 0x30 )
#define SERIAL1_ISR     *( SERIAL1_BASE + 0x14 )
#define SERIAL1_IMR     *( SERIAL1_BASE + 0x14 )
#define SERIAL1_RD      *( SERIAL1_BASE + 0x0C )
#define SERIAL1_WD      *( SERIAL1_BASE + 0x0C )

#define SERIAL0_IMR_MASK	0x1 << 12
#define SERIAL0_BASE  	RTX_COLDFIRE_MBAR + 0x1C0
#define SERIAL0_ICR     *( RTX_COLDFIRE_MBAR + 0x50 )
#define SERIAL0_UCR     *( SERIAL0_BASE + 0x8 )
#define SERIAL0_UBG1    *( SERIAL0_BASE + 0x18 )
#define SERIAL0_UBG2    *( SERIAL0_BASE + 0x1C )
#define SERIAL0_UCSR    *( SERIAL0_BASE + 0x04 )
#define SERIAL0_UMR     *( SERIAL0_BASE + 0x00 )
#define SERIAL0_IVR     *( SERIAL0_BASE + 0x30 )
#define SERIAL0_ISR     *( SERIAL0_BASE + 0x14 )
#define SERIAL0_IMR     *( SERIAL0_BASE + 0x14 )
#define SERIAL0_RD      *( SERIAL0_BASE + 0x0C )
#define SERIAL0_WD      *( SERIAL0_BASE + 0x0C )
/*
 * RTX Error Codes
 */
#define RTX_SUCCESS 0
#define RTX_ERROR   -1
/*
 *	Define the ColdFire UART register set addresses.
 */
#define	MCFUART_UMR		0x00		/* Mode register (r/w) */
#define	MCFUART_USR		0x04		/* Status register (r) */
#define	MCFUART_UCSR		0x04		/* Clock Select (w) */
#define	MCFUART_UCR		0x08		/* Command register (w) */
#define	MCFUART_URB		0x0c		/* Receiver Buffer (r) */
#define	MCFUART_UTB		0x0c		/* Transmit Buffer (w) */
#define	MCFUART_UIPCR		0x10		/* Input Port Change (r) */
#define	MCFUART_UACR		0x10		/* Auxiliary Control (w) */
#define	MCFUART_UISR		0x14		/* Interrupt Status (r) */
#define	MCFUART_UIMR		0x14		/* Interrupt Mask (w) */
#define	MCFUART_UBG1		0x18		/* Baud Rate MSB (r/w) */
#define	MCFUART_UBG2		0x1c		/* Baud Rate LSB (r/w) */
#ifdef	CONFIG_M5272
#define	MCFUART_UTF		0x28		/* Transmitter FIFO (r/w) */
#define	MCFUART_URF		0x2c		/* Receiver FIFO (r/w) */
#define	MCFUART_UFPD		0x30		/* Frac Prec. Divider (r/w) */
#else
#define	MCFUART_UIVR		0x30		/* Interrupt Vector (r/w) */
#endif
#define	MCFUART_UIPR		0x34		/* Input Port (r) */
#define	MCFUART_UOP1		0x38		/* Output Port Bit Set (w) */
#define	MCFUART_UOP0		0x3c		/* Output Port Bit Reset (w) */

/*
 *	Define bit flags in Mode Register 1 (MR1).
 */
#define	MCFUART_MR1_RXRTS	0x80		/* Auto RTS flow control */
#define	MCFUART_MR1_RXIRQFULL	0x40		/* RX IRQ type FULL */
#define	MCFUART_MR1_RXIRQRDY	0x00		/* RX IRQ type RDY */
#define	MCFUART_MR1_RXERRBLOCK	0x20		/* RX block error mode */
#define	MCFUART_MR1_RXERRCHAR	0x00		/* RX char error mode */

#define	MCFUART_MR1_PARITYNONE	0x10		/* No parity */
#define	MCFUART_MR1_PARITYEVEN	0x00		/* Even parity */
#define	MCFUART_MR1_PARITYODD	0x04		/* Odd parity */
#define	MCFUART_MR1_PARITYSPACE	0x08		/* Space parity */
#define	MCFUART_MR1_PARITYMARK	0x0c		/* Mark parity */

#define	MCFUART_MR1_CS5		0x00		/* 5 bits per char */
#define	MCFUART_MR1_CS6		0x01		/* 6 bits per char */
#define	MCFUART_MR1_CS7		0x02		/* 7 bits per char */
#define	MCFUART_MR1_CS8		0x03		/* 8 bits per char */

/*
 *	Define bit flags in Mode Register 2 (MR2).
 */
#define	MCFUART_MR2_LOOPBACK	0x80		/* Loopback mode */
#define	MCFUART_MR2_REMOTELOOP	0xc0		/* Remote loopback mode */
#define	MCFUART_MR2_AUTOECHO	0x40		/* Automatic echo */
#define	MCFUART_MR2_TXRTS	0x20		/* Assert RTS on TX */
#define	MCFUART_MR2_TXCTS	0x10		/* Auto CTS flow control */

#define	MCFUART_MR2_STOP1	0x07		/* 1 stop bit */
#define	MCFUART_MR2_STOP15	0x08		/* 1.5 stop bits */
#define	MCFUART_MR2_STOP2	0x0f		/* 2 stop bits */

/*
 *	Define bit flags in Status Register (USR).
 */
#define	MCFUART_USR_RXBREAK	0x80		/* Received BREAK */
#define	MCFUART_USR_RXFRAMING	0x40		/* Received framing error */
#define	MCFUART_USR_RXPARITY	0x20		/* Received parity error */
#define	MCFUART_USR_RXOVERRUN	0x10		/* Received overrun error */
#define	MCFUART_USR_TXEMPTY	0x08		/* Transmitter empty */
#define	MCFUART_USR_TXREADY	0x04		/* Transmitter ready */
#define	MCFUART_USR_RXFULL	0x02		/* Receiver full */
#define	MCFUART_USR_RXREADY	0x01		/* Receiver ready */

#define	MCFUART_USR_RXERR	(MCFUART_USR_RXBREAK | MCFUART_USR_RXFRAMING | \
				MCFUART_USR_RXPARITY | MCFUART_USR_RXOVERRUN)

/*
 *	Define bit flags in Clock Select Register (UCSR).
 */
#define	MCFUART_UCSR_RXCLKTIMER	0xd0		/* RX clock is timer */
#define	MCFUART_UCSR_RXCLKEXT16	0xe0		/* RX clock is external x16 */
#define	MCFUART_UCSR_RXCLKEXT1	0xf0		/* RX clock is external x1 */

#define	MCFUART_UCSR_TXCLKTIMER	0x0d		/* TX clock is timer */
#define	MCFUART_UCSR_TXCLKEXT16	0x0e		/* TX clock is external x16 */
#define	MCFUART_UCSR_TXCLKEXT1	0x0f		/* TX clock is external x1 */

/*
 *	Define bit flags in Command Register (UCR).
 */
#define	MCFUART_UCR_CMDNULL		0x00	/* No command */
#define	MCFUART_UCR_CMDRESETMRPTR	0x10	/* Reset MR pointer */
#define	MCFUART_UCR_CMDRESETRX		0x20	/* Reset receiver */
#define	MCFUART_UCR_CMDRESETTX		0x30	/* Reset transmitter */
#define	MCFUART_UCR_CMDRESETERR		0x40	/* Reset error status */
#define	MCFUART_UCR_CMDRESETBREAK	0x50	/* Reset BREAK change */
#define	MCFUART_UCR_CMDBREAKSTART	0x60	/* Start BREAK */
#define	MCFUART_UCR_CMDBREAKSTOP	0x70	/* Stop BREAK */

#define	MCFUART_UCR_TXNULL	0x00		/* No TX command */
#define	MCFUART_UCR_TXENABLE	0x04		/* Enable TX */
#define	MCFUART_UCR_TXDISABLE	0x08		/* Disable TX */
#define	MCFUART_UCR_RXNULL	0x00		/* No RX command */
#define	MCFUART_UCR_RXENABLE	0x01		/* Enable RX */
#define	MCFUART_UCR_RXDISABLE	0x02		/* Disable RX */

/*
 *	Define bit flags in Input Port Change Register (UIPCR).
 */
#define	MCFUART_UIPCR_CTSCOS	0x10		/* CTS change of state */
#define	MCFUART_UIPCR_CTS	0x01		/* CTS value */

/*
 *	Define bit flags in Input Port Register (UIP).
 */
#define	MCFUART_UIPR_CTS	0x01		/* CTS value */

/*
 *	Define bit flags in Output Port Registers (UOP).
 *	Clear bit by writing to UOP0, set by writing to UOP1.
 */
#define	MCFUART_UOP_RTS		0x01		/* RTS set or clear */

/*
 *	Define bit flags in the Auxiliary Control Register (UACR).
 */
#define	MCFUART_UACR_IEC	0x01		/* Input enable control */

/*
 *	Define bit flags in Interrupt Status Register (UISR).
 *	These same bits are used for the Interrupt Mask Register (UIMR).
 */
#define	MCFUART_UIR_COS		0x80		/* Change of state (CTS) */
#define	MCFUART_UIR_DELTABREAK	0x04		/* Break start or stop */
#define	MCFUART_UIR_RXREADY	0x02		/* Receiver ready */
#define	MCFUART_UIR_TXREADY	0x01		/* Transmitter ready */

#endif
