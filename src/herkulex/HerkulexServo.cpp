#include "HerkulexServo.h"
#include "HerkulexBus.h"

namespace herkulex {
	Servo::Servo(uint8_t id, Bus& bus, Serial* log) : _id(id), _status(0), _position(0), _bus(bus), _log(log) {
		_log->printf("Hello from servo \n");
	}

	void Servo::mgrUpdateId(uint8_t new_id)
	{
		_id = new_id; 
	}

	void Servo::mgrUpdatePosition(uint8_t new_position)
	{
		_position = new_position; 
	} 

	void Servo::mgrUpdateStatus(uint8_t new_status_error, uint8_t new_status_detail)
	{
		_status_error = new_status_error; 
		_status_detail = new_status_detail; 
		_torque_on = static_cast<bool>(_status_detail & constants::StatusDetail::MotorOnFlag);
	} 

	uint16_t Servo::getPosition() const {
		return _position;
	}

	uint8_t Servo::getStatusError() const {
		return _status_error; 
	}

	uint8_t Servo::getStatusDetail() const {
		return _status_detail;
	}

	uint8_t Servo::getId() const {
		return _id;
	}

	void Servo::enableTorque(bool value) {
		_desired_torque_on = value; 
	}

	bool Servo::isTorqueOn() const {
		return _torque_on; 
	}

	void Servo::setDefaultLedColor(constants::LedColor led_color) {
		_default_led_color = led_color; 
	}

	constants::LedColor Servo::getDefaultLedColor() const {
		return _default_led_color; 
	}

	void Servo::setMovingLedColor(constants::LedColor led_color) {
		_moving_led_color = led_color; 
	}

	constants::LedColor Servo::getMovingLedColor() const {
		return _moving_led_color; 
	}

	void Servo::setPosition(uint16_t newPosition) {
		_desired_position = newPosition; 
	}
}
