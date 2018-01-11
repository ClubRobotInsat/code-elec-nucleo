#include "mbed.h"
#include "herkulex.h"
 
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0x03; 
Serial pc(USBTX, USBRX, 9600); 

Serial connection(D8,D2, 115200);

int main() 
{  
    pc.printf("lol\n");

    Herkulex sv(&connection, &pc);//, pc);

    pc.printf("lol ?\n");

    sv.clear(0x03);
    sv.setTorque(0x03, TORQUE_ON);

    sv.clear(0xfd); 
    sv.setTorque(0xfd, TORQUE_ON); 

    while(true)
    {
        // sv.setLedOn(0x03); 
        // wait_us(500); 

        sv.positionControl(0x03, 800, 60, GLED_ON);
        sv.positionControl(0xfd, 300, 60, BLED_ON); 
        wait_ms(700);
        
        sv.positionControl(0x03, 300, 60, GLED_ON);
        sv.positionControl(0xfd, 800, 60, BLED_ON); 
        wait_ms(700);
    }
}
