#include "mbed.h"
#include "QEI.h"

#define PPR 4096

Serial pc(USBTX, USBRX);

class Motor
{
	public:
		Motor(PinName pin_qei_1, PinName pin_qei_2, PinName pin_motor_control,PinName pin_direction_control,float period_asserv) : 
			_pulse_wanted(0)
			,_encoder(pin_qei_1, pin_qei_2, NC, PPR, QEI::X4_ENCODING)
			, _motor_speed_control(pin_motor_control)
			, _motor_direction_control(pin_direction_control)
			, _prev_error(0.0)
			  {
				_motor_speed_control.period(0.0001);
				_motor_speed_control.write(0);
				_motor_direction_control.write(0);
			}


		/* L'angle est en degré entre 0 et 360 !!!! */
		void set_position(float angle) {
			// Reduction : 11.6 tours de codeur pour 1 tour de moteur
			float reduction = 11.6;
			float pulses = reduction * (float)PPR;
			_pulse_wanted = static_cast<int>((pulses/360.0)*angle);
		}


		void asserv() {

			int pulses = _encoder.getPulses();
			if(_pulse_wanted != pulses) {

				float error = abs(pulses - _pulse_wanted);
				float delta_error = fabs(error - _prev_error);
				_prev_error = error;

				float pwm_value = static_cast<float>(error * Kp)/10000.0 + static_cast<float>(delta_error*Kd)/10000.0;
				debug("Je veut être en %d et je suis en %d => déplacement avec pour consigne %f\n\r",_pulse_wanted,pulses,pwm_value);
				if (_pulse_wanted < pulses) {
					debug("Je recule ! \n\r");
					_motor_speed_control.write(pwm_value);
					_motor_direction_control.write(1);
				} else {
					debug("J'avance  ! \n\r");
					_motor_speed_control.write(pwm_value);
					_motor_direction_control.write(0);
				}
			}
			else {
				debug("Je suis en position ! \n\r");
				_motor_speed_control.write(0);
			}
		}

	private :
		constexpr static float Kp = 0.125;
		constexpr static float Kd = 0.08;
		int32_t _pulse_wanted;
		QEI _encoder;
		PwmOut _motor_speed_control;
		DigitalOut _motor_direction_control;
		float _prev_error;

};

int i = 0;

Motor motor(D3,D5,D9,D10,0.04);
void avancer() {
	//motor.set_position( ((float) (i % 4)) * 90.0);q7
	motor.set_position( ((float) (i % 2)) * 180.0);
	i++;
}

int main() {

	Ticker av;
	av.attach(&avancer,3.0);

	while(1) {
		wait_ms(1);
		motor.asserv();
	}

}
