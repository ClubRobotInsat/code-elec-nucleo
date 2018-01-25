#include "bus.h"
#include "mbed.h"
#include "HerkulexConst.h"

// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0x03;
Serial pc(USBTX, USBRX, 9600);

Serial connection(D8, D2, 115200);

int main() {
	pc.printf("xxxXXX-DEBUT-XXXxxx");

	HerkulexBus bus(&connection);
	HerkulexServo sv3 = bus.makeNewServo(0x03);
	HerkulexServo svfd = bus.makeNewServo(0xFD);
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
