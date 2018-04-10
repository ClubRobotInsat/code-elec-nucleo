#include "mbed.h"
Ticker ticker;
DigitalOut led1(LED1);
DigitalOut led2(LED2);

CAN can1(PA_11, PA_12,100000);
//CAN can2(PB_5, PB_13,100000);
char counter = 0;
/*
void send() {
	printf("send()\n\r");
	if(can1.write(CANMessage(1337, &counter, 1))) {
		printf("wloop()\n\r");
		counter++;
		printf("Message sent: %d\n\r", counter);
	} 
	else {
		printf("Erreur d'envoi : %d", can1.tderror());
	}
	led1 = !led1;
}
*/

int main() {
	printf("main()---------------------------\n\r");
	//ticker.attach(&send, 1);
	CANMessage msg;
	while(1) {
		printf("loop()\n\r");
		
		if(can1.read(msg)) {
			if (msg.len == 1) {
			printf("Message received: %d\n\r", msg.data[0]);
			}
			else {
				printf("slt : %d \n\r", msg.len);
			}
			led2 = !led2;
		} else {
			printf("Erreur de lecture : %d", can1.rderror());
		}
		
		wait(0.2);
	}
}
