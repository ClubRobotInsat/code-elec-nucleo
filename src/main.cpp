#include "HerkulexBus.h"
#include "HerkulexConst.h"
#include "HerkulexManager.h"
#include "mbed.h"

#include <trame.h>

using namespace herkulex;
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0xFD;

Serial pc(USBTX, USBRX, 9600);

Trame lire_trame(Serial pc) {
	
	if (pc.getc()==0xAC) {
		if (pc.getc()==0xDC) {
			if (pc.getc()==0xAC) {
				if (pc.getc()==0xAC) {
							
				}
			}
		}
	}
}

int main() {
	pc.printf("---- ! Initialisation de la carte ! ----\n\r");
	Manager<2> mgr(A0, A1, 1, &pc);

	Servo* sv = mgr.registerNewServo(id);
	Servo* sv3 = mgr.registerNewServo(0x03);
	sv3->reboot();
	sv->reboot();
	sv->enableTorque(true);
	sv3->enableTorque(true);
	sv->setInpositionLedColor(herkulex::constants::LedColor::Green);
	sv3->setInpositionLedColor(herkulex::constants::LedColor::Green);
	sv->setMovingLedColor(herkulex::constants::LedColor::Blue);
	sv3->setMovingLedColor(herkulex::constants::LedColor::Blue);
	while(true) {
		sv->setPosition(200);
		sv3->setPosition(200);
		wait(3);
		sv->setPosition(1000);
		sv3->setPosition(1000);
		wait(3);
		// printf("Position : %d | Status : %#x \n\r",sv->getPosition(),sv->getStatusError());
		// mgr.sendDebugMessage();
	}
}
