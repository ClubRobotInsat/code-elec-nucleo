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

	/* La classe bus gère un bus UART.
	 * Elle propose plusieurs types de messages et permet de les envoyer.
	 * A noter qu'aucun message n'est réellement envoyé tant que l'on a pas flush le bus.
	 */
	class Bus {
	public:
		/* --------------------------------------------------------------------------------------------
		 * Constructeur (explicit)
		 * Construit un bus serie pour communiquer avec les servos sur les pin txPin et rxPin.
		 * Le baudrate de la com peut etre specifie.
		 * --------------------------------------------------------------------------------------------
		 */
		explicit Bus(PinName txPin, PinName rxPin, uint32_t baudrate = 115200, float flush_frequency = 0.2);

		/* --------------------------------------------------------------------------------------------
		 * Destructeur
		 * Desalloue le Serial utilise par le bus
		 * --------------------------------------------------------------------------------------------
		 */
		virtual ~Bus();

		/*
		 * Renvoie vrai si il y a des messages à envoyer.
		 */
		bool need_flush();

		/*
		 * Envoie tous les messages du buffer sur le bus.
		 */
		void flush();

		/* --------------------------------------------------------------------------------------------
		 * sendMsg
		 * Construis un message et l'ajoute à la liste des messages devant être envoyé lors du prochain flush.
		 * --------------------------------------------------------------------------------------------
		 */
		void send_msg(const uint8_t id, const constants::CMD::toServo::toServoEnum cmd, const uint8_t* data = nullptr, const uint8_t length = 0);


		// - TODO - Optimize Read/Write functions (same operations, duplicated... )
		// 		  - Maybe templates for length field (optimization && compile-time error generation is bad value)
		/* --------------------------------------------------------------------------------------------
		 * sendEEPWriteMsg
		 * Construit un message d'ecriture dans la ROM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * --------------------------------------------------------------------------------------------
		 */
		void send_EEP_write_msg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t lsb, uint8_t len = 1, uint8_t msb = 0x00);

		/* --------------------------------------------------------------------------------------------
		 * sendEEPReadMsg
		 * Construit un message de lecture dans la ROM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * !!! Ne realise pas l'operation de lecture !!!
		 * --------------------------------------------------------------------------------------------
		 */
		void send_EEP_read_msg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t len = 1);

		/* --------------------------------------------------------------------------------------------
		 * sendRAMWriteMsg
		 * Construit un message d'ecriture dans la RAM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * --------------------------------------------------------------------------------------------
		 */
		void send_RAM_write_msg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t lsb, uint8_t len = 1, uint8_t msb = 0x00);

		/* --------------------------------------------------------------------------------------------
		 * sendRAMReadMsg
		 * Construit un message de lecture dans la RAM, et l'envoie avec Bus::sendMsg.
		 * len < 2
		 * !!! Ne realise pas l'operation de lecture !!!
		 * --------------------------------------------------------------------------------------------
		 */
		void send_RAM_read_msg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t len = 1);

		/* --------------------------------------------------------------------------------------------
		 * sendIJOGMsg
		 * Envoi d'une commande IJOG par Bus::sendMsg
		 * Utiliser des | entre flags de l'enum constants::JOG_CMD::JOG_CMDEnum pour composer SET
		 * --------------------------------------------------------------------------------------------
		 */
		void send_IJOG_msg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set);

		/* --------------------------------------------------------------------------------------------
		 * sendSJOGMsg
		 * Envoi d'une commande SJOG par Bus::sendMsg
		 * Utiliser des | entre flags de l'enum constants::JOG_CMD::JOG_CMDEnum pour composer SET
		 * --------------------------------------------------------------------------------------------
		 */
		void send_SJOG_msg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set);

		/* --------------------------------------------------------------------------------------------
		 * sendStatMsg
		 * Envoi un message de demande de status
!!! NB: Ambiguite dans la documentation (6.7 p.49) sur la contenance de data[0] et data[1] pour
!!! la requete de status
		 * --------------------------------------------------------------------------------------------
		 */
		void send_stat_msg(uint8_t id);

		/* --------------------------------------------------------------------------------------------
		 * sendRollbackMsg
		 * Envoi un message de rollback : remise en parametre d'usine (voir doc. p.49)
		 * --------------------------------------------------------------------------------------------
		 */
		void send_rollback_msg(uint8_t id, bool skipIDRollback = true, bool skipBaudrateRollback = true);

		/* --------------------------------------------------------------------------------------------
		 * sendRebootMsg
		 * Envoi un message de reboot
		 * --------------------------------------------------------------------------------------------
		 */
		void send_reboot_msg(uint8_t id);

		void send_debug_message();

	protected:
		/* Envoie un message du buffer vers le bus. */
		void flush_one_message();

		/* Cette méthode est appelée quand le flush est terminé */
		void cb_write_done(int e);

	private:
		Ticker _ticker_flush;

		volatile bool _callback_waiting;
		volatile bool _write_done;

		Serial _ser;

		event_callback_t _read_callback;
		event_callback_t _write_callback;

		uint8_t _buffer[13];

		// Hold data that needs to be written.
		CircularBuffer<uint8_t*, 256> _buffer_write_data;

		// Hold the length of the data that needs to be written.
		CircularBuffer<uint8_t, 256> _buffer_write_length;

		uint32_t _total_write_length;

		Callback<void(uint8_t, uint8_t, uint8_t)>* _callback_read_stat;
		Callback<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)>* _callback_read_addr;

		uint8_t _expected_reply_id;
		constants::CMD::fromServo::fromServoEnum _expected_reply_cmd;

		uint8_t* _data_written;
	};
}

#endif
