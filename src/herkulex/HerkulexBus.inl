#ifndef HERKULEX_BUS_INL
#define HERKULEX_BUS_INL

namespace herkulex
{
	/* --------------------------------------------------------------------------------------------
	 * sendEEPWriteMsg 
	 * Construit un message d'ecriture dans la ROM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendEEPWriteMsg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t lsb, uint8_t len, uint8_t msb) {
		// Check valid length
		if(len < 1 || len > 2)
		{
			_log->printf("Utilisation de Bus::sendEEPWriteMsg avec une longueur erronee !\n");
		} 
		else 
		{
			// NEW
			uint8_t * data = new uint8_t(2 + len);
	
			data[0] = addr;
			data[1] = len;
			data[2] = lsb;
			if(len > 1) {
				data[3] = msb;
			}
	
			sendMsg(id, constants::CMD::toServo::EEPWrite, data, (2 + len));

			// DELETE
			delete data; 
		}
	}

	/* --------------------------------------------------------------------------------------------
	 * sendEEPReadMsg
	 * Construit un de lecture dans la ROM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * !!! Ne realise pas l'operation de lecture !!! 
	 * --------------------------------------------------------------------------------------------
	 */	
	void Bus::sendEEPReadMsg(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t len) {
		// Check valid length
		if(len < 1 || len > 2)
		{
			_log->printf("Utilisation de Bus::sendEEPReadMsg avec une longueur erronee !\n");
		} 
		else 
		{
			uint8_t data[2];

			data[0] = addr;
			data[1] = len;

			sendMsg(id, constants::CMD::toServo::EEPRead, data, 2);
		}
	}


	/* --------------------------------------------------------------------------------------------
	 * sendRAMWriteMsg
	 * Construit un message d'ecriture dans la RAM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendRAMWriteMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t lsb, uint8_t len, uint8_t msb) {
		// Check valid length
		if(len < 1 || len > 2)
		{
			_log->printf("Utilisation de Bus::sendRAMWriteMsg avec une longueur erronee !\n");
		} 
		else 
		{
			// NEW
			uint8_t * data = new uint8_t(2 + len);

			data[0] = addr;
			data[1] = len;
			data[2] = lsb;
			if(len > 1) {
				data[3] = msb;
			}

			sendMsg(id, constants::CMD::toServo::RAMWrite, data, 2 + len);

			// DELETE
			delete data;
		}
	}

	/* --------------------------------------------------------------------------------------------
	 * sendRAMReadMsg
	 * Construit un de lecture dans la RAM, et l'envoie avec Bus::sendMsg.
	 * len < 2
	 * !!! Ne realise pas l'operation de lecture !!! 
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendRAMReadMsg(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t len) {
		// Check valid length
		if(len < 1 || len > 2)
		{
			_log->printf("Utilisation de Bus::sendRAMWriteMsg avec une longueur erronee !\n");
		} 
		else 
		{
			// NEW
			uint8_t * data = new uint8_t(2 + len);

			data[0] = addr;
			data[1] = len;

			sendMsg(id, constants::CMD::toServo::RAMRead, data, 2);

			// DELETE
			delete data;
		}
	}

	/* --------------------------------------------------------------------------------------------
	 * sendIJOGMsg
	 * Envoi d'une commande IJOG par Bus::sendMsg
	 * jogFlags est une liste de drapeaux, on peut trouver la definition dans la doc Herkulex : 
	 * -> champ SET d'un paquet IJOG
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendIJOGMsg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set) {
		// NEW
		uint8_t * data = new uint8_t(5); 

		data[0] = (jogValue & 0xff); 
		data[1] = (jogValue & 0xff00) >> 8; 
		data[2] = set; 
		data[3] = id; 
		data[4] = playtime; 

		sendMsg(id, constants::CMD::toServo::IJOG, data, 5); 

		// DELETE
		delete data; 
	}

	/* --------------------------------------------------------------------------------------------
	 * sendSJOGMsg
	 * Envoi d'une commande SJOG par Bus::sendMsg
	 * jogFlags est une liste de drapeaux, on peut trouver la definition dans la doc Herkulex : 
	 * -> champ SET d'un paquet SJOG
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendSJOGMsg(uint8_t id, uint8_t playtime, uint16_t jogValue, uint8_t set) {
		// NEW
		uint8_t * data = new uint8_t(5); 

		data[0] = playtime; 
		data[1] = (jogValue & 0xff); 
		data[2] = (jogValue & 0xff00) >> 8; 
		data[3] = set; 
		data[4] = id; 

		sendMsg(id, constants::CMD::toServo::SJOG, data, 5); 

		// DELETE
		delete data; 
	}

	/* --------------------------------------------------------------------------------------------
	 * sendStatMsg
	 * Envoi un message de demande de status
!!! NB: Ambiguite dans la documentation (6.7 p.49) sur la contenance de data[0] et data[1] pour 
!!! la requete de status
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendStatMsg(uint8_t id) {
		// NEW 
		uint8_t * data = new uint8_t(2); 

		data[0] = 0; 
		data[1] = 0; 

		sendMsg(id, constants::CMD::toServo::Stat, data, 2); 

		// DELETE
		delete data; 
	}

	/* --------------------------------------------------------------------------------------------
	 * sendRollbackMsg
	 * Envoi un message de rollback : remise en parametre d'usine (voir doc. p.49)
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendRollbackMsg(uint8_t id, bool skipIDRollback, bool skipBaudrateRollback) {
		// NEW 
		uint8_t * data = new uint8_t(2); 

		data[0] = (skipIDRollback ? 1 : 0); 
		data[1] = (skipBaudrateRollback ? 1 : 0); 

		sendMsg(id, constants::CMD::toServo::Rollback, data, 2); 

		// DELETE
		delete data; 		
	}

	/* --------------------------------------------------------------------------------------------
	 * sendRebootMsg
	 * Envoi un message de reboot
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::sendRebootMsg(uint8_t id) {
		sendMsg(id, constants::CMD::toServo::Rollback); 
	}

	/* --------------------------------------------------------------------------------------------
	 * readEEPAddr
	 * Read at a specified address in a servo's EEP. 
	 * Callback prototype example : 
	 * 	void callback(uint8_t id, uint8_t status_error, uint8_t status_detail, uint8_t data0, uint8_t data1 = 0); 
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::readEEPAddr(uint8_t id, constants::EEPAddr::EEPAddrEnum addr, uint8_t len, 
		Callback<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)> * callback) 
	{ 
		sendEEPReadMsg(id, addr, len); 
		_callback_read_addr = callback;
		_expected_reply_id = id; 
		_expected_reply_cmd = constants::CMD::fromServo::EEPReadAck; 
	}

	/* --------------------------------------------------------------------------------------------
	 * readRAMAddr
	 * Read at a specified address in a servo's RAM. 
	 * Callback prototype example : 
	 * 	void callback(uint8_t id, uint8_t status_error, uint8_t status_detail, uint8_t data0, uint8_t data1 = 0); 
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::readRAMAddr(uint8_t id, constants::RAMAddr::RAMAddrEnum addr, uint8_t len, 
		Callback<void(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)> * callback)
	{
		sendRAMReadMsg(id, addr, len); 
		_callback_read_addr = callback;
		_expected_reply_id = id; 
		_expected_reply_cmd = constants::CMD::fromServo::RAMReadAck; 
	}

	/* --------------------------------------------------------------------------------------------
	 * readStat
	 * Read the status of a servo. 
	 * Callback prototype example : 
	 * 	void callback(uint8_t id, uint8_t status_error, uint8_t status_detail); 
	 * --------------------------------------------------------------------------------------------
	 */
	void Bus::readStat(uint8_t id, Callback<void(uint8_t, uint8_t, uint8_t)> * callback) {
		sendStatMsg(id); 
		_callback_read_stat = callback; 
		_expected_reply_id = id; 
		_expected_reply_cmd = constants::CMD::fromServo::StatAck;
	}

	void Bus::parseAddrMsg() {
		// Check checksum validity
		uint8_t chksum1 = (_buffer[2] ^ _buffer[3] ^ _buffer[4] ^ _buffer[7] ^ _buffer[8] ^ _buffer[9] ^ _buffer[10] ^
		    _buffer[11] ^ _buffer[12]) & 0xFE;
		uint8_t chksum2 = (~_buffer[5] & 0xFE);

		if(chksum1 != _buffer[5] || chksum2 != _buffer[6]) 
		{
			debug("Bad position Checksum");
		}
		else if(_callback_read_addr == nullptr) 
		{
			debug("Callback for read addr is nullptr.");
		}
		else 
		{
			// Check carried data length
			if(_buffer[8] == 1)
			{
				// Call the callback with status error, status detail, data[0] - id allready checked
				_callback_read_addr->call(_expected_reply_id, _buffer[10], _buffer[11], _buffer[9], 0);
			}
			else
			{
				// Call the callback with status error, status detail, data[0], data[1] - id allready checked
				_callback_read_addr->call(_expected_reply_id, _buffer[11], _buffer[12], _buffer[9], _buffer[10]);
			}

			// Reset the callback storage member
			_callback_read_addr = nullptr;
		}
	}

	void Bus::parseStatMsg() {
		// Check checksum validity
		uint8_t chksum1 = (_buffer[2] ^ _buffer[3] ^ _buffer[4] ^ _buffer[7] ^ _buffer[8]) & 0xFE;
		uint8_t chksum2 = (~_buffer[5] & 0xFE);

		if(chksum1 != _buffer[5] || chksum2 != _buffer[6])
		{
			debug("Bad status checksum.");
		}
		else if(_callback_read_stat == nullptr) 
		{
			debug("Callback for read status is nullptr.");
		}
		else 
		{
			// Call the callback with status error and status detail - id allready checked
			_callback_read_stat->call(_expected_reply_id, _buffer[7], _buffer[8]);

			// Reset the callback storage member
			_callback_read_stat = nullptr;
		}
	}
}

#endif