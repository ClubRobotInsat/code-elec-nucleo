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
	/*
	PwmOut test_pwm(PA_9);
	DigitalOut test_direction(PC_7);
	test_pwm.period(0.0001);
	test_pwm.write(0.2f);
	test_direction.write(0);
	int i = 0;
	*/
	while(1) {
		robot.manage_robot();
		/*
		    test_direction.write(i % 2);
		    test_pwm.write(((float)i)/4.0);
		    i = (i + 1) % 4;
		    printf("%d\n\r",i);
		    wait_ms(750);
		*/
	}
}
