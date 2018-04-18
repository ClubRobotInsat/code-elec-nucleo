#include "mbed.h"
Ticker ticker;
DigitalOut led1(LED1);
DigitalOut led2(LED2);

CAN can1(PA_11, PA_12,500000);

void afficher_message(CANMessage mess) {
	debug("CAN : %#x (%d) : ",mess.id,mess.len);
	for (uint8_t i = 0; i < mess.len; i++) {
		debug("%#x ",mess.data[i]);
	}
	debug("\n\r");
}

int main() {
	printf("Start \n\r");
	CANMessage msg;
	while(1) {
		if(can1.read(msg)) {
			afficher_message(msg);
		} else {
			printf("Erreur de lecture : %d\n\r", can1.rderror());
		}
		wait(0.2);
	}
}
