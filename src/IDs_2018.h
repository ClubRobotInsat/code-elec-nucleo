/**
 * @file 	mon_header.h
 * @brief 	Header commun aux codes info et elec pour partager des informations communes
 *
 * @details     Permet par exemple d'avoir dans un même fichier les ID des différentes cartes
 *              ou bien encore leur types de messages.
 */

/**
 * Date historique de fondation de ce fichier légendaire : le 2 avril 2015.
 */

#ifndef HEADER_COMMUN
#define HEADER_COMMUN

////////////////////////
// ID DES CARTES ELEC //
////////////////////////

/// IDs des cartes

#define ID_NUCLEO 0
#define ID_ELEC_CARD_MOVEMENT 1
// La nucléo gère directement l'IO, les servos et les moteurs
#define ID_ELEC_CARD_SERVOS 2
#define ID_ELEC_CARD_IO 3
#define ID_ELEC_CARD_AVOIDANCE 4
#define ID_ELEC_CARD_MOTORS 5

/// IDs des servos
#define ID_SERVO_CUBE_DOOR 3
#define ID_SERVO_HOLD_DOOR_LEFT 2
#define ID_SERVO_HOLD_DOOR_RIGHT 0
#define ID_SERVO_BEE 0xFD
#define ID_SERVO_LOQUET 1

/// IDs des contacteurs
#define ID_CONTACTOR_TIRETTE 2

/// IDs des moteurs
// Moteurs asservis
#define ID_MOTOR_ELEVATOR_LEFT 1
#define ID_MOTOR_ELEVATOR_RIGHT 2
// Moteurs non asservis
#define ID_MOTOR_SWALLOW_LEFT 3
#define ID_MOTOR_SWALLOW_RIGHT 4
// Brushless
#define ID_MOTOR_BRUSHLESS_LEFT 5
#define ID_MOTOR_BRUSHLESS_RIGHT 6

#endif /* HEADER_COMMUN */
