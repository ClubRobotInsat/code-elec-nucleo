#include "Trame.h"
#include <CircularBuffer.h>
#include <mbed.h>

#ifndef DEF_TRAME_READER
#define DEF_TRAME_READER

// Le nombre d'octets que l'on demande à mbed de nous lire à chaque fois.
#define READ_SIZE 8

// Le nombre d'octets que l'on peut stocker
#define READ_BUFFER_SIZE 256

// Le nombre de trames que l'on peut stocker.
#define TRAME_BUFFER_SIZE 128

/* Une énumeration qui représente les différents états que l'on peut avoir lorsque on lis des trames depuis une
 * connexion série */
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
/* TODO : Utiliser un buffer circulaire */
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
		delete[] _byte_buffer;
	}

	/* Attache la machine à état sur une connexion série */
	void attach_to_serial(Serial* ser);

	/* Renvoie une trame si le buffer n'est pas vide. Il faut impérativement vérifier avec `TrameReader::trame_ready`
	 * que le buffer a des données */
	Trame get_trame();

	/* Renvoie vrai si le buffer contiens au moins une trame. */
	bool trame_ready() const;

	void parse_buffer();

private:
	void make_trame();

	/* S'occupe d'un buffer en le traitant octet par octet. */
	void handle_buffer(int e);

	/* S'occupe d'analyser un octet. */
	void parse_byte(uint8_t byte);

	/* Le buffer contenant les trames */
	CircularBuffer<Trame, TRAME_BUFFER_SIZE> _trame_buffer;

	/* Le buffer contenant les octets de données */
	uint8_t* _byte_buffer;
	/* La taille du buffer */
	volatile uint8_t _byte_buffer_size;

	/* L'état actuel de la machine à état */
	volatile TrameReaderState _state;

	/* Le nombre d'octet de donnée que l'on a reçu à l'état actuel */
	volatile uint8_t _data_received;

	/* La connexion série potentiellement non initialisé (il faut appeler TrameRead::attach_to_serial) */
	Serial* _ser;

	/* La trame en cours de construction */
	WIPTrame _trame_in_build;

	/* Le buffer que l'on donne à la couche mbed pour la lecture */
	uint8_t _input_buffer[READ_SIZE];

	CircularBuffer<uint8_t, READ_BUFFER_SIZE> _read_buffer;

	/* Le callback que l'on donne à mbed pour la lecture */
	event_callback_t _read_done;
};
#endif
