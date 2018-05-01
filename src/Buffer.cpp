#include "Buffer.h"

// Le tableau est copié lors de l'appel.
Buffer::Buffer(uint8_t* data, uint8_t size)
        : _data(nullptr), _size(size) {
	_data = new uint8_t[size];
	memcpy(_data, data, size);
}
Buffer::~Buffer() {
	delete[] _data;
}

uint8_t* Buffer::get_buffer() const {
	return this->_data;
}

void Buffer::write() {
	buffers_to_write[index_write] = this;
	index_write++;
}

void Buffer::delete_buffers() {
	delete[] data_wrote;
}

void Buffer::flush_buffers(Serial* ser) {
	uint32_t total_length = 0;
	for (int i = 0; i < Buffer::index_write; i++) {
		total_length += buffers_to_write[i]->_size;
	}
	uint8_t* data = new uint8_t[total_length];
	uint32_t ind = 0;
	for (int i = 0; i < Buffer::index_write; i++) {
		uint8_t length = buffers_to_write[i]->_size;
		for (int k = 0; k < length; k++) {
			data[ind + k] = buffers_to_write[i]->_data[k];
		}
		ind += length;
		delete buffers_to_write[i];
	}
	data_wrote = data;
	Buffer::write_done = false;
	ser->write(data,total_length,Buffer::destroy,SERIAL_EVENT_TX_ALL);
	index_write = 0;
}

void Buffer::write_callback(int) {
	Buffer::write_done = true;
}

uint8_t Buffer::index_delete = 0;
uint8_t Buffer::index_write = 0;
uint8_t* Buffer::data_wrote = nullptr;
volatile bool Buffer::write_done = false;
Buffer* Buffer::buffers_to_delete[MAX_SEND_BUFFER]{};
Buffer* Buffer::buffers_to_write[MAX_SEND_BUFFER]{};
event_callback_t Buffer::destroy = Callback<void(int)>(&Buffer::write_callback);
