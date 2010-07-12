/*
 * test.h
 *
 *  Created on: 2010-06-07
 *      Author: mzakharo
 */

#ifndef I_UART_H_
#define I_UART_H_

#ifdef _GDB_

#define SERIAL_OFFSET SERIAL0_BASE
#define SERIAL_IMR_MASK	SERIAL0_IMR_MASK
#define SERIAL_ICR    SERIAL0_ICR
#else
#define SERIAL_OFFSET SERIAL1_BASE
#define SERIAL_IMR_MASK	 SERIAL1_IMR_MASK
#define SERIAL_ICR    SERIAL1_ICR
#endif

#define SERIAL_UCR     *(( SERIAL_OFFSET) + 0x8 )
#define SERIAL_UBG1    *(( SERIAL_OFFSET) + 0x18 )
#define SERIAL_UBG2    *(( SERIAL_OFFSET) + 0x1C )
#define SERIAL_UCSR    *(( SERIAL_OFFSET) + 0x04 )
#define SERIAL_UMR     *(( SERIAL_OFFSET) + 0x00 )
#define SERIAL_IVR     *(( SERIAL_OFFSET) + 0x30 )
#define SERIAL_ISR     *(( SERIAL_OFFSET) + 0x14 )
#define SERIAL_IMR     *(( SERIAL_OFFSET) + 0x14 )
#define SERIAL_RD      *(( SERIAL_OFFSET)	+ 0x0C )
#define SERIAL_WD      *(( SERIAL_OFFSET) + 0x0C )

#endif /* I_UART_H_ */
