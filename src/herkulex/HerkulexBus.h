/* --------------------------------------------------------------------------------------------
 * maFonction
 * La description claire et succinte de ce que fait ma fonction 
 * --------------------------------------------------------------------------------------------
 */

#ifndef HERKULEX_BUS_H
#define HERKULEX_BUS_H

#include "HerkulexConst.h"
#include "HerkulexServo.h"

namespace herkulex {
	template <uint8_t N_SERVOS>
	class Manager;

	/* TODO : Revoir le mecanisme de lecture, 
	 * le bus n'a pas a stocker de Servo. 
	 * Il doit notifier une reponse d'un servo (identifié par un ID) au manager. 
	 * L'ideal etant que le manager s'occupe de chaque servo de maniere periodique. 
	 * On pourrait ainsi borner le temps d'attente pour la reponse d'un servo, et 
	 * s'en servir comme timeout pour une fonction de lecture blocante. 
	 * Dans ce cas la, on eviterait de rechercher par ID quel servo a repondu, du cote du manager. 
	 * En sachant que la solution : bus qui update le servo sans passer par le manager n'est vraiment pas propre...   
	 */ 
	class Bus { 
	protected: 
		template <uint8_t N_SERVOS>
		friend class Manager;

		/* --------------------------------------------------------------------------------------------
		 * Constructeur (explicit)
		 * Construit un bus serie pour communiquer avec les servos sur les pin txPin et rxPin. 
		 * Log son activite sur le Serial pointe par log.
		 * Le baudrate de la com. peut etre specifie.  
		 * --------------------------------------------------------------------------------------------
		 */
		explicit Bus(PinName txPin, PinName rxPin, Serial * log, uint32_t baudrate = 115200);

		/* --------------------------------------------------------------------------------------------
		 * Destructeur 
		 * Desalloue le Serial utilise par le bus
		 * --------------------------------------------------------------------------------------------
		 */
		virtual ~Bus();

		/* --------------------------------------------------------------------------------------------
		 * write
		 * Prend un buffer (data), de taille fixe (length), passe par addresse et l'ecoule sur le bus 
		 * --------------------------------------------------------------------------------------------
		 */
		void write(uint8_t* data, uint8_t length);

		/* --------------------------------------------------------------------------------------------
		 * sendMsg
		 * Construit un message pour les servos, et l'envoi immediatement sur le bus. 
		 * --------------------------------------------------------------------------------------------
		 */ 
		void sendMsg(const uint8_t id, const constants::CMD::toServo::toServoEnum cmd, const uint8_t* data, const uint8_t length);


		/* --------------------------------------------------------------------------------------------
		 * sendEEPWriteMsg
		 * Construit un message d'ecriture dans la ROM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * --------------------------------------------------------------------------------------------
		 */
		void sendEEPWriteMsg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t lsb, uint8_t len = 1, uint8_t msb = 0x00);

		/* --------------------------------------------------------------------------------------------
		 * sendEEPReadMsg
		 * Construit un de lecture dans la ROM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * !!! Ne realise pas l'operation de lecture !!! 
		 * --------------------------------------------------------------------------------------------
		 */		
		void sendEEPReadMsg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t len = 1);

		/* --------------------------------------------------------------------------------------------
		 * sendRAMWriteMsg
		 * Construit un message d'ecriture dans la RAM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * --------------------------------------------------------------------------------------------
		 */
		void sendRAMWriteMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t lsb, uint8_t len = 1, uint8_t msb = 0x00);

		/* --------------------------------------------------------------------------------------------
		 * sendRAMReadMsg
		 * Construit un de lecture dans la RAM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * !!! Ne realise pas l'operation de lecture !!! 
		 * --------------------------------------------------------------------------------------------
		 */
		void sendRAMReadMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t len = 1);


		// void sendIJOGMsg();
		// void sendSJOGMsg();
		// void sendStatMsg();
		// void sendRollbackMsg();
		// void sendRebootMsg();

		template <uint8_t ID, constants::CMD::fromServo::fromServoEnum CMD>
		inline uint8_t readMsg(uint8_t* message);


		/* You should not call this method directly but prefer Servo::updatePosition. */
		void fetchPosition(Servo* servo);

		/* You should not call this method directly but prefer Servo::updateStatus. */
		void fetchStatus(Servo* servo);


		void parseStatusMessage(Servo* servo);

		void parsePositionMessage(Servo* servo);



		void cbInterpretBuffer(int event);

	private: 
		volatile bool _callback_waiting;

		Serial* _ser; // TODO : Changer en value

		Serial* _log;

		event_callback_t _read_callback;

		uint8_t _buffer[13];

		Servo* _servo_registered_for_callback; // ??? 
	};
}

#endif
