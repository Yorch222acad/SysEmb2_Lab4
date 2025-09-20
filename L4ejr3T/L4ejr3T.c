#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

// Variable global para la frecuencia del sistema
uint32_t g_ui32SysClock;
uint8_t cont = 0;
uint8_t button = 0;
uint32_t FS = 120000000;  // 1 segundo de interrupción, cambiar luego para 2 y 5 segundos

// Función de interrupción del Timer0
void mitimerA0(void)
{
    // Limpiar la interrupción del temporizador
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if(GPIOPinRead(GPIO_PORTJ_BASE, 0x01) == 0)
    {
        button++;
    }
    else if(button == 1)
    {
        FS = 120000000;
        TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
    }
    else if(button == 2)
    {
        FS = 120000000*0.5;
        TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
    }
    else
    {
        button=0;
    }


    // Encender el siguiente LED según el contador
    if (cont == 0)
    {
        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);  // Enciende PN1
    }
    else if (cont == 1)
    {
        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);  // Enciende PN1
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);  // Enciende PN0
    }
    else if (cont == 2)
    {
        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);  // Enciende PN1
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);  // Enciende PN0
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);  // Enciende PF4
    }
    else if (cont == 3)
    {
        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);  // Enciende PN1
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);  // Enciende PN0
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);  // Enciende PF4
        GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x01);  // Enciende PF0  
    }
    else
    {
        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x00);  // Enciende PN1
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x00);  // Enciende PN0
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x00);  // Enciende PF4
        GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x00);  // Enciende PF0
    }

    // Incrementar el contador y reiniciar después de PF0 (después de 3)
   cont++;
    if (cont >3) {
        
        cont = -1;  // Reiniciar la secuencia después de PF0
    }
    
    if(GPIOPinRead(GPIO_PORTJ_BASE, 0x01) == 0)
    {
        button++;
    }
    else if(button == 1)
    {
        FS = 120000000;
        TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
    }
    else if(button == 2)
    {
        FS = 120000000*0.5;
        TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
    }
    else
    {
        button=0;
    }
    // Recargar el temporizador para la siguiente interrupción (1 segundo)
    TimerLoadSet(TIMER0_BASE, TIMER_A, FS - 1);
}


int main(void)
{
    // Configuración del reloj del sistema a 120 MHz
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_240), 120000000);

    // Habilitamos los puertos
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);  // Configurar PN0 y PN1 como salida para el LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);  // Configurar PF4 y PF0 como salida para el LED
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, 0x01); 
    // Habilitar el periférico del temporizador 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configurar el temporizador 0 como periódico
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, FS - 1);  // Cargar el valor del temporizador (1 segundo inicialmente)

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
        // El trabajo se maneja en la interrupción, no se hace nada aquí
    }
}


