#include "HerkulexBus.h"
#include "HerkulexConst.h"
#include "HerkulexManager.h"
#include "mbed.h"

using namespace herkulex;
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0x03;

Serial pc(USBTX, USBRX, 9600);

int main() {
	pc.printf("xxxXXX-DEBUT-XXXxxx \n");
	Manager<16> mgr(D8, D2, 1500, &pc);
	pc.printf("1 \n");
	Servo* sv3 = mgr.registerNewServo(0xFD);
	pc.printf("2 \n");
	if(sv3 == nullptr) {
		pc.printf("AIE\n");
	}
	sv3->clear();
	pc.printf("3 \n");
	sv3->setLedOn();
	pc.printf("4 \n");
	// sv3->setTorque(TORQUE_ON);
	pc.printf("5 \n");

	/*Servo svfd = bus.makeNewServo(0xFD);
	svfd.clear();
	svfd.setLedOn();
	svfd.setTorque(TORQUE_ON);
*/

	// sv3->positionControl(800, 60, GLED_ON);
	/*
	svfd.positionControl(800, 60, GLED_ON);
	wait_ms(800);
	svfd.positionControl(300, 60, BLED_ON);
	sv3.positionControl(300, 60, BLED_ON);

	sv3.updateStatus();
	svfd.updateStatus();

	sv3.updatePosition();
	svfd.updatePosition();
	wait_ms(750);
*/
	while(true) {
		/*		sv3.updateStatus();
		        svfd.updateStatus();
		        svfd.updatePosition();
		        sv3.updatePosition();
		        */
		wait_ms(7);
	}
}
