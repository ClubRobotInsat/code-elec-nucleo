#include "HerkulexBus.h"
#include "HerkulexConst.h"
#include "HerkulexManager.h"
#include "mbed.h"

using namespace herkulex;
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0xFD;

Serial pc(USBTX, USBRX, 9600);

int main() {
	Manager<16> mgr(D8, D2, 0.2, &pc);
	Servo* sv = mgr.registerNewServo(id);
	sv->enableTorque(true);
	sv->setInpositionLedColor(herkulex::constants::LedColor::Red);
	//sv.positionControl(800, 60, GLED_ON);
	//wait_ms(800);
	//sv.positionControl(300, 60, BLED_ON);

	//sv.updateStatus();

	//sv.updatePosition();
	while(true) {
		/*		sv3.updateStatus();
		        sv.updateStatus();
		        sv.updatePosition();
		        sv3.updatePosition();
		        */
		wait_ms(7);
	}
}
