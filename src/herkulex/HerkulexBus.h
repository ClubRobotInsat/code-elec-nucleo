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
		 * Construit un message de lecture dans la ROM, et l'envoie avec Bus::sendMsg.
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
		 * Construit un message de lecture dans la RAM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * !!! Ne realise pas l'operation de lecture !!! 
		 * --------------------------------------------------------------------------------------------
		 */
		void sendRAMReadMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t len = 1);

		/* --------------------------------------------------------------------------------------------
		 * sendIJOGMsg
		 * Envoi d'une commande IJOG par Bus::sendMsg
		 * Utiliser des | entre flags de l'enum constants::JOG_CMD::JOG_CMDEnum pour composer SET
		 * --------------------------------------------------------------------------------------------
		 */
		void sendIJOGMsg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set);

		/* --------------------------------------------------------------------------------------------
		 * sendSJOGMsg
		 * Envoi d'une commande SJOG par Bus::sendMsg
		 * Utiliser des | entre flags de l'enum constants::JOG_CMD::JOG_CMDEnum pour composer SET
		 * --------------------------------------------------------------------------------------------
		 */
		void sendSJOGMsg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set);

		/* --------------------------------------------------------------------------------------------
		 * sendStatMsg
		 * Envoi un message de demande de status
!!! NB: Ambiguite dans la documentation (6.7 p.49) sur la contenance de data[0] et data[1] pour 
!!! la requete de status
		 * --------------------------------------------------------------------------------------------
		 */
		void sendStatMsg(uint8_t id);

		/* --------------------------------------------------------------------------------------------
		 * sendRollbackMsg
		 * Envoi un message de rollback : remise en parametre d'usine (voir doc. p.49)
		 * --------------------------------------------------------------------------------------------
		 */
		void sendRollbackMsg(uint8_t id, bool skipIDRollback = true, bool skipBaudrateRollback = true);

		/* --------------------------------------------------------------------------------------------
		 * sendRebootMsg
		 * Envoi un message de reboot
		 * --------------------------------------------------------------------------------------------
		 */
		inline void sendRebootMsg(uint8_t id);

		/* --------------------------------------------------------------------------------------------
		 * cbInterpretBuffer
		 * Callback pour lire et interpreter un message recu. Globalement, switch sur le type de 
		 * message et appelle le parser correspondant au message.
		 * --------------------------------------------------------------------------------------------
		 */
		void cbInterpretBuffer(int event);


		/* You should not call this method directly but prefer Servo::updatePosition. */
		void fetchPosition(Servo* servo);

		/* You should not call this method directly but prefer Servo::updateStatus. */
		void fetchStatus(Servo* servo);


		void parseStatusMessage(Servo* servo);

		void parsePositionMessage(Servo* servo);




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
