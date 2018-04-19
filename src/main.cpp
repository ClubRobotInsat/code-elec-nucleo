#include "mbed.h"
#include "QEI.h"

#define PPR 4096
#define MIN_PWM 0.08
#define MARGE 150

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
			, _error_sum(0.0)
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

		void turn_N(int nb_turns)
		{
			float reduction = 11.6;
			float pulses = reduction * (float)PPR*(float)nb_turns;
			_pulse_wanted += static_cast<int>(pulses);
		}

		static void afficher_coeff() {
			printf("Kp : %f | Ki : %f | Kd : %f\n\r",Kp*10000,Ki*10000,Kd*10000);
		}

		void asserv() {

			int pulses = _encoder.getPulses();

			if(_pulse_wanted >= pulses+MARGE || _pulse_wanted <= pulses-MARGE) {

				float error = abs(pulses - _pulse_wanted);
				float delta_error = fabs(error - _prev_error);
				_error_sum += error;
				_prev_error = error;


				// /!\ PWM >= MIN_PWM sinon le moteur ne tourne pas
				float pwm_value = static_cast<float>(error * Kp) + static_cast<float>(_error_sum*Ki) + static_cast<float>(delta_error*Kd) ;

				if(pwm_value < MIN_PWM)
				{
					pwm_value = MIN_PWM;
				}

				//debug("Je veut être en %d et je suis en %d => déplacement avec pour consigne %f\n\r",_pulse_wanted,pulses,pwm_value);

				if (_pulse_wanted < pulses) {
					_motor_speed_control.write(pwm_value);
					_motor_direction_control.write(1);
				} else {
					_motor_speed_control.write(pwm_value);
					_motor_direction_control.write(0);
				}
				
			}
			else {
				//debug("Je suis en position ! \n\r");
				_motor_speed_control.write(0);
				_error_sum = 0;
				_prev_error = 0;
			}
		}

		static float Kp;
		static float Ki;
		static float Kd;

	private :

		int32_t _pulse_wanted;
		QEI _encoder;
		PwmOut _motor_speed_control;
		DigitalOut _motor_direction_control;
		float _prev_error;
		float _error_sum;

};


// float Motor::Kp = 0.157/10000;
// float Motor::Ki = 0.006471/10000;
// float Motor::Kd = 0.005000/10000;

float Motor::Kp = 0.200/10000;
float Motor::Ki = 0.000/10000;
float Motor::Kd = 0.000/10000;

int i = 0;

Motor motor(D3,D5,D9,D10,0.04);
void update_pid();

void avancer() {
	//motor.set_position( ((float) (i % 4)) * 90.0);q7
	motor.set_position( ((float) (i % 2)) * 180.0);
	i++;
}

AnalogIn akp(A0);
AnalogIn aki(A1);
AnalogIn akd(A2);

float max_kp = 0.4/10000;
float max_ki = 0.1/10000;
float max_kd = 0.2/1000;

void update_pid() {

	float kp = akp.read() * max_kp;
	float ki = aki.read() * max_ki;
	float kd = akd.read() * max_kd;

	if(kp < 0.0009/10000)
		kp = 0.0;
	if(ki < 0.0009/10000)
		ki = 0.0;
	if(kd < 0.0009/10000)
		kd = 0.0;

	//Motor::Kp = kp;
	//Motor::Ki = ki;
	//Motor::Kd = kd;

	Motor::afficher_coeff();
}

void test_turn_N()
{
	motor.turn_N(-1);
}

int main() {

	Ticker av;
	//av.attach(&avancer,4.0);

	Ticker turn;
	turn.attach(&test_turn_N,5.0);

	while(1) {
		wait_ms(1);
		motor.asserv();
		update_pid();
	}

}
