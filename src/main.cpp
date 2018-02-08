#include "HerkulexBus.h"
#include "HerkulexConst.h"
#include "HerkulexManager.h"
#include "mbed.h"

using namespace herkulex;
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0x03;
Serial pc(USBTX, USBRX, 9600);

Serial connection(D8, D2, 115200);

int main() {
	pc.printf("xxxXXX-DEBUT-XXXxxx");

	Bus bus(&connection);
	Servo sv3 = bus.makeNewServo(0x03);
	Servo svfd = bus.makeNewServo(0xFD);
	sv3.clear();
	svfd.clear();

	sv3.setLedOn();
	svfd.setLedOn();

	sv3.setTorque(TORQUE_ON);
	svfd.setTorque(TORQUE_ON);

	sv3.positionControl(800, 60, GLED_ON);
	svfd.positionControl(800, 60, GLED_ON);
	wait_ms(800);
	svfd.positionControl(300, 60, BLED_ON);
	sv3.positionControl(300, 60, BLED_ON);

	sv3.updateStatus();
	svfd.updateStatus();

	sv3.updatePosition();
	svfd.updatePosition();
	wait_ms(750);

	while(true) {
		sv3.updateStatus();
		svfd.updateStatus();
		svfd.updatePosition();
		sv3.updatePosition();
		wait_ms(7);
	}
}
