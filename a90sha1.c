#include "xparameters.h"
#include "xgpio.h"
#include "xscutimer.h"
#include <sleep.h>
#include <xil_cache.h>
#include "ZedboardOLED.h"
#include <stdio.h>
#include "sha1.h"

XScuTimer Timer;		/* Cortex A9 SCU Private Timer Instance */
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
int main (void)
{
   unsigned char tmpbuf[16];
   unsigned char encrypted[SHA1_BLOCK_SIZE];
   unsigned int  Status, newvalue, this;
   unsigned int * a0 		= (unsigned int *) 		0x1BF00000;
   unsigned int * a1 		= (unsigned int *) 		0x1BF00008;
   unsigned char *buffer0 	= ( unsigned char *)	0x1BF01000;
   unsigned char *buffer1 	= ( unsigned char *)  	0x1BF01030;
   *a0 = 0;
   //Xil_DCacheFlush();

   Xil_DCacheDisable();
   Xil_ICacheDisable();

   xil_printf("-- Start of the Program --\r\n");
   xil_printf("a0 : %d a1: %d \r\n", *a0, *a1);
   //xil_printf("%x a  %x b \r\n", a0, a1);
   //usleep(400);

   // PS Timer related definitions
   XScuTimer_Config *ConfigPtr;
   XScuTimer *TimerInstancePtr = &Timer;
   // Initialize the timer
   ConfigPtr = XScuTimer_LookupConfig (XPAR_PS7_SCUTIMER_0_DEVICE_ID);
   Status = XScuTimer_CfgInitialize	(TimerInstancePtr, ConfigPtr, ConfigPtr->BaseAddr);

   if(Status != XST_SUCCESS){
	   xil_printf("Timer init() failed\r\n");
	   return XST_FAILURE;
   }

   XScuTimer_LoadTimer(TimerInstancePtr, 4000000000);
   // Set AutoLoad mode
   XScuTimer_EnableAutoReload(TimerInstancePtr);

   xil_printf("Enter your password please: ");
   char userinput[16];
   scanf("%s",userinput);

   SHA1_CTX ctx;

   sha1_init(&ctx);
   sha1_update(&ctx, userinput, 16);
   sha1_final(&ctx, encrypted);

   writeBuf( buffer0, encrypted,SHA1_BLOCK_SIZE);
   //writeBuf( buffer0, "0123456789abcdef",16);
   xil_printf("-- Written buffer0 \r\n");
   readBuf(tmpbuf, encrypted, SHA1_BLOCK_SIZE);
   //readBuf(tmpbuf, buffer0, 16);
   printBuf(tmpbuf,SHA1_BLOCK_SIZE);
   *a0 = 1; //changing the value of shared variable.
   //Xil_DCacheFlush();

   // Start the timer
   XScuTimer_Start (TimerInstancePtr);

   while( (*a1 != 1) ) {
	 //  Xil_DCacheInvalidate();
	   usleep(500);
   }
   XScuTimer_Stop(TimerInstancePtr);
   newvalue = XScuTimer_GetCounterValue(TimerInstancePtr);
   this = 4000000000 - newvalue;
   xil_printf("Cycles counted: %d for reply.\r\n", this);
   *a1 = 0;
   *a0 = 0;
   //Xil_DCacheFlush();

   //Xil_DCacheInvalidate();
   readBuf(tmpbuf, buffer1, 8);
   xil_printf("Status : \r\n");
   printBuf(tmpbuf, 8);

   return 0;


}

