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

	void destroy(int);

	void write(Serial* ser);

	static void delete_buffers();

private:
	static Buffer* buffers_to_delete[MAX_SEND_BUFFER];
	static uint8_t index;
	uint8_t* _data;
	uint8_t _size;

	event_callback_t _destroy;
};

#endif
