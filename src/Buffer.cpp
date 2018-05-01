#include "Buffer.h"

// Le tableau est copié lors de l'appel.
Buffer::Buffer(uint8_t* data, uint8_t size)
        : _data(nullptr), _size(size), _destroy(Callback<void(int)>(this, &Buffer::destroy)) {
	_data = new uint8_t[size];
	memcpy(_data, data, size);
}
Buffer::~Buffer() {
	delete[] _data;
}

uint8_t* Buffer::get_buffer() const {
	return this->_data;
}

void Buffer::destroy(int) {
	if(index < 8) {
		Buffer::buffers_to_delete[index] = this;
		index++;
	}
}

void Buffer::write(Serial* ser) {
	ser->write(_data, _size, _destroy, SERIAL_EVENT_TX_ALL);
}

void Buffer::delete_buffers() {
	for(int i = 0; i < index; i++) {
		delete buffers_to_delete[i];
	}
	index = 0;
}
uint8_t Buffer::index = 0;
Buffer* Buffer::buffers_to_delete[MAX_SEND_BUFFER]{};
