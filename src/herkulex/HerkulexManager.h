#ifndef HERKULEX_MANAGER_H
#define HERKULEX_MANAGER_H

#include "HerkulexBus.h"
#include "HerkulexServo.h"
#include <mbed.h>

namespace herkulex {
	template <uint8_t N_SERVOS>
	class Manager {
	private:
		// Stocke le bus
		Bus _bus;

		// Logger
		Serial* _log;

		// The ticker calls a callback when the period is elapsed
		// Used to update all servos at a defined rate
		Ticker _it_ticker;

		// The manager stores servos
		Servo* _servos[N_SERVOS];

		// This member stores the number of registred servos.
		// It must raise an error/a warning if _nb_reg_servos != N_SERVOS
		uint8_t _nb_reg_servos;

		// Le numero du prochain servo a manager
		uint8_t _num_next_servo; 

		// Periode de raffraichissement -> le temps ecoule entre la mise
		// a jour de deux servos stockes consecutivement dans _servos
		// sera de _refreshPeriod / N_SERVOS (? ou _nb_reg_servos ? a voir)
		const us_timestamp_t _refreshPeriod; // TODO : RENAME (naming conv...)

		Callback<void(uint8_t, uint8_t, uint8_t)> _callback_update_servo_status; 
		Callback<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)> _callback_update_servo; 


	public:
		/*
		 * Return a pointer to a Servo if there is a registered Servo with the specified id.
		 * Otherwise return nullptr.
		 */
		Servo* getServoById(uint8_t id);

		explicit Manager(PinName txPin, PinName rxPin, us_timestamp_t _refreshPeriod, Serial* pc);
		virtual ~Manager();

		// This function is used to get a reference to a servo object with desired ID
		Servo* registerNewServo(uint8_t id);

	private:
		void cbSendUpdatesToNextServo(); 

		void cbFuncUpdateServoStatus(uint8_t id, uint8_t status_error, uint8_t status_detail);
		void cbFuncUpdateServo(uint8_t id, uint8_t status_error, uint8_t status_detail, 
			uint8_t data0, uint8_t data1 = 0); // Checker l'addresse lue dans le Ack coté bus

		inline void updateServo(uint8_t id); 

		void updateAllServos(); // ??
	};
}

// Templated source file, included in the header
#include "HerkulexManager.tcc"

#endif
