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
        , _trame_in_build()
        , _input_buffer{0}
        , _read_done(Callback<void(int)>(this, &TrameReader::handle_buffer)) {
	_trame_buffer = new Trame[_trame_buffer_size];
	_byte_buffer = new uint8_t[_byte_buffer_size];
}

void TrameReader::attach_to_serial(Serial* ser) {
	ser->read((uint8_t*)&_input_buffer, READ_BUFFER_SIZE, _read_done, SERIAL_EVENT_RX_ALL);
	_ser = ser;
}

void TrameReader::handle_buffer(int e) {
	for(uint8_t byte : _input_buffer) {
		this->parse_byte(byte);
	}
	_ser->read((uint8_t*)&_input_buffer, READ_BUFFER_SIZE, _read_done, SERIAL_EVENT_RX_ALL);
}

void TrameReader::parse_byte(uint8_t byte) {
	switch(_state) {
		case TrameReaderState::WAITING_FOR_H1: {
			if(byte == 0xAC) {
				this->_state = TrameReaderState::WAITING_FOR_H2;
			} else {
				_state = TrameReaderState::WAITING_FOR_H1;
			}
			break;
		}
		case TrameReaderState::WAITING_FOR_H2: {
			if(byte == 0xDC) {
				_state = TrameReaderState::WAITING_FOR_H3;
			} else {
				_state = TrameReaderState::WAITING_FOR_H1;
			}
			break;
		}
		case TrameReaderState::WAITING_FOR_H3: {
			if(byte == 0xAB)
				_state = TrameReaderState::WAITING_FOR_TRAME_TYPE;
			else {
				_state = TrameReaderState::WAITING_FOR_H1;
			}
			break;
		}
		case TrameReaderState::WAITING_FOR_TRAME_TYPE: {
			if(byte == 0xBA)
				_state = TrameReaderState::WAITING_FOR_NUMPAQUET;
			else {
				_state = TrameReaderState::WAITING_FOR_H1;
			}
			break;
		}
		case TrameReaderState::WAITING_FOR_NUMPAQUET: {
			_trame_in_build.num_paquet = byte;
			_state = TrameReaderState::WAITING_FOR_BYTE_1_ID_CMD;
			break;
		}
		case TrameReaderState::WAITING_FOR_BYTE_1_ID_CMD: {
			_trame_in_build.byte_1_id_and_cmd = byte;
			_state = TrameReaderState::WAITING_FOR_BYTE_2_ID_CMD;
			break;
		}
		case TrameReaderState::WAITING_FOR_BYTE_2_ID_CMD: {
			_trame_in_build.byte_2_id_and_cmd = byte;
			_state = TrameReaderState::WAITING_FOR_DATA_LENGTH;
			break;
		}
		case TrameReaderState::WAITING_FOR_DATA_LENGTH: {
			_trame_in_build.data_length = byte;
			_state = TrameReaderState::WAITING_FOR_DATA;
			break;
		}
		case TrameReaderState::WAITING_FOR_DATA: {
			/* On reçois tout sauf le dernier octet */
			if(not _trame_in_build.data_length == 0 and _data_received < _trame_in_build.data_length - 1) {
				_byte_buffer[_data_received] = byte;
				_data_received++;
				break;
			}
			/* On reçois le dernier octet */
			else {
				_byte_buffer[_data_received] = byte;
				uint8_t id = Trame::demultiplexId(_trame_in_build.byte_1_id_and_cmd, _trame_in_build.byte_2_id_and_cmd);
				uint8_t cmd = Trame::demultiplexCmd(_trame_in_build.byte_1_id_and_cmd, _trame_in_build.byte_2_id_and_cmd);
				if(_trame_buffer_position < _trame_buffer_size) {
					_trame_buffer[_trame_buffer_position] =
					    Trame(id, cmd, _trame_in_build.data_length, _byte_buffer, _trame_in_build.num_paquet);
				}

				/* The buffer is full we dump all the Trame */
				else {
					_trame_buffer_position = 0;
					_trame_buffer[_trame_buffer_position] =
					    Trame(id, cmd, _trame_in_build.data_length, _byte_buffer, _trame_in_build.num_paquet);
				}

				_trame_buffer_position++;
				_state = TrameReaderState::WAITING_FOR_H1;
				_data_received = 0;
				break;
			}
		}

		default:
			break;
	}
}

Trame TrameReader::get_trame() {
	if(_trame_buffer_position == 0) {
		error("Empty Trame Buffer \n\r");
		return Trame();
	} else {
		debug("Renvoi d'une bonne trame : \n\r");
		_trame_buffer_position--;
		return _trame_buffer[_trame_buffer_position];
	}
}
bool TrameReader::trame_ready() const {
	return _trame_buffer_position > 0;
}
