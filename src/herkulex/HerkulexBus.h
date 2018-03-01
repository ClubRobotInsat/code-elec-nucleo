/* --------------------------------------------------------------------------------------------
 * maFonction
 * La description claire et succinte de ce que fait ma fonction 
 * --------------------------------------------------------------------------------------------
 */

#ifndef HERKULEX_BUS_H
#define HERKULEX_BUS_H

#include "HerkulexConst.h"
#include "platform/CircularBuffer.h"

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
	public: 
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

		/*
		 * Return true if there is some message that need to be flushed.
		 */
		bool needFlush();

		/*
		 * This function will issue a call to write.
		 */
		void flush();
		/* --------------------------------------------------------------------------------------------
		 * sendMsg
		 * Construct a message and add it to the list of message to be sent with the next flush. 
		 * --------------------------------------------------------------------------------------------
		 */ 
		void sendMsg(const uint8_t id, const constants::CMD::toServo::toServoEnum cmd, 
			const uint8_t* data = nullptr, const uint8_t length = 0);


	// - TODO - Optimize Read/Write functions (same operations, duplicated... )
	// 		  - Maybe templates for length field (optimization && compile-time error generation is bad value)
		/* --------------------------------------------------------------------------------------------
		 * sendEEPWriteMsg
		 * Construit un message d'ecriture dans la ROM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * --------------------------------------------------------------------------------------------
		 */
		inline void sendEEPWriteMsg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t lsb, 
			uint8_t len = 1, uint8_t msb = 0x00);

		/* --------------------------------------------------------------------------------------------
		 * sendEEPReadMsg
		 * Construit un message de lecture dans la ROM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * !!! Ne realise pas l'operation de lecture !!! 
		 * --------------------------------------------------------------------------------------------
		 */		
		inline void sendEEPReadMsg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t len = 1);

		/* --------------------------------------------------------------------------------------------
		 * sendRAMWriteMsg
		 * Construit un message d'ecriture dans la RAM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * --------------------------------------------------------------------------------------------
		 */
		inline void sendRAMWriteMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t lsb, 
			uint8_t len = 1, uint8_t msb = 0x00);

		/* --------------------------------------------------------------------------------------------
		 * sendRAMReadMsg
		 * Construit un message de lecture dans la RAM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * !!! Ne realise pas l'operation de lecture !!! 
		 * --------------------------------------------------------------------------------------------
		 */
		inline void sendRAMReadMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t len = 1);

		/* --------------------------------------------------------------------------------------------
		 * sendIJOGMsg
		 * Envoi d'une commande IJOG par Bus::sendMsg
		 * Utiliser des | entre flags de l'enum constants::JOG_CMD::JOG_CMDEnum pour composer SET
		 * --------------------------------------------------------------------------------------------
		 */
		inline void sendIJOGMsg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set);

		/* --------------------------------------------------------------------------------------------
		 * sendSJOGMsg
		 * Envoi d'une commande SJOG par Bus::sendMsg
		 * Utiliser des | entre flags de l'enum constants::JOG_CMD::JOG_CMDEnum pour composer SET
		 * --------------------------------------------------------------------------------------------
		 */
		inline void sendSJOGMsg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set);

		/* --------------------------------------------------------------------------------------------
		 * sendStatMsg
		 * Envoi un message de demande de status
!!! NB: Ambiguite dans la documentation (6.7 p.49) sur la contenance de data[0] et data[1] pour 
!!! la requete de status
		 * --------------------------------------------------------------------------------------------
		 */
		inline void sendStatMsg(uint8_t id);

		/* --------------------------------------------------------------------------------------------
		 * sendRollbackMsg
		 * Envoi un message de rollback : remise en parametre d'usine (voir doc. p.49)
		 * --------------------------------------------------------------------------------------------
		 */
		inline void sendRollbackMsg(uint8_t id, bool skipIDRollback = true, bool skipBaudrateRollback = true);

		/* --------------------------------------------------------------------------------------------
		 * sendRebootMsg
		 * Envoi un message de reboot
		 * --------------------------------------------------------------------------------------------
		 */
		inline void sendRebootMsg(uint8_t id);

		/* --------------------------------------------------------------------------------------------
		 * readEEPAddr
		 * Read at a specified address in a servo's EEP. 
		 * Callback prototype example : 
		 * 	void callback(uint8_t id, uint8_t status_error, uint8_t status_detail, uint8_t data0, uint8_t data1 = 0); 
		 * --------------------------------------------------------------------------------------------
		 */
		inline void readEEPAddr(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t len, 
			Callback<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)> * callback);

		/* --------------------------------------------------------------------------------------------
		 * readRAMAddr
		 * Read at a specified address in a servo's RAM. 
		 * Callback prototype example : 
		 * 	void callback(uint8_t id, uint8_t status_error, uint8_t status_detail, uint8_t data0, uint8_t data1 = 0); 
		 * --------------------------------------------------------------------------------------------
		 */
		inline void readRAMAddr(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t len, 
			Callback<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)> * callback);

		/* --------------------------------------------------------------------------------------------
		 * readStat
		 * Read the status of a servo. 
		 * Callback prototype example : 
		 * 	void callback(uint8_t id, uint8_t status_error, uint8_t status_detail); 
		 * --------------------------------------------------------------------------------------------
		 */
		inline void readStat(uint8_t id, Callback<void(uint8_t, uint8_t, uint8_t)> * callback);

		void sendDebugMessage();

	protected: 

		void cbWriteDone(int e);

		/* --------------------------------------------------------------------------------------------
		 * write
		 * Prend un buffer (data), de taille fixe (length), passe par addresse et l'ecoule sur le bus 
		 * --------------------------------------------------------------------------------------------
		 */
		void write(uint8_t* data, uint8_t length);

		/* --------------------------------------------------------------------------------------------
		 * cbInterpretBuffer
		 * Callback pour lire et interpreter un message recu. Globalement, switch sur le type de 
		 * message et appelle le parser correspondant au message.
		 * --------------------------------------------------------------------------------------------
		 */
		void cbInterpretBuffer(int event);

		/* --------------------------------------------------------------------------------------------
		 * parseAddrMsg
		 * This function is an helper. It should not be called outside of cbInterpretBuffer. 
		 * Parses a addr read (EEP or RAM) ack message received from a servo.
		 * --------------------------------------------------------------------------------------------
		 */
		inline void parseAddrMsg();

		/* --------------------------------------------------------------------------------------------
		 * parseStatMsg
		 * This function is an helper. It should not be called outside of cbInterpretBuffer. 
		 * Parses a status ack message received from a servo.
		 * --------------------------------------------------------------------------------------------
		 */
		inline void parseStatMsg();


	private: 
		volatile bool _callback_waiting;
		volatile bool _write_done;

		Serial _ser; 

		Serial* _log;

		event_callback_t _read_callback;
		event_callback_t _write_callback;

		uint8_t _buffer[13];

		// Hold data that needs to be written.
		CircularBuffer<uint8_t*,256> _buffer_write_data;

		// Hold the length of the data that needs to be written.
		CircularBuffer<uint8_t,256> _buffer_write_length;

		uint32_t _total_write_length;

		Callback<void(uint8_t, uint8_t, uint8_t)> * _callback_read_stat;
		Callback<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)> * _callback_read_addr; 

		uint8_t _expected_reply_id; 
		constants::CMD::fromServo::fromServoEnum _expected_reply_cmd; 
	};
}

// Definitions of the inline functions, included here
#include "HerkulexBus.inl"

#endif
