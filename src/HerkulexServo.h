#ifndef HERKULEX_SERVO_H
#define HERKULEX_SERVO_H

#include "mbed.h"

namespace herkulex {
	class HerkulexBus;

	template <uint8_t N_SERVOS>
	class HerkulexManager;

	class HerkulexServo {
		template <uint8_t N_SERVOS>
		friend class HerkulexManager;

	private:
		void setId(uint8_t id);

	public:
		/* You should not call this constructor directly, but call HerkulexBus::makeNewServo */
		HerkulexServo(uint8_t id, HerkulexBus* bus);

		/* Accessors --- */

		/* Return the cached status */
		uint8_t getStatus() const;

		/* Return the id of the servo */
		uint8_t getId() const;

		/* Return the cached position */
		uint16_t getPosition() const;

		/* Settors ---
		 * These methods are not meant to be used outside of this library.
		 */

		/* Set the status to `newStatus` */
		void setStatus(uint8_t newStatus);

		/* Set the position to `newPosition` */
		void setPosition(uint16_t newPosition);

		/* Communication ---
		 * These methods communicates with the servo.
		 */

		/* Move the servo to the desired position. */
		void positionControl(uint16_t position, uint8_t playtime, uint8_t setLED) const;

		/* Send a clear command to the servo. */
		void clear() const;

		/* Send a torque command with `torque` as a parameter. */
		void setTorque(uint8_t torque) const;

		/* Control the velocity of the servomotor directly. */
		void velocityControl(int16_t speed, uint8_t setLED) const;

		/* Enable the LED. */
		void setLedOn() const;

		/* Send a message to the herkulex servo and update the cached value. This call can be blocking if a previous
		 * update-style did not receive an answer.		*/
		void updatePosition();
		/* Send a message to the herkulex servo and update the cached value. This call can be blocking if a previous
		 * update-style did not receive an answer.		*/
		void updateStatus();

	private:
		uint8_t _id;
		uint8_t _status;
		uint16_t _position;

		// The internal bus that communicates in serial with the servo.
		HerkulexBus* _bus;
	};
}

#endif
