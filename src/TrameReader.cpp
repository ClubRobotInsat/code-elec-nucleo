#include "TrameReader.h"

void print_state(TrameReaderState state) {
	switch(state) {
		case TrameReaderState::WAITING_FOR_H1:
			printf("WAITING_FOR_H1\n\r");
			break;
		case TrameReaderState::WAITING_FOR_H2:
			printf("WAITING_FOR_H2\n\r");
			break;
		case TrameReaderState::WAITING_FOR_H3:
			printf("WAITING_FOR_H3\n\r");
			break;
		case TrameReaderState::WAITING_FOR_BYTE_1_ID_CMD:
			printf("WAITING_FOR_BYTE_1_ID_CMD\n\r");
			break;
		case TrameReaderState::WAITING_FOR_BYTE_2_ID_CMD:
			printf("WAITING_FOR_BYTE_2_ID_CMD\n\r");
			break;
		case TrameReaderState::WAITING_FOR_DATA:
			printf("WAITING_FOR_DATA\n\r");
			break;
		case TrameReaderState::WAITING_FOR_DATA_LENGTH:
			printf("WAITING_FOR_DATA_LENGTH\n\r");
			break;
		case TrameReaderState::WAITING_FOR_NUMPAQUET:
			printf("WAITING_FOR_NUMPAQUET\n\r");
			break;
		case TrameReaderState::WAITING_FOR_TRAME_TYPE:
			printf("WAITING_FOR_TRAME_TYPE\n\r");
			break;
	}
}

TrameReader::TrameReader()
        : _trame_buffer()
        , _byte_buffer(nullptr)
        , _byte_buffer_size(255)
        , _state(TrameReaderState::WAITING_FOR_H1)
        , _data_received(0)
        , _ser(nullptr)
        , _trame_in_build()
        , _input_buffer{0}
        , _read_done(Callback<void(int)>(this, &TrameReader::handle_buffer)) {
	_byte_buffer = new uint8_t[_byte_buffer_size];
}

void TrameReader::attach_to_serial(Serial* ser) {
	_ser = ser;
	ser->read((uint8_t*)&_input_buffer, READ_SIZE, _read_done, SERIAL_EVENT_RX_ALL);
}

void TrameReader::handle_buffer(int e) {
	for(uint8_t byte : _input_buffer) {
		_read_buffer.push(byte);
	}

	_ser->read((uint8_t*)&_input_buffer, READ_SIZE, _read_done, SERIAL_EVENT_RX_ALL);
}

void TrameReader::parse_buffer() {
	while(not _read_buffer.empty()) {
		uint8_t byte;
		_read_buffer.pop(byte);
		// printf("Byte : %#x",byte);
		// printf(" | State :");
		// print_state(_state);
		// printf("\n\r");
		parse_byte(byte);
	}
}

void TrameReader::parse_byte(uint8_t byte) {
	switch(_state) {
		case TrameReaderState::WAITING_FOR_H1: {
			if(byte == 0xAC) {
				_state = TrameReaderState::WAITING_FOR_H2;
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
			if(byte < 8) {
				_trame_in_build.data_length = byte;
				_state = TrameReaderState::WAITING_FOR_DATA;
			} else {
				_state = TrameReaderState::WAITING_FOR_H1;
			}
			break;
		}
		case TrameReaderState::WAITING_FOR_DATA: {
			debug("Byte for data : %#x \n\r", byte);
			/* On reçois tout sauf le dernier octet */
			if(not _trame_in_build.data_length == 0 and _data_received <= _trame_in_build.data_length - 1) {
				_byte_buffer[_data_received] = byte;
				_data_received++;
				break;
			}
			/* On reçois le dernier octet */
			else {
				_byte_buffer[_data_received] = byte;
				uint8_t id = Trame::demultiplex_id(_trame_in_build.byte_1_id_and_cmd, _trame_in_build.byte_2_id_and_cmd);
				uint8_t cmd = Trame::demultiplex_cmd(_trame_in_build.byte_1_id_and_cmd, _trame_in_build.byte_2_id_and_cmd);
				debug("id : %#x | num : %#x | cmd : %#x | (%d)", id, _trame_in_build.num_paquet, cmd, _trame_in_build.data_length);
				for(int i = 0; i < _trame_in_build.data_length; i++) {
					debug("%#x ", _byte_buffer[i]);
				}
				debug("\n\r");
				Trame t = Trame(id, cmd, _trame_in_build.data_length, _byte_buffer, _trame_in_build.num_paquet);
				_trame_buffer.push(t);
			}

			_state = TrameReaderState::WAITING_FOR_H1;
			_data_received = 0;
			break;
		}

		default:
			break;
	}
}

Trame TrameReader::get_trame() {
	if(_trame_buffer.empty()) {
		error("Empty Trame Buffer \n\r");
		return Trame();
	} else {
		Trame result;
		_trame_buffer.pop(result);
		return result;
	}
}
bool TrameReader::trame_ready() const {
	return not _trame_buffer.empty();
}
