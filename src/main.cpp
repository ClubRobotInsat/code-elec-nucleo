#include "mbed.h"
#include "herkulex.h"
 
// set serial port and baudrate, (mbed <-> HerculexX)
uint8_t id = 0x03; 
Serial pc(USBTX, USBRX, 9600); 

Serial txd(D8, NC);
Serial rxd(NC, D2);



int main() 
{  
    pc.printf("lol\n");

    Herkulex sv(&txd, &rxd, &pc);//, pc);

    pc.printf("lol ?\n");

    sv.clear(0x03);
    
//    sv.setTorque(0x03, TORQUE_ON);
    
    sv.getStatus(0x03);

    while (1) {
	    sleep();
    }
    /*
    sv.getPos(0x03);
   
    while(1) 
    {   
        // POS Mode, +159.8 Degree, 100*11.2ms = 1120ms, Green LED On
        sv.positionControl(0x03, 1002, 100, GLED_ON);
        wait(5);
        
        // POS Mode, -159.8 Degree, 100*11.2ms = 1120ms, Blue LED On
        sv.positionControl(0x03, 21, 100, BLED_ON);
        wait(5);
 
        // TURN Mode, CCW Turn, Green LED On
        sv.velocityControl(0x03, 1023, GLED_ON);
        wait(5);
        
        // TURN Mode, CW Turn, Blue LED On        
        sv.velocityControl(0x03, -1023, BLED_ON);
        wait(5);
        
        // POS Mode, 0 Degree, 100*11.2ms = 1120ms, Red LED On
        sv.positionControl(0x03, 512, 100, RLED_ON);
        wait(5);
        
        // Clear error status
        sv.clear(0x03);
    }
    */
}
