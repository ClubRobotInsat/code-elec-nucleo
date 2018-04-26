#include "Robot.h"
#include <mbed.h>


#ifdef SIMULATED_MAIN
int main_elec() {
#else
int main() {
#endif
	Robot robot;
	robot.initialize_meca();
	wait_ms(500);
	while(1) {
		robot.manage_robot();
	}
}
