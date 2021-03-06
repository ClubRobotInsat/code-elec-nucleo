#ifndef ROBOT_C
#define ROBOT_C

#include "Brushless.h"
#include "HerkulexManager.h"
#include "Motor.h"
#include "Motor2018.h"
#include "SimpleMotor.h"
#include "TrameReader.h"
#include <CircularBuffer.h>
#include <mbed.h>

class Robot {
public:
	Robot();

	/* Initialise la partie mécanique du robot. */
	void initialize_meca();

	/* Appelle toutes les fonctions pour gérer le robot
	 * Cette fonction doit être appelée regulièrement.  */
	void manage_robot();

	static void print_trame(const Trame& trame);

	/* Fonction utilitaire pour répondre facilement pong. */
	void send_pong(uint8_t id);

private:
	/* Traite une trame que l'on a reçue */
	void handle_trame(Trame trame);

	void handle_trame_can(Trame trame);

	void handle_trame_servo(Trame trame);

	void handle_trame_nucleo(Trame trame);

	void handle_trame_io(Trame trame);

	void callback_tirette();

	/* La méthode appelée quand un message est reçu depuis le bus CAN. */
	void read_trame_from_can();

	// La connexion CAN avec l'ancienne éléctronique.
	CAN _can;

	// La connexion série avec l'informatique.
	Serial _pc;

	// Les servomoteurs numériques.
	herkulex::Manager<6> _servo_manager;

	// Moteurs brushless.
	Brushless _turbine_left;
	Brushless _turbine_right;

	// Moteurs asservis.
	Motor _motor_elevator_left;
	Motor _motor_elevator_right;

	// Moteurs non asservis.
	SimpleMotor _motor_swallow_left;
	SimpleMotor _motor_swallow_right;

	// La tirette.
	InterruptIn _tirette;

	// La machine à état permettant de lire des trames.
	TrameReader _trame_reader;

	// Un buffer de trame lue depuis le CAN.
	CircularBuffer<Trame, 256> _trame_from_can_buffer;

	Motor2018 _motor_logic;

	volatile bool _tirette_detected;
};


#endif
