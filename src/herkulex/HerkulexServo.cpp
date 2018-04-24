#ifndef HERKULEX_SERVO_INL
#define HERKULEX_SERVO_INL

#include "HerkulexServo.h"

namespace herkulex {

	Servo::Servo(uint8_t id, Bus* bus)
	        : _id(id)
	        , _status_error(0)
	        , _status_detail(0)
	        , _torque_on(false)
	        , _desired_torque_on(true)
	        , _should_reboot(false)
	        , _inposition_led_color(constants::LedColor::Green)
	        , _moving_led_color(constants::LedColor::Blue)
	        , _position(0)
	        , _desired_position(0)
	        , _bus(bus)

	{}

	Servo::~Servo() {}


	/* MANAGER FUNCTIONS - DO NOT USE OUTSIDE OF MANAGER */

	/* --------------------------------------------------------------------------------------------
	 * mgrUpdateId
	 * Function for the Manager : updates servo's id
	 * --------------------------------------------------------------------------------------------
	 */
	void Servo::mgrUpdateId(uint8_t new_id) {
		_id = new_id;
	}

	/* --------------------------------------------------------------------------------------------
	 * mgrUpdatePosition
	 * Function for the Manager : updates servo's position
	 * --------------------------------------------------------------------------------------------
	 */
	void Servo::mgrUpdatePosition(uint8_t new_position) {
		_position = new_position;
	}

	/* --------------------------------------------------------------------------------------------
	 * mgrUpdateStatus
	 * Function for the Manager : updates servo's status
	 * --------------------------------------------------------------------------------------------
	 */
	void Servo::mgrUpdateStatus(uint8_t new_status_error, uint8_t new_status_detail) {
		_status_error = new_status_error;
		_status_detail = new_status_detail;
	}


	/* GETTERS */

	uint8_t Servo::get_status_error() const {
		return _status_error;
	}

	uint8_t Servo::get_status_detail() const {
		return _status_detail;
	}

	uint8_t Servo::get_id() const {
		return _id;
	}

	uint16_t Servo::get_position() const {
		return _position;
	}

	bool Servo::is_torque_on() const {
		return (_status_detail & constants::StatusDetail::MotorOnFlag) > 0;
	}

	bool Servo::is_in_position() const {
		return (_status_detail & constants::StatusDetail::InpositionFlag) > 0;
	}

	bool Servo::is_moving() const {
		return (_status_detail & constants::StatusDetail::MovingFlag) > 0;
	}

	constants::LedColor::LedColorEnum Servo::get_inposition_led_color() const {
		return _inposition_led_color;
	}

	constants::LedColor::LedColorEnum Servo::get_moving_led_color() const {
		return _moving_led_color;
	}


	/* SETTERS */

	void Servo::set_position(uint16_t newPosition) {
		_bus->send_SJOG_msg(_id, constants::jog_default_playtime, newPosition, constants::JOG_CMD::PositionMode | 0x04);
		_desired_position = newPosition;
	}

	void Servo::enable_torque(bool value) {
		if(value) {
			_bus->send_RAM_write_msg(_id, constants::RAMAddr::TorqueControl, constants::TorqueControl::TorqueOn);
		} else {
			_bus->send_RAM_write_msg(_id, constants::RAMAddr::TorqueControl, constants::TorqueControl::TorqueFree);
		}
		_desired_torque_on = value;
	}

	void Servo::set_inposition_led_color(constants::LedColor::LedColorEnum led_color) {
		_inposition_led_color = led_color;
	}

	void Servo::set_moving_led_color(constants::LedColor::LedColorEnum led_color) {
		_moving_led_color = led_color;
	}

	void Servo::reboot() {
		_bus->send_reboot_msg(_id);
		_should_reboot = true;
	}
}

#endif
