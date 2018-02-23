#ifndef HERKULEX_SERVO_H
#define HERKULEX_SERVO_H

#include "mbed.h"
#include "HerkulexConst.h"
#include "HerkulexBus.h"

namespace herkulex {
	class Bus;

	template <uint8_t N_SERVOS>
	class Manager;

	class Servo {
	private:
		template <uint8_t N_SERVOS>
		friend class Manager; 

		inline void mgrUpdateId(uint8_t new_id);

		inline void mgrUpdatePosition(uint8_t new_position); 

		inline void mgrUpdateStatus(uint8_t new_status_error, uint8_t new_status_detail); 

		/* You should not call this constructor directly, but call Bus::makeNewServo */
		explicit inline Servo(uint8_t id, Bus& bus, Serial* log);

		virtual ~Servo() {
			_log->printf("Destruction servo");
		};

	public:
		/* Accessors --- */
		inline uint8_t getStatusError() const; 
		inline uint8_t getStatusDetail() const;

		/* Return the id of the servo */
		inline uint8_t getId() const;

		/* Return the cached position */
		/*
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!> */
		inline uint16_t getPosition() const;
	/*	<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		*/

		/* Settors ---
		 */

		/* Set the position to `newPosition` */
		inline void setPosition(uint16_t newPosition);

		inline void enableTorque(bool value);
		inline bool isTorqueOn() const;

		inline void setDefaultLedColor(constants::LedColor::LedColorEnum led_color); 
		inline constants::LedColor::LedColorEnum getDefaultLedColor() const;

		inline void setMovingLedColor(constants::LedColor::LedColorEnum led_color); 
		inline constants::LedColor::LedColorEnum getMovingLedColor() const;  

	private:
		uint8_t _id;
		uint8_t _status;

		uint8_t _status_error; 
		uint8_t _status_detail; 

		bool _torque_on; 
		bool _desired_torque_on; 

		constants::LedColor::LedColorEnum _default_led_color;
		constants::LedColor::LedColorEnum _moving_led_color;  

		uint16_t _position;
		uint16_t _desired_position; 

		Serial* _log;

		// The internal bus that communicates in serial with the servo.
		Bus& _bus;
	};
}; 

#include "HerkulexServo.inl"

#endif
