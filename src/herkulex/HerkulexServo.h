#ifndef HERKULEX_SERVO_H
#define HERKULEX_SERVO_H

#include "HerkulexBus.h"
#include "HerkulexConst.h"
#include "mbed.h"

namespace herkulex {
	template <uint8_t N_SERVOS>
	class Manager;

	class Servo {
	private:
		template <uint8_t N_SERVOS>
		friend class Manager;

		/* You should never call this constructor directly, but call Manager::makeNewServo */
		explicit Servo(uint8_t id, Bus* bus);

		virtual ~Servo();

		/* MANAGER FUNCTIONS - DO NOT USE OUTSIDE OF MANAGER */

		/* --------------------------------------------------------------------------------------------
		 * mgrUpdateId
		 * Function for the Manager : updates servo's id
		 * --------------------------------------------------------------------------------------------
		 */
		void mgrUpdateId(uint8_t new_id);

		/* --------------------------------------------------------------------------------------------
		 * mgrUpdatePosition
		 * Function for the Manager : updates servo's position
		 * --------------------------------------------------------------------------------------------
		 */
		void mgrUpdatePosition(uint8_t new_position);

		/* --------------------------------------------------------------------------------------------
		 * mgrUpdateStatus
		 * Function for the Manager : updates servo's status, and torque status (on/off)
		 * --------------------------------------------------------------------------------------------
		 */
		void mgrUpdateStatus(uint8_t new_status_error, uint8_t new_status_detail);

	public:
		/* GETTERS */

		/* --------------------------------------------------------------------------------------------
		 * get_status_error
		 * Return servo's StatusError. For further informations, refer to herkulex documentation p.39.
		 * --------------------------------------------------------------------------------------------
		 */
		uint8_t get_status_error() const;

		/* --------------------------------------------------------------------------------------------
		 * get_status_detail
		 * Return servo's StatusDetail. For further informations, refer to herkulex documentation p.39.
		 * --------------------------------------------------------------------------------------------
		 */
		uint8_t get_status_detail() const;

		/* --------------------------------------------------------------------------------------------
		 * get_id
		 * Return servo's ID.
		 * --------------------------------------------------------------------------------------------
		 */
		uint8_t get_id() const;

		/* --------------------------------------------------------------------------------------------
		 * get_position
		 * Return servo's current position.
		 * --------------------------------------------------------------------------------------------
		 *<!><!><!>  Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!><!><!>
		     */
		uint16_t get_position() const;

		/* --------------------------------------------------------------------------------------------
		 * is_torque_on
		 * Return servo's motor on (= torque on ) flag. (cf. status detail doc.).
		 * --------------------------------------------------------------------------------------------
		 */
		bool is_torque_on() const;

		/* --------------------------------------------------------------------------------------------
		 * isInPosition
		 * Return servo's inposition flag. (cf. status detail doc.).
		 * --------------------------------------------------------------------------------------------
		 */
		bool is_in_position() const;

		/* --------------------------------------------------------------------------------------------
		 * is_moving
		 * Return servo's moving flag. (cf. status detail doc.).
		 * --------------------------------------------------------------------------------------------
		 */
		bool is_moving() const;

		/* --------------------------------------------------------------------------------------------
		 * get_inposition_led_color
		 * Get the color of the led for a servo in position.
		 * --------------------------------------------------------------------------------------------
		 */
		constants::LedColor::LedColorEnum get_inposition_led_color() const;

		/* --------------------------------------------------------------------------------------------
		 * get_moving_led_color
		 * Get the color of the led for a servo moving.
		 * --------------------------------------------------------------------------------------------
		 */
		constants::LedColor::LedColorEnum get_moving_led_color() const;

		/* SETTERS */

		void reboot();

		/* --------------------------------------------------------------------------------------------
		 * set_position
		 * Set a new position for the servo.
		 * --------------------------------------------------------------------------------------------
		 */
		void set_position(uint16_t newPosition);

		/* --------------------------------------------------------------------------------------------
		 * enable_torque
		 * Set a new torque mode for the servo (on/off).
		 * --------------------------------------------------------------------------------------------
		 */
		void enable_torque(bool value);

		/* --------------------------------------------------------------------------------------------
		 * set_inposition_led_color
		 * Set the color of the led for a servo in position.
		 * --------------------------------------------------------------------------------------------
		 */
		void set_inposition_led_color(constants::LedColor::LedColorEnum led_color);


		/* --------------------------------------------------------------------------------------------
		 * set_moving_ced_color
		 * Set the color of the led for a servo moving.
		 * --------------------------------------------------------------------------------------------
		 */
		void set_moving_led_color(constants::LedColor::LedColorEnum led_color);

	private:
		uint8_t _id;

		uint8_t _status_error;
		uint8_t _status_detail;

		bool _torque_on;
		bool _desired_torque_on;
		bool _should_reboot;

		constants::LedColor::LedColorEnum _inposition_led_color;
		constants::LedColor::LedColorEnum _moving_led_color;

		uint16_t _position;
		uint16_t _desired_position;

		Bus* _bus;
	};
};

#endif
