#include <mbed.h>

#ifndef ROBOT_BUFFER_C
#define ROBOT_BUFFER_C
#define MAX_SEND_BUFFER 8

/* Un buffer qui s'auto-détruit après écriture */
class Buffer {

public:
	Buffer() = default;

	// Le tableau est copié lors de l'appel.
	Buffer(uint8_t* data, uint8_t size);
	~Buffer();

	uint8_t* get_buffer() const;

	void write();

	static void delete_buffers();

	static void flush_buffers(Serial* ser);

private:

	static void write_callback(int e);

	static volatile bool write_done;
	static Buffer* buffers_to_delete[MAX_SEND_BUFFER];
	static Buffer* buffers_to_write[MAX_SEND_BUFFER];
	static uint8_t index_delete;
	static uint8_t index_write;
	static uint8_t* data_wrote;
	static event_callback_t destroy;

	uint8_t* _data;
	uint8_t _size;

};

#endif
