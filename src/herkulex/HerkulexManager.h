#ifndef HERKULEX_MANAGER_H
#define HERKULEX_MANAGER_H

#define BUS_BAUDRATE 115200

#include "HerkulexBus.h"
#include "HerkulexServo.h"
#include <mbed.h>

namespace herkulex {
	/* Le Manager s'occupe de créer des servos et de les surveiller :
	 * Il peut enregistrer un nouveau servo sur le bus et traiter les messages qui le concerne.
	 * Le paramètre `N_SERVOS` indique le nombre maximal de servo que pourra gérer le Manager. */
	template <uint8_t N_SERVOS>
	class Manager {
	public:
		explicit Manager(PinName txPin, PinName rxPin);

		virtual ~Manager();

		/* Envoie un message sur le bus qui demande à tous les servos de s'identifier */
		void send_debug_message();

		/*
		 * Renvoie un pointeur vers le servo concerné par `id` si le manager à un servo enregistré avec cet ID.
		 * Sinon retourne nullptr.
		 */
		Servo* get_servo_by_id(uint8_t id);


		/*
		 * Crée un nouveau servo avec pour id `id`.
		 */
		Servo* register_new_servo(uint8_t id);

		/*
		 * Envoie tous les messages nous envoyés sur le bus.
		 */
		void flush_bus();

	private:
		/* Stocke le bus. */
		Bus _bus;

		/* Les servos que manipule le manager */
		Servo* _servos[N_SERVOS];

		/* Stocke le numbre de servos actuellement enregistré.
		 * Si nb_reg_servos > N_SERVOS lève une erreur.
		 */
		uint8_t _nb_reg_servos;
	};
}

#include "HerkulexManager.hpp"

#endif
