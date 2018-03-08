#include "HerkulexBus.h"
#include "HerkulexConst.h"
#include "HerkulexManager.h"
#include "mbed.h"

#include <trame.h>

using namespace herkulex;
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0xFD;

Serial pc(USBTX, USBRX, 9600);

CircularBuffer<Trame, 256> file_attente;

Trame lire_trame(Serial* pc) {
	uint8_t numPaquet, id, cmd, data_length;
	uint8_t* data;
	bool done = false;
	while(not done) {
		if(pc->getc() == 0xAC) {
			if(pc->getc() == 0xDC) {
				if(pc->getc() == 0xAB) {
					if(pc->getc() == 0xBA) {
						numPaquet = pc->getc();
						uint8_t aux1 = pc->getc();
						uint8_t aux2 = pc->getc();
						id = Trame::demultiplexId(aux1, aux2);
						cmd = Trame::demultiplexCmd(aux1, aux2);
						data_length = pc->getc();
						data = new uint8_t[data_length];
						for(int j = 0; j < data_length; j++) {
							*data = pc->getc();
						}
						done = true;
					}
				}
			}
		}
	}
	pc->write(Trame::makeAck(numPaquet), 15, NULL, 0);
	return Trame(id, cmd, data_length, data);
}

void mettreTrameDansFileAttente() {
	while(true) {
		file_attente.push(lire_trame(&pc));
	}
}

void afficherTrame() {
	if(not file_attente.empty()) {
		Trame trame;
		file_attente.pop(trame);
		debug("Trame reçue : id %#x | cmd %#x | data_length %#x | data ", trame.getId(), trame.getCmd(), trame.getDataLength());
		uint8_t* data = trame.getData();
		for(int i = 0; i < trame.getDataLength(); i++) {
			debug("%#x ", data[i]);
		}
		debug("\n\r");
	}
}

int main() {
	mettreTrameDansFileAttente();
	Ticker ticker;
	ticker.attach(afficherTrame, 0.005);
	while(true) {
	}
}
