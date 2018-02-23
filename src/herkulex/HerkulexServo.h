#ifndef HERKULEX_SERVO_H
#define HERKULEX_SERVO_H

#include "mbed.h"
#include "HerkulexConst.h"

namespace herkulex {
	class Bus;

	template <uint8_t N_SERVOS>
	class Manager;

	class Servo {
	private:
		template <uint8_t N_SERVOS>
		friend class Manager; 

		void mgrUpdateId(uint8_t new_id);
		void mgrUpdatePosition(uint8_t new_position); 
		void mgrUpdateStatus(uint8_t new_status_error, uint8_t new_status_detail); 

		virtual ~Servo() {
			_log->printf("Destruction servo");
		};

		/* You should not call this constructor directly, but call Bus::makeNewServo */
		explicit Servo(uint8_t id, Bus& bus, Serial* log);

	public:
		/* Accessors --- */
		uint8_t getStatusError() const; 
		uint8_t getStatusDetail() const;

		/* Return the id of the servo */
		uint8_t getId() const;

		/* Return the cached position */
		/*
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!> */
		uint16_t getPosition() const;
	/*	<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		*/

		/* Settors ---
		 */

		/* Set the position to `newPosition` */
		void setPosition(uint16_t newPosition);

		void enableTorque(bool value);
		bool isTorqueOn() const;

		void setDefaultLedColor(constants::LedColor led_color); 
		constants::LedColor getDefaultLedColor() const;

		void setMovingLedColor(constants::LedColor led_color); 
		constants::LedColor getMovingLedColor() const;  

	private:
		uint8_t _id;
		uint8_t _status;

		uint8_t _status_error; 
		uint8_t _status_detail; 

		bool _torque_on; 
		bool _desired_torque_on; 

		constants::LedColor _default_led_color;
		constants::LedColor _moving_led_color;  

		uint16_t _position;
		uint16_t _desired_position; 

		Serial* _log;

		// The internal bus that communicates in serial with the servo.
		Bus& _bus;
	};
}

#endif
