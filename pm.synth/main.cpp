/* ============================================================================
 * main.cpp
 *
 *  Created on: 2014. 1. 26.
 *      Author: Meiday
 ============================================================================*/

#include <Arduino.h>
#include <Usb.h>
#include "usbh_midi.h"
#include "board_setup.h"

USB Usb;
MIDI Midi(&Usb);

boolean bFirst;
uint16_t pid, vid;
void midiPoll();
void doDelay( unsigned long t1, unsigned long t2, unsigned long delayTime );


//=============================================================================
void midiPoll() {
	char buf[20];
	uint8_t bufMidi[64];
	uint16_t rcvd;

	if( Midi.vid != vid || Midi.pid != pid ) {
		sprintf( buf, "VID: %04X, PID: %04X", Midi.vid, Midi.pid );
		Serial.println( buf );
		vid = Midi.vid;
		pid = Midi.pid;
	}
	if( Midi.RcvData( &rcvd, bufMidi ) == 0 ) {
		sprintf( buf, "%08ld:", millis() );
		Serial.print( buf );
		for( int i = 0; i < 64; i++ ) {
			sprintf( buf, "%02X", bufMidi[i] );
			Serial.print( buf );
		}
		Serial.println( "" );
	}
 }

void doDelay( unsigned long t1, unsigned long t2, unsigned long delayTime ) {
	unsigned long t3;

	if( t1 > t2 ) {	// ? why don't use min max instead?
		t3 = ( 4294967295 - t1 + t2 );
	} else {
		t3 = t2 - t1;
	}

	if( t3 < delayTime ) delayMicroseconds( delayTime - t3 );
}



// Arduino IDE functions
//=============================================================================
void setup() {

	board_setup();
	bFirst = true;
	vid = pid = 0;
	Serial.begin( 115200 );
	Serial.println( "usb initialization starts" );

	// Workaround for non UHS2 shield
	pinMode( 7, OUTPUT );
	digitalWrite( 7, HIGH );

	if( Usb.Init() == -1 ) {
		while(1);	//halt
	}
	delay(200);
}

void loop() {

	Usb.Task();
	if( Usb.getUsbTaskState() == USB_STATE_RUNNING ) midiPoll();

}


// Skeleton of main
//=============================================================================
int main() {

	init();
	setup();

	while(1) {
		loop();
	}

	return 1;		// should never come here
}
