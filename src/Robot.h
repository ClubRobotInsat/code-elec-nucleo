#ifndef ROBOT_C
#define ROBOT_C

#include "Brushless.h"
#include "HerkulexManager.h"
#include "Motor.h"
#include "TrameReader.h"
#include <mbed.h>

class Robot {

	Robot();

	/* Appelle toutes les fonctions pour gérer le robot */
	void manage_robot();

private:
	/* Traite une trame que l'on a reçue */
	void handle_trame(Trame trame);

	void handle_trame_can(Trame trame);

	void handle_trame_motor(Trame trame);

	void handle_trame_servo(Trame trame);

	void handle_trame_nucleo(Trame trame);

	void handle_trame_io(Trame trame);

	// La connexion CAN avec l'ancienne éléctronique
	CAN _can;

	// La connexion série avec l'informatique
	Serial _pc;

	// Les servomoteurs numériques
	herkulex::Manager<6> _servo_manager;

	// Moteurs brushless
	Brushless _turbine_left;
	Brushless _turbine_right;

	// Moteurs asservis
	Motor _motor_elevator_left;
	Motor _motor_elevator_right;

	// Moteurs non asservis
	PwmOut _motor_swallow_left;
	PwmOut _motor_swallow_right;

	// La tirette
	DigitalIn _tirette;

	// La machine à état permettant de lire des trames.
	TrameReader _trame_reader;
};


#endif
