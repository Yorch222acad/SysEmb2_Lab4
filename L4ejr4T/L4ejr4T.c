#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

void CntBin(int freq, int *binAnt, int l[], float *Mfreq);
void LecBtn(int freq, float *Mfreq);
void SumBin(int *binAnt, int l[]); 
void EstbLed (int l[]);
void mitimerA0 (void);

// Variable global para la frecuencia del sistema
volatile int sen = 1; // 1 para contar hacia arriba, 0 para contar hacia abajo
volatile int cnt = 0;

int main(void)
{
  int freq = 120000000;
  int binAnt = 0;
  int l[4] = {0};
  float Mfreq = 1.5;

    // Configuración del reloj del sistema a 120 MHz
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),freq); 

    // Habilitar el puerto GPIO para el LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03); // Enable pin 0 and 1
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11); // Enable pin 0 and 1
    //--------------------------------------------------------------
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, 0x01);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, 0x01, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    //--------------------------------------------------------------
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configurar el temporizador 0 como periódico
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (uint32_t)(freq * Mfreq) - 1);  // Cargar el valor del temporizador (1 segundo inicialmente)

    // Habilitar las interrupciones del procesador
    IntMasterEnable();

    // Habilitar la interrupción de Timer0A
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Habilitar el temporizador 0
    TimerEnable(TIMER0_BASE, TIMER_A);

    // Bucle principal vacío (el trabajo se hace en la interrupción)
    while(1)
    {
       CntBin(freq, &binAnt, l, &Mfreq);
    }
}
//========================================================================= Funciones ##

//========================================================================= Funciones ##

void CntBin(int freq, int *binAnt, int l[], float *Mfreq) {
  LecBtn(freq, Mfreq);
  SumBin(binAnt, l);
  EstbLed (l);  
}
//------------------------------------------------------------------------
void LecBtn(int freq, float *Mfreq) {
  volatile uint32_t ui32Loop = 0;
  if ((GPIOPinRead(GPIO_PORTJ_BASE, 0x01) == 0)) {
    if (*Mfreq == 1.5){
      *Mfreq = 3.0;
    }
    else{
      *Mfreq = 1.5;
    }
    TimerLoadSet(TIMER0_BASE, TIMER_A, (uint32_t)(freq * (*Mfreq)) - 1);
    for (ui32Loop = 0; ui32Loop < (freq/40); ui32Loop++) {}
  }
}
//------------------------------------------------------------------------
void SumBin(int *binAnt, int l[]) {
  int i, j;
  if (*binAnt != cnt) {
    for (i = 0; i < 4; i++) {
      l[i] = 0;
    }
    for (i = 0; i < cnt; i++) {
      for (j = 3; j >= 0; j--) {
        if (l[j] == 0) {
          l[j] = 1;  // pongo un 1 donde encuentro el primer 0
            break;     // salgo del for
        } else {
          l[j] = 0;  // si era 1, pongo 0 y sigo el acarreo
        }
      }
    }
    *binAnt = cnt;  // actualizar variable real
  }
}
//------------------------------------------------------------------------
void EstbLed (int l[]){
  if (l[0]==1){
    GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);
  }
  else {
    GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0);
  } 
  if (l[1]==1){
    GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);
  }
  else {
    GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0);
  }         
  if (l[2]==1){
    GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);
  }
  else {
    GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0);
  }  
  if (l[3]==1){
    GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x01);
  }
  else {
    GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0);
  }  
}
//------------------------------------------------------------------------
void mitimerA0 (void){
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if (cnt==15) {
      sen=0;
    }
    else if (cnt==0) {
      sen=1;
    }
    //---------------
    if (sen==1) {
      cnt++;
    }
    else if (sen==0) {
      cnt--;
    }
}