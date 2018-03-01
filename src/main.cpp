#include "HerkulexBus.h"
#include "HerkulexConst.h"
#include "HerkulexManager.h"
#include "mbed.h"

using namespace herkulex;
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0xFD;

Serial pc(USBTX, USBRX, 9600);

int main() {
	pc.printf("---- ! Initialisation de la carte ! ----\n\r");
	Manager<1> mgr(A0, A1, 2.0, &pc);
	
	Servo* sv = mgr.registerNewServo(id);
	sv->reboot();
	sv->enableTorque(true);
	sv->setInpositionLedColor(herkulex::constants::LedColor::Green);
	sv->setMovingLedColor(herkulex::constants::LedColor::Blue);
	wait_ms(500);	
	while(true) {
		sv->setPosition(200);
		wait_ms(500);
		sv->setPosition(1000);
		wait_ms(500);
		//printf("Position : %d | Status : %#x \n\r",sv->getPosition(),sv->getStatusError());
		//mgr.sendDebugMessage();
	}
}
