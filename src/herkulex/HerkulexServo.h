#ifndef HERKULEX_SERVO_H
#define HERKULEX_SERVO_H

#include "mbed.h"

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

		/* Return the cached status */
		uint8_t getStatus() const;

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
		 * These methods are not meant to be used outside of this library.
		 */

		//  Set the status to `newStatus` 
		// void setStatus(uint8_t newStatus);

		/* Set the position to `newPosition` */
		void setPosition(uint16_t newPosition);

		/* Communication --- */

		/*
		 * These methods communicates with the servo.
		 */

		// /* Move the servo to the desired position. */
		// void positionControl(uint16_t position, uint8_t playtime, uint8_t setLED) const;

		// /* Send a clear command to the servo. */
		// void clear() const;

		void enableTorque(bool value);
		bool Servo::isTorqueOn() const;

		void setDefaultLedColor(constants::LedColor led_color); 
		constants::LedColor getDefaultLedColor() const;

		void setMovingLedColor(constants::LedColor led_color); 
		constants::LedColor getMovingLedColor() const;  

		// /* Control the velocity of the servomotor directly. */
		// void velocityControl(int16_t speed, uint8_t setLED) const;

		/* Enable the LED. */
		void setLedOn() const;

		// /* Send a message to the herkulex servo and update the cached value. This call can be blocking if a previous
		//  * update-style did not receive an answer.		*/
		// void updatePosition();

		//  Send a message to the herkulex servo and update the cached value. This call can be blocking if a previous
		//  * update-style did not receive an answer.		
		// void updateStatus();

	private:
		uint8_t _id;
		uint8_t _status;

		uint8_t _status_error; 
		uint8_t _status_detail; 

		bool _torque_on; 

		constants::LedColor _led_default_color;
		constants::LedColor _led_moving_color;  

		uint16_t _position;
		uint16_t _desired_position; 

		Serial* _log;

		// The internal bus that communicates in serial with the servo.
		Bus& _bus;
	};
}

#endif
