#include "TrameReader.h"

void print_state(TrameReaderState state) {
	switch(state) {
		case TrameReaderState::WAITING_FOR_H1:
			debug("WAITING_FOR_H1\n\r");
			break;
		case TrameReaderState::WAITING_FOR_H2:
			debug("WAITING_FOR_H2\n\r");
			break;
		case TrameReaderState::WAITING_FOR_H3:
			debug("WAITING_FOR_H3\n\r");
			break;
		case TrameReaderState::WAITING_FOR_BYTE_1_ID_CMD:
			debug("WAITING_FOR_BYTE_1_ID_CMD\n\r");
			break;
		case TrameReaderState::WAITING_FOR_BYTE_2_ID_CMD:
			debug("WAITING_FOR_BYTE_2_ID_CMD\n\r");
			break;
		case TrameReaderState::WAITING_FOR_DATA:
			debug("WAITING_FOR_DATA\n\r");
			break;
		case TrameReaderState::WAITING_FOR_DATA_LENGTH:
			debug("WAITING_FOR_DATA_LENGTH\n\r");
			break;
		case TrameReaderState::WAITING_FOR_NUMPAQUET:
			debug("WAITING_FOR_NUMPAQUET\n\r");
			break;
		case TrameReaderState::WAITING_FOR_TRAME_TYPE:
			debug("WAITING_FOR_TRAME_TYPE\n\r");
			break;
	}
}

TrameReader::TrameReader()
        : _trame_buffer(nullptr)
        , _trame_buffer_size(255)
        , _trame_buffer_position(0)
        , _byte_buffer(nullptr)
        , _byte_buffer_size(255)
        , _state(TrameReaderState::WAITING_FOR_H1)
        , _data_received(0)
        , _ser(nullptr)
        , _trame_in_build() {
	_trame_buffer = new Trame*[_trame_buffer_size];
	_byte_buffer = new uint8_t[_byte_buffer_size];
}

void TrameReader::attach_to_serial(Serial* ser) {
	ser->attach(Callback<void()>(this, &TrameReader::handle_buffer),Serial::RxIrq);
	_ser = ser;
}

void TrameReader::handle_buffer() {
	debug("Handling serial buffer\n\r");
	while(_ser->readable()) {
		debug("STATE : ");
		print_state(_state);
		this->parse_byte(_ser->getc());
		debug("NEW STATE : ");
		print_state(_state);
	}
}

void TrameReader::parse_byte(uint8_t byte) {
	debug("Handling byte(%#x)\n\r", byte);
	switch(_state) {
		case TrameReaderState::WAITING_FOR_H1: {
			if(byte == 0xAC) {
				debug("H1 RECEIVED | New state :");
				this->_state = TrameReaderState::WAITING_FOR_H2;
				print_state(_state);
			} else {
				debug("Aborting receive \n\r");
				_state = TrameReaderState::WAITING_FOR_H1;
			}
			break;
		}
		case TrameReaderState::WAITING_FOR_H2: {
			if(byte == 0xDC) {
				debug("H2 RECEIVED\n\r");
				_state = TrameReaderState::WAITING_FOR_H3;
			} else {
				debug("Aborting receive \n\r");
				_state = TrameReaderState::WAITING_FOR_H1;
			}
			break;
		}
		case TrameReaderState::WAITING_FOR_H3: {
			if(byte == 0xAB)
				_state = TrameReaderState::WAITING_FOR_TRAME_TYPE;
			else {
				debug("Aborting receive \n\r");
				_state = TrameReaderState::WAITING_FOR_H1;
			}
			break;
		}
		case TrameReaderState::WAITING_FOR_TRAME_TYPE: {
			if(byte == 0xBA)
				_state = TrameReaderState::WAITING_FOR_NUMPAQUET;
			else {
				debug("Aborting receive \n\r");
				_state = TrameReaderState::WAITING_FOR_H1;
			}
			break;
		}
		case TrameReaderState::WAITING_FOR_NUMPAQUET: {
			_trame_in_build.num_paquet = byte;
			_state = TrameReaderState::WAITING_FOR_BYTE_1_ID_CMD;
			debug("Received packet number\n\r");
			break;
		}
		case TrameReaderState::WAITING_FOR_BYTE_1_ID_CMD: {
			_trame_in_build.byte_1_id_and_cmd = byte;
			_state = TrameReaderState::WAITING_FOR_BYTE_2_ID_CMD;
			debug("Received first byte of id \n\r");
			break;
		}
		case TrameReaderState::WAITING_FOR_BYTE_2_ID_CMD: {
			_trame_in_build.byte_2_id_and_cmd = byte;
			_state = TrameReaderState::WAITING_FOR_DATA_LENGTH;
			debug("Received second byte of id \n\r");
			break;
		}
		case TrameReaderState::WAITING_FOR_DATA_LENGTH: {
			_trame_in_build.data_length = byte;
			_state = TrameReaderState::WAITING_FOR_DATA;
			debug("Received data length \n\r");
			break;
		}
		case TrameReaderState::WAITING_FOR_DATA: {
			/* On reçois tout sauf le dernier octet */
			if(not _trame_in_build.data_length == 0 and _data_received < _trame_in_build.data_length - 1) {
				_byte_buffer[_data_received] = byte;
				_data_received++;
				debug("Recevied data. More to come... \n\r");
			}
			/* On reçois le dernier octet */
			else {
				_byte_buffer[_data_received] = byte;
				uint8_t id = Trame::demultiplexId(_trame_in_build.byte_1_id_and_cmd, _trame_in_build.byte_2_id_and_cmd);
				uint8_t cmd = Trame::demultiplexCmd(_trame_in_build.byte_1_id_and_cmd, _trame_in_build.byte_2_id_and_cmd);
				uint8_t* data = new uint8_t[_trame_in_build.data_length];
				memcpy(&data, &_byte_buffer, _trame_in_build.data_length);
				debug("Received all data, new trame created \n\r");
				if(_trame_buffer_position < _trame_buffer_size) {
					_trame_buffer[_trame_buffer_position] =
					    new Trame(id, cmd, _trame_in_build.data_length, data, _trame_in_build.num_paquet);
				}

				/* The buffer is full we dump all the Trame */
				else {
					debug("Trame buffer is full, dumping ... \n\r");
					_trame_buffer_position = 0;
					_trame_buffer[_trame_buffer_position] =
					    new Trame(id, cmd, _trame_in_build.data_length, data, _trame_in_build.num_paquet);
				}

				_trame_buffer_position++;
				_state = TrameReaderState::WAITING_FOR_H1;
				_data_received = 0;
			}
			break;
		}

		default:
			break;
	}
}

Trame* TrameReader::get_trame() {
	if(_trame_buffer_position == 0) {
		return nullptr;
	} else {
		debug("Renvoi d'une bonne trame : \n\r");
		_trame_buffer_position--;
		return _trame_buffer[_trame_buffer_position];
	}
}
