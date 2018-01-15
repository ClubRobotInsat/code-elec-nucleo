#include "bus.h"
#include "mbed.h"

// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0x03;
Serial pc(USBTX, USBRX, 9600);

Serial connection(D8, D2, 115200);

int main() {
	pc.printf("xxxXXX-DEBUT-XXXxxx");

	HerkulexBus bus(&connection);
	HerkulexServo sv3 = bus.makeNewServo(0x03);
	sv3.clear();
	sv3.setLedOn();
	sv3.setTorque(TORQUE_ON);
	sv3.positionControl(800, 60, GLED_ON);
	sv3.updateStatus();
	sv3.updatePosition();

	while(true) {
		wait_ms(7);
	}
}
