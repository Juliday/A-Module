/*
 * board_setup.cpp
 *
 *  Created on: 2014. 2. 2.
 *      Author: Meiday
 */

#include "board_setup.h"


extern uint16_t playNote();

//=============================================================================
void board_setup() {

	// Timer 2 setup; audio rate timer, fires at 15625Hz sampling rate.
	TIMSK2 = 1 << OCIE2A;	// interrupt enable audio timer.
	OCR2A = 2048;
	TCCR2A = 2;					// CTC mode, counts up to 127 until resets.
	TCCR2B = 0 << CS22 | 1 << CS21 | 0 << CS20;	// different for atmega8
	SPCR = 0x50;					// setup SPI port SPCR = 01010000
	SPSR = 0x01;
	DDRB |= 0x2E;				// PB output for DAC CS, and SPI port
	PORTB |= ( 1 << 1 );		// CS high

	sei();									// global interrupt enable.
}


//=============================================================================
// Timer 2 interrupt routine calling the synth enging for every sample(?)
// two bytes that go to the DAC over SPI
uint8_t dacSPI0;
uint8_t dacSPI1;

ISR( TIMER2_COMPA_vect ) {

	OCR2A = 127;									//PORTB &= ~( 1 << 1 ) //(transmit?) Frame sync low for SPI
	uint16_t data = playNote();

	// 1. format data for SPI port
	if( data >= 1 ) {
		dacSPI0 = data >> 8;
		dacSPI0 >>= 4;
		dacSPI0 |= 0x10;							// unbuffered, gain x 2		//dacSPI0 |= 0x50;	// buffered, gain x 2
	} else {
		dacSPI0 = data >> 8;
		dacSPI0 >>= 4;
		dacSPI0 |= 0;								// shut DAC off
	}
	dacSPI1 = data >> 4;

	// 2. transmit value out the SPI port
	PORTB  &= ~( 1 << 1 );					// frame sync low
	SPDR = dacSPI0;								// SPI data register - writing to the register initiates data transmission.
	while( !( SPSR & ( 1 << SPIF ) ) );	// wait for data to be sent over SPI, flag raised.
	SPDR = dacSPI1;
	while( !( SPSR & ( 1 << SPIF ) ) );
	PORTB |= ( 1 << 1 );						// frame sync high
}
