// #include "mbed.h"
// #include "herkulex.h"

// Serial pc(USBTX, USBRX);

// DigitalOut debug_led(D4);

// uint8_t id = 0xfd; 

// void blink() {
//         debug_led = 1;
//         wait(0.2);
//         debug_led = 0;
//         wait(0.2);
// }

// void longblink() {
//         debug_led = 1;
//         wait(1);
//         debug_led = 0;
//         wait(1);
// }

// int main() {
//     debug_led = 0;
//     longblink();
//     pc.printf("Init");
//     //Herkulex sv(D8, D2, 115200);
//     //wait_ms(500);
//     pc.printf("Start");
//     while(1) {
//         for(id = 0; id < 0xfe; id++) 
//         {
//             //printf("Status %s",sv.getStatus(id));
//             //sv.positionControl(id, 400, 100,BLED_ON); 
//             blink();
//             //wait_ms(10);
//             pc.printf("A attendu");
//             longblink();
//             //printf("Status %s",sv.getStatus(id));
//             //sv.positionControl(id, 600, 100,GLED_ON); 
//             //wait_ms(10); 
//             //sv.clear(id); 
//         }
//     }
// }
// /*
// #include "mbed.h"

// DigitalOut debug_led(D4);
// Serial pc(USBTX, USBRX);

// int main() {
//     while(1) {
//         pc.printf("COUCOU");
//         debug_led = 1;
//         wait(0.2);
//         debug_led = 0;
//         wait(0.2);
//     }
// }
// */