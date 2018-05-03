#ifndef CONFIG_ROBOT_C
#define CONFIG_ROBOT_C

/* Variable de configuration du robot */
#define DMA_USAGE_ROBOT DMA_USAGE_OPPORTUNISTIC // La politique d'utilisation des DMAs sur le robot
#define BAUD_RATE_RPI_NUCLEO 115200             // Le baudrate de la connexion nucleo <-> info
#define BAUD_RATE_SERVO 115200                  // Le baudrate de la connexion servo <-> nucleo

/* Variable de configuration des moteurs */
#define MOTOR_MIN_PWM 0.01 // La PWM minimale que l'on sors aux moteurs
#define MOTOR_MAX_PWM 0.2
#define MOTOR_DEAD_ZONE 150 // Le nombre de pulse de tolérance sur l'asservissement en position d'un moteur

#endif
