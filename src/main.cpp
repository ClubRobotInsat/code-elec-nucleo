#include "mbed.h"
#include "Servo.h"
#define PIN_ESC D9


Servo moteur(PIN_ESC);
AnalogIn bouton(A0);

int controleVentilateur(int entree, Servo * moteurCible) //entree = 0 => moteur éteint, allume sinon
{
    (entree == 0) ? (*moteurCible = 1.0) : (*moteurCible = 0.0);
    return 0;
}

int main()
{
    moteur = 0.0;
    wait(2);
    
    while(1)
    {
    (bouton == 0) ? (controleVentilateur(0, &moteur)) : (controleVentilateur(1, &moteur));
    }
    
}
    
