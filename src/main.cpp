#include "mbed.h"
#include "herkulex.h"
 
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0x03; 
Serial pc(USBTX, USBRX, 9600); 

Serial connection(D8,D2, 115200);

int main() 
{  
    pc.printf("lol\n");

    Herkulex sv3(0x03,&connection, &pc);
    Herkulex svfd(0xfd,&connection, &pc);

    pc.printf("lol ?\n");

    sv3.clear();
    sv3.setTorque(TORQUE_ON);

    svfd.clear(); 
    svfd.setTorque(TORQUE_ON); 

    while(true)
    {
        // sv.setLedOn(0x03); 
        // wait_us(500); 

        sv3.positionControl(800, 60, GLED_ON);
        svfd.positionControl(300, 60, BLED_ON); 
        wait_ms(700);
        
        sv3.positionControl(300, 60, GLED_ON);
        svfd.positionControl(800, 60, BLED_ON); 
        wait_ms(700);
    }
}
