#ifndef HERKULEX_SERVO_INL
#define HERKULEX_SERVO_INL

namespace herkulex {

	Servo::Servo(uint8_t id)
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

	bool Servo::shouldReboot() const {
		return _should_reboot;
	}
	/* --------------------------------------------------------------------------------------------
	 * getStatusError
	 * Return servo's StatusError. For further informations, refer to herkulex documentation p.39.
	 * Update rate defined from the refresh period of the manager.
	 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager).
	 * --------------------------------------------------------------------------------------------
	 */
	uint8_t Servo::getStatusError() const {
		return _status_error;
	}

	/* --------------------------------------------------------------------------------------------
	 * getStatusDetail
	 * Return servo's StatusDetail. For further informations, refer to herkulex documentation p.39.
	 * Update rate defined from the refresh period of the manager.
	 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager).
	 * --------------------------------------------------------------------------------------------
	 */
	uint8_t Servo::getStatusDetail() const {
		return _status_detail;
	}

	/* --------------------------------------------------------------------------------------------
	 * getId
	 * Return servo's ID.
	 * --------------------------------------------------------------------------------------------
	 */
	uint8_t Servo::getId() const {
		return _id;
	}

	/* --------------------------------------------------------------------------------------------
	 * getPosition
	 * Return servo's current position.
	 <!> Position range is 0~1023, as defined in datasheet. Angle is position * 0.325 <!>
	 * Update rate defined from the refresh period of the manager.
	 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager).
	 * --------------------------------------------------------------------------------------------
	 */
	uint16_t Servo::getPosition() const {
		return _position;
	}

	/* --------------------------------------------------------------------------------------------
	 * isTorqueOn
	 * Return servo's motor on (= torque on ) flag. (cf. status detail doc.).
	 * Update rate defined from the refresh period of the manager.
	 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager).
	 * --------------------------------------------------------------------------------------------
	 */
	bool Servo::isTorqueOn() const {
		return (_status_detail & constants::StatusDetail::MotorOnFlag) > 0;
	}

	/* --------------------------------------------------------------------------------------------
	 * isInPosition
	 * Return servo's inposition flag. (cf. status detail doc.).
	 * Update rate defined from the refresh period of the manager.
	 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager).
	 * --------------------------------------------------------------------------------------------
	 */
	bool Servo::isInPosition() const {
		return (_status_detail & constants::StatusDetail::InpositionFlag) > 0;
	}

	/* --------------------------------------------------------------------------------------------
	 * isMoving
	 * Return servo's moving flag. (cf. status detail doc.).
	 * Update rate defined from the refresh period of the manager.
	 * update rate = refresh period / number of servos (template argument N_SERVOS of the manager).
	 * --------------------------------------------------------------------------------------------
	 */
	bool Servo::isMoving() const {
		return (_status_detail & constants::StatusDetail::MovingFlag) > 0;
	}

	/* --------------------------------------------------------------------------------------------
	 * getInpositionLedColor
	 * Get the color of the led for a servo in position.
	 * --------------------------------------------------------------------------------------------
	 */
	constants::LedColor::LedColorEnum Servo::getInpositionLedColor() const {
		return _inposition_led_color;
	}

	/* --------------------------------------------------------------------------------------------
	 * getMovingLedColor
	 * Get the color of the led for a servo moving.
	 * --------------------------------------------------------------------------------------------
	 */
	constants::LedColor::LedColorEnum Servo::getMovingLedColor() const {
		return _moving_led_color;
	}


	/* SETTERS */


	/* --------------------------------------------------------------------------------------------
	 * setPosition
	 * Set a new position for the servo.
	 * Sent at a rate defined from the refresh period of the manager.
	 * sending rate = refresh period / number of servos (template argument N_SERVOS of the manager).
	 * --------------------------------------------------------------------------------------------
	 */
	void Servo::setPosition(uint16_t newPosition) {
		_desired_position = newPosition;
	}

	/* --------------------------------------------------------------------------------------------
	 * enableTorque
	 * Set a new torque mode for the servo (on/off).
	 * Sent at a rate defined from the refresh period of the manager.
	 * sending rate = refresh period / number of servos (template argument N_SERVOS of the manager).
	 * --------------------------------------------------------------------------------------------
	 */
	void Servo::enableTorque(bool value) {
		_desired_torque_on = value;
	}

	/* --------------------------------------------------------------------------------------------
	 * setInpositionLedColor
	 * Set the color of the led for a servo in position.
	 * Sent at a rate defined from the refresh period of the manager.
	 * sending rate = refresh period / number of servos (template argument N_SERVOS of the manager).
	 * --------------------------------------------------------------------------------------------
	 */
	void Servo::setInpositionLedColor(constants::LedColor::LedColorEnum led_color) {
		_inposition_led_color = led_color;
	}

	/* --------------------------------------------------------------------------------------------
	 * setMovingLedColor
	 * Set the color of the led for a servo moving.
	 * Sent at a rate defined from the refresh period of the manager.
	 * sending rate = refresh period / number of servos (template argument N_SERVOS of the manager).
	 * --------------------------------------------------------------------------------------------
	 */
	void Servo::setMovingLedColor(constants::LedColor::LedColorEnum led_color) {
		_moving_led_color = led_color;
	}

	void Servo::reboot() {
		_should_reboot = true;
	}
}

#endif
