#include "mbed.h"
#include "trame.h"


#ifndef DEF_TRAME_READER
#define DEF_TRAME_READER
enum class TrameReaderState {
	WAITING_FOR_H1,
	WAITING_FOR_H2,
	WAITING_FOR_H3,
	WAITING_FOR_TRAME_TYPE,
	WAITING_FOR_NUMPAQUET,
	WAITING_FOR_BYTE_1_ID_CMD,
	WAITING_FOR_BYTE_2_ID_CMD,
	WAITING_FOR_DATA_LENGTH,
	WAITING_FOR_DATA,
};

void print_state(TrameReaderState state);

/* Une machine à état qui lis des trames depuis une connexion série */
class TrameReader {
	/* Contiens une trame en cours de construction */
	class WIPTrame {
	public:
		WIPTrame() : byte_1_id_and_cmd(0), byte_2_id_and_cmd(0), num_paquet(0), data_length(0) {}
		uint8_t byte_1_id_and_cmd;
		uint8_t byte_2_id_and_cmd;
		uint8_t num_paquet;
		uint8_t data_length;
	};

public:
	TrameReader();

	~TrameReader() {
		debug("DESTURCTION \n\r");
	};

	void attach_to_serial(Serial* ser);

	Trame* get_trame();

private:
	void handle_buffer();

	void parse_byte(uint8_t byte);


	Trame** _trame_buffer;
	uint8_t _trame_buffer_size;
	uint8_t _trame_buffer_position;
	uint8_t* _byte_buffer;
	uint8_t _byte_buffer_size;
	TrameReaderState _state;
	uint8_t _data_received;
	Serial* _ser;
	WIPTrame _trame_in_build;
};
#endif
