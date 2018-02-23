#ifndef HERKULEX_SERVO_H
#define HERKULEX_SERVO_H

#include "mbed.h"
#include "HerkulexConst.h"

namespace herkulex {
	template <uint8_t N_SERVOS>
	class Manager;

	class Servo {
	private:
		template <uint8_t N_SERVOS>
		friend class Manager; 

		/* You should never call this constructor directly, but call Manager::makeNewServo */
		explicit inline Servo(uint8_t id, Serial* log);

		virtual inline ~Servo();
		
		/* MANAGER FUNCTIONS - DO NOT USE OUTSIDE OF MANAGER */ 

		/* --------------------------------------------------------------------------------------------
		 * mgrUpdateId
		 * Function for the Manager : updates servo's id 
		 * --------------------------------------------------------------------------------------------
		 */
		inline void mgrUpdateId(uint8_t new_id);

		/* --------------------------------------------------------------------------------------------
		 * mgrUpdatePosition
		 * Function for the Manager : updates servo's position
		 * --------------------------------------------------------------------------------------------
		 */
		inline void mgrUpdatePosition(uint8_t new_position); 

		/* --------------------------------------------------------------------------------------------
		 * mgrUpdateStatus
		 * Function for the Manager : updates servo's status, and torque status (on/off)
		 * --------------------------------------------------------------------------------------------
		 */
		inline void mgrUpdateStatus(uint8_t new_status_error, uint8_t new_status_detail); 

	public:
		/* GETTERS */ 

		/* --------------------------------------------------------------------------------------------
		 * getStatusError
		 * Return servo's StatusError. For further informations, refer to herkulex documentation p.39.
		 * Update rate defined from the refresh period of the manager. 
		 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager). 
		 * --------------------------------------------------------------------------------------------
		 */
		inline uint8_t getStatusError() const; 

		/* --------------------------------------------------------------------------------------------
		 * getStatusDetail
		 * Return servo's StatusDetail. For further informations, refer to herkulex documentation p.39.
		 * Update rate defined from the refresh period of the manager. 
		 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager). 
		 * --------------------------------------------------------------------------------------------
		 */
		inline uint8_t getStatusDetail() const;

		/* --------------------------------------------------------------------------------------------
		 * getId
		 * Return servo's ID.
		 * --------------------------------------------------------------------------------------------
		 */
		inline uint8_t getId() const;

		/* --------------------------------------------------------------------------------------------
		 * getPosition
		 * Return servo's current position. 
		 * Update rate defined from the refresh period of the manager. 
		 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager). 
		 * --------------------------------------------------------------------------------------------
		 */
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

		/* --------------------------------------------------------------------------------------------
		 * isTorqueOn
		 * Return servo's motor on (= torque on ) flag. (cf. status detail doc.). 
		 * Update rate defined from the refresh period of the manager. 
		 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager). 
		 * --------------------------------------------------------------------------------------------
		 */
		inline bool isTorqueOn() const;

		/* --------------------------------------------------------------------------------------------
		 * isInPosition
		 * Return servo's inposition flag. (cf. status detail doc.). 
		 * Update rate defined from the refresh period of the manager. 
		 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager). 
		 * --------------------------------------------------------------------------------------------
		 */
		inline bool isInPosition() const; 

		/* --------------------------------------------------------------------------------------------
		 * isMoving
		 * Return servo's moving flag. (cf. status detail doc.). 
		 * Update rate defined from the refresh period of the manager. 
		 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager). 
		 * --------------------------------------------------------------------------------------------
		 */
		inline bool isMoving() const; 

		/* --------------------------------------------------------------------------------------------
		 * getInpositionLedColor
		 * Get the color of the led for a servo in position.
		 * --------------------------------------------------------------------------------------------
		 */
		inline constants::LedColor::LedColorEnum getInpositionLedColor() const;

		/* --------------------------------------------------------------------------------------------
		 * getMovingLedColor
		 * Get the color of the led for a servo moving.
		 * --------------------------------------------------------------------------------------------
		 */
		inline constants::LedColor::LedColorEnum getMovingLedColor() const;  

		/* SETTERS */ 

		/* --------------------------------------------------------------------------------------------
		 * setPosition
		 * Set a new position for the servo. 
		 * Sent at a rate defined from the refresh period of the manager. 
		 * sending rate = refresh period / number of servos (template argument N_SERVOS of the manager). 
		 * --------------------------------------------------------------------------------------------
		 */
		inline void setPosition(uint16_t newPosition);

		/* --------------------------------------------------------------------------------------------
		 * enableTorque
		 * Set a new torque mode for the servo (on/off). 
		 * Sent at a rate defined from the refresh period of the manager. 
		 * sending rate = refresh period / number of servos (template argument N_SERVOS of the manager). 
		 * --------------------------------------------------------------------------------------------
		 */
		inline void enableTorque(bool value);

		/* --------------------------------------------------------------------------------------------
		 * setInpositionLedColor
		 * Set the color of the led for a servo in position.
		 * Sent at a rate defined from the refresh period of the manager. 
		 * sending rate = refresh period / number of servos (template argument N_SERVOS of the manager). 
		 * --------------------------------------------------------------------------------------------
		 */
		inline void setInpositionLedColor(constants::LedColor::LedColorEnum led_color); 


		/* --------------------------------------------------------------------------------------------
		 * setMovingLedColor
		 * Set the color of the led for a servo moving.
		 * Sent at a rate defined from the refresh period of the manager. 
		 * sending rate = refresh period / number of servos (template argument N_SERVOS of the manager). 
		 * --------------------------------------------------------------------------------------------
		 */
		inline void setMovingLedColor(constants::LedColor::LedColorEnum led_color); 

	private:
		uint8_t _id;

		uint8_t _status_error; 
		uint8_t _status_detail; 

		bool _torque_on; 
		bool _desired_torque_on; 

		constants::LedColor::LedColorEnum _inposition_led_color;
		constants::LedColor::LedColorEnum _moving_led_color;  

		uint16_t _position;
		uint16_t _desired_position; 

		Serial* _log;
	};
}; 

#include "HerkulexServo.inl"

#endif
