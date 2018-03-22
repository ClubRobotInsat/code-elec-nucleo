#include "HerkulexBus.h"
#include "HerkulexConst.h"
#include "HerkulexManager.h"
#include "HerkulexServo.h"
#include "mbed.h"

#include <trame.h>

using namespace herkulex;
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0xFD;

Serial pc(USBTX, USBRX, 9600);
Serial logger(D8, D2, 9600);

herkulex::Manager<6> servo_manager(A0, A1, 2, &pc);

void traiterTrameServo(Trame trame_servo);

// Forme un uint16_t avec 2 uint8_t
uint16_t paquetage(uint8_t msb, uint8_t lsb) {
	uint16_t resultat = 0x0000;
	resultat = msb;
	resultat = resultat << 8;
	resultat |= lsb;
	return resultat;
}

void init_servo() {
	pc.printf("Initialisation des servos\n\r");
	Servo* servo_fd = servo_manager.registerNewServo(0xFD);
	servo_fd->setPosition(512);
	servo_fd->reboot();
	Servo* servo_03 = servo_manager.registerNewServo(0x03);
	servo_03->setPosition(512);
	servo_03->reboot();
}

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
							data[j] = pc->getc();
						}
						done = true;
					}
				}
			}
		}
	}
	// pc->write(Trame::makeAck(numPaquet), 15, NULL, 0);
	return Trame(id, cmd, data_length, data, numPaquet);
}

void afficherTrame(Trame trame) {
	pc.printf("Trame reçue : id %#x | cmd %#x | data_length %#x | data ", trame.getId(), trame.getCmd(), trame.getDataLength());
	uint8_t* data = trame.getData();
	for(int i = 0; i < trame.getDataLength(); i++) {
		pc.printf("%#x ", data[i]);
	}
	pc.printf("\n\r");
}

void traiterTrame(Trame trame) {
	uint8_t id;
	// Si une trame est dans le buffer on la recupere
	id = trame.getId();
	if(id == 1) {
		pc.printf("Trame can reçue\n\r");
	} else if(id == 2) {
		pc.printf("Trame servo reçue\n\r");
		traiterTrameServo(trame);
	} else {
		pc.printf("Id de trame invalide\n\r");
	}
}

void traiterTrameServo(Trame trame_servo) {
	uint8_t id_servo = 0;
	uint16_t angle = 0;

	// longueur generique d'une trame servo
	uint8_t longueur_trame_servo = 3;

	// valeurs recuperees dans la trame
	uint8_t angle_lsb = 0;
	uint8_t angle_msb = 0;

	// Si la trame fait la bonne taille (3 octets)
	if(trame_servo.getDataLength() == longueur_trame_servo) {
		switch(trame_servo.getCmd()) {
			case 0x01: {
				// 1er octet = id
				// octets 2 et 3 = angle (position)

				// Recuperer l'id de la trame
				id_servo = (trame_servo.getData())[0];

				// Concatener les 2 octets du champs data de la trame
				angle_msb = (trame_servo.getData())[1];
				angle_lsb = (trame_servo.getData())[2];
				angle = paquetage(angle_msb, angle_lsb);

				// Faire tourner le servo concerne
				herkulex::Servo* servo_commande = servo_manager.getServoById(id_servo);
				if(servo_commande != nullptr) {
					pc.printf("Déplacement du servo %#x \n\r", id_servo);
					servo_commande->setPosition(angle);
				} else {
					pc.printf("idServo non trouve\n\r");
				}
				break;
			}

			default:
				pc.printf("Erreur commande trame\n\r");
				break;
		}
	} else {
		pc.printf("Erreur longueur de trame\n\r");
	}
}

int main() {
	init_servo();
	while(true) {
		Trame trame = lire_trame(&pc);
		traiterTrame(trame);
	}
}
