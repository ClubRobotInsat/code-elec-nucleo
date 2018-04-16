#include "HerkulexBus.h"
#include "HerkulexConst.h"
#include "HerkulexManager.h"
#include "HerkulexServo.h"
#include "TrameReader.h"
#include "trame.h"
#include "mbed.h"

using namespace herkulex;
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0xFD;

Serial pc(PC_6, PC_7, 9600);

//herkulex::Manager<6> servo_manager(A0, A1, 2);

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
	/*
	debug("Initialisation des servos\n\r");
	Servo* servo_fd = servo_manager.registerNewServo(0xFD);
	servo_fd->setPosition(512);
	servo_fd->reboot();
	Servo* servo_03 = servo_manager.registerNewServo(0x03);
	servo_03->setPosition(512);
	servo_03->reboot();
	servo_manager.sendUpdatesToNextServo();
	servo_manager.sendUpdatesToNextServo();
	servo_manager.flushBus();
	wait_ms(50);
	servo_manager.sendUpdatesToNextServo();
	servo_manager.sendUpdatesToNextServo();
	servo_manager.flushBus();
	*/
}

void afficherTrame(Trame trame) {
	printf("Trame reçue : id %#x | n° %#x | cmd %#x | data_length %#x | data ", trame.getId(), trame.get_packet_number(), trame.getCmd(), trame.getDataLength());
	uint8_t* data = trame.getData();
	for(int i = 0; i < trame.getDataLength(); i++) {
		printf("%#x ", data[i]);
	}
	printf("\n\r");
}

void traiterTrame(Trame trame) {
	uint8_t id;
	// Si une trame est dans le buffer on la recupere
	id = trame.getId();
	if(id == 1) {
		debug("Trame can reçue\n\r");
	} else if(id == 2) {
		debug("Trame servo reçue\n\r");
		traiterTrameServo(trame);
	} else {
		debug("Id de trame invalide\n\r");
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
			case 0x05: {
				// 1er octet = id
				// octets 2 et 3 = angle (position)

				// Recuperer l'id de la trame
				id_servo = (trame_servo.getData())[0];

				// Concatener les 2 octets du champs data de la trame
				angle_msb = (trame_servo.getData())[1];
				angle_lsb = (trame_servo.getData())[2];
				angle = paquetage(angle_msb, angle_lsb);

				// Faire tourner le servo concerne
				/*herkulex::Servo* servo_commande = servo_manager.getServoById(id_servo);
				if(servo_commande != nullptr) {
					debug("Déplacement du servo %#x \n\r", id_servo);
					servo_commande->setPosition(angle);
				} else {
					debug("idServo non trouve\n\r");
				}
				*/
				break;
			}

			default:
				debug("Erreur commande trame\n\r");
				break;
		}
	} else {
		debug("Erreur longueur de trame\n\r");
	}
}


uint8_t test[8] = {1,2,3,4,5,6,7,8};

int main() {
	printf("\n\rStart\n\r");
	pc.set_dma_usage_tx(DMA_USAGE_OPPORTUNISTIC);
	pc.set_dma_usage_rx(DMA_USAGE_OPPORTUNISTIC);
	TrameReader reader;
	init_servo();
	reader.attach_to_serial(&pc);
	while(true) {
		if (reader.trame_ready()) {
			Trame trame = reader.get_trame();
			//afficherTrame(trame);
			//traiterTrame(trame);
			//	printf("sending ack\n\r");
			Trame::send_ack(trame.get_packet_number(),&pc);
			//	printf("ack sent\n\r");
		}
		//printf("Send\n\r");
		//pc.write((uint8_t *)&test,8,0,0);
		//servo_manager.sendUpdatesToNextServo();
		//servo_manager.flushBus();
		//	wait_ms(150);
	}
}
