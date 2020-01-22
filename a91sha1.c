
#include "xparameters.h"
#include "xgpio.h"
#include <sleep.h>
#include "xscutimer.h"
#include <xil_cache.h>
#include "ZedboardOLED.h"
#include <string.h>
#include <stdio.h>
#include "sha1.h"
#define		ACCEPT 		"accepted"
#define		DECLINE		"declined"
XScuTimer Timer;		/* Cortex A9 SCU Private Timer Instance */
#define USERS 1024


unsigned char passwords[USERS][16];
unsigned char encrypted[USERS][SHA1_BLOCK_SIZE]; // storing the sha1 of each password.

void fill_encrypted(void)
{

	xil_printf("i am here1\n");
	for(int i = 0; i < USERS; i++) {

		SHA1_CTX ctx;
		 sha1_init(&ctx);
		 sha1_update(&ctx, passwords[i], 16);
		 sha1_final(&ctx, encrypted[i]);
		 /*
		 xil_printf("SHA1[");

		 for( int j = 0; j < 16; j++) {
			 xil_printf("%c",passwords[i][j]);
		 }
		 xil_printf("] = ");

		 for(int w = 0; w < SHA1_BLOCK_SIZE; w++)
			 xil_printf("%4x", encrypted[i][w]);
		 xil_printf("\n");
		 */
	}
	xil_printf("i am here2\n");

}

void writeBuf(unsigned char *buf0, char *s, uint8_t len)
{
	for(int i = 0; i < len; i++)
		buf0[i] = s[i];
	return ;
}
void readBuf(unsigned char *dest, unsigned char *s, uint8_t len)
{
	for(int i=0; i < len; i++)
		dest[i] = s[i];
	return;
}
void printBuf(unsigned char *buf, uint8_t len) {
	for(int i = 0; i < len; i++)
		   xil_printf("%x ", buf[i]);
	   xil_printf("\n");
}
int matches(unsigned char *in) {

	for( int i = 0; i < USERS; i++){

		//xil_printf("Testing: %d\r\n", i);
		if( strncmp( (const char *)in, (const char *) encrypted[i], SHA1_BLOCK_SIZE) == 0)
			return i+1;
	}
	return 0;
}
int main (void)
{
	xil_printf("-- Start of the Program --\r\n");
	char snum[5];
	char tnum[4];

	for(int i = 0; i < USERS; i++) {

		strcpy(passwords[i], "abcdefghabcdefgh");

		memcpy(snum, "00000", 5);
		memcpy(tnum, "0000", 4);
		sprintf(snum, "%4d",i);
		sprintf(tnum, "%d",i);
		int len = strlen(tnum);

		if(len!=4)
			sprintf(passwords[i], "%s", snum);
		else
			sprintf(passwords[i], "%s", tnum);

		if(len == 1) {
			passwords[i][0] = '0';
			passwords[i][1] = '0';
			passwords[i][2] = '0';
		} else if(len == 2) {
			passwords[i][0] = '0';
			passwords[i][1] = '0';
		} else if(len == 3)
			passwords[i][0] = '0';
		else if(len == 4)
			passwords[i][0] = '1';

		passwords[i][4] = 'a';

		//for(int j = 0; j < 16; j++)
			//	xil_printf("%c",passwords[i][j]);

		//	xil_printf("\n\r");
		}
		xil_printf("Passwds generated\n\r");



	fill_encrypted();
	//Xil_DCacheInvalidate();
	Xil_DCacheDisable();
	Xil_ICacheDisable();

	clear_OLED(); // clear the OLED
	print_message("Enter password2:",1);
	unsigned int * a0 		= (unsigned int *) 		0x1BF00000;
	unsigned int * a1 		= (unsigned int *) 		0x1BF00008;
	unsigned char *buffer0 	= ( unsigned char *)	0x1BF01000;
	unsigned char *buffer1 	= ( unsigned char *)  	0x1BF01030;
	char thesi[4];
	*a0 = 0;
	memcpy(thesi, 32, 4); // in a non printable char.
	usleep(1000000);
	xil_printf("-- Start of the Program --\r\n");
	xil_printf("a0 : %d a1: %d \r\n", *a0, *a1);
	//xil_printf("%x a  %x b \r\n", a0, a1);

	unsigned char input[16];

	xil_printf("\nWaiting to read buffer0\r\n");
	//Xil_DCacheInvalidate();
	while( (*a0) != 1) {
		//xil_printf("a0 : %d\r\n", *a0);
		//xil_printf("a1 : %d\r\n", *a1);
		//xil_printf("wait..\r\n");
		//Xil_DCacheInvalidate();
		//Xil_DCacheInvalidate();
		//sleep(1);
	}
	readBuf(input, buffer0 , 20);
	//xil_printf("Read the buffer0 \r\n");
	//printBuf(input, 20);
	int ret = matches(input);

	xil_printf("ret value: %d\r\n",ret);
	clear_OLED();
	if( ret != 0) {

		xil_printf("Accept authorization. \r\n");
		print_message("Access Granted",1);

		sprintf(thesi, "%4d",ret);
		for(int j = 0; j < 4; j++)
			print_char(thesi[j],2,j);
		writeBuf(buffer1, ACCEPT, strlen(ACCEPT));
	}else {
		xil_printf("Decline authorization. \r\n");
		print_message("Access Declined",1);
		writeBuf(buffer1, DECLINE, strlen(DECLINE));
	}
	*a1 = 1;
	//Xil_DCacheFlush();
   return 0 ;
}

