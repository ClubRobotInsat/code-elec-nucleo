#include "mbed.h"

/* Un buffer qui s'auto-détruit après écriture */
class Buffer {

	public :
		Buffer(uint8_t* data, uint8_t size) : _data(data) , _size(size), _destroy(Callback<void(int)>(this,&Buffer::destroy)) {}
		~Buffer() {
			delete[] _data;
		}

		uint8_t* get_buffer() const {
			return this->_data;
		}

		void destroy(int) {
			printf("Destruction d'un buffer\n\r");
			delete this;
		}

		void write(Serial* ser) {

			ser->write(_data,_size,_destroy,SERIAL_EVENT_TX_ALL);
		}

	private :
		uint8_t* _data;
		uint8_t _size;

		event_callback_t _destroy;

};
