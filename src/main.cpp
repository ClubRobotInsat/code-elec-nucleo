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

CircularBuffer<Trame, 256> file_attente;

herkulex::Manager<6> servo_manager (D8,D2,500,&pc);

void traiterTrameServo(Trame trame_servo);

// Forme un uint16_t avec 2 uint8_t
uint16_t paquetage(uint8_t msb,uint8_t lsb)
{
	uint16_t resultat = 0x0000;
	resultat = msb;
	resultat << 8;
	resultat |= lsb;
	return resultat;
}

void init_servo()
{
	Servo* servo_fd = servo_manager.registerNewServo(0xFD);
	servo_fd->reboot();
	Servo* servo_03 = servo_manager.registerNewServo(0x03);
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

void traiterTrame(){
	Trame trame_traitee;
	uint8_t id;
	// Si une trame est dans le buffer on la recupere
	if(file_attente.pop(trame_traitee))
	{
		id = trame_traitee.getId();
		if(id == 1)
		{
			// traiter can
		}
		else if(id == 2)
		{
			traiterTrameServo(trame_traitee);
		}
		else
		{
			// erreur
		}
	}
}

void traiterTrameServo(Trame trame_servo)
{
	uint8_t id_servo = 0;
	uint16_t angle = 0;
	
	// longueur generique d'une trame servo
	uint8_t longueur_trame_servo = 3;

	herkulex::Servo * servo_commande = NULL;

	// valeurs recuperees dans la trame
	uint8_t angle_lsb = 0;
	uint8_t angle_msb = 0;

	// Si la trame fait la bonne taille (3 octets)
	if(trame_servo.getDataLength() == longueur_trame_servo)
	{
		switch(trame_servo.getCmd())
		{
		case 0x05 :
		// 1er octet = id
		// octets 2 et 3 = angle (position)

		// Recuperer l'id de la trame
		id_servo = (trame_servo.getData())[0];

		// Concatener les 2 octets du champs data de la trame
		angle_msb = (trame_servo.getData())[1];
		angle_lsb = (trame_servo.getData())[2];
		angle = paquetage(angle_msb,angle_lsb);

		// Faire tourner le servo concerne
		if((servo_commande = servo_manager.getServoById(id_servo)) != NULL)
		{
			servo_commande->setPosition(angle);
		}
		else
		{
			debug("idServo non trouve\n");
		}
		break;

		default:
		debug("Erreur commande trame\n");
		break;
		}
	}
	else
	{
		debug("Erreur longueur de trame\n");
	}
	
}

int main() {
	init_servo();
	Ticker ticker;
	ticker.attach(traiterTrame, 0.005);
	while(true) 
	{
		mettreTrameDansFileAttente();
	}
}
