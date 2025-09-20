import RPi.GPIO as GPIO
import time

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

# Pines
Btn1 = 8
Btn2 = 7
Btn3 = 1   # Seleccionar laboratorio
#-----------------------------------
Led1 = 0
Led2 = 5
Led3 = 6
Led4 = 13
#-----------------------------------
PinsLed = [Led1, Led2, Led3, Led4]
PinsBtn = [Btn1, Btn2, Btn3]
Buzz = 21

# Configuración pines
for pin in PinsBtn:
    GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
for pin in PinsLed:
    GPIO.setup(pin, GPIO.OUT)
GPIO.setup(Buzz, GPIO.OUT)

# Variables globales
last_states = {Btn1: 1, Btn2: 1, Btn3: 1}
estado_led = 0
opcion = 1   # valor por defecto


# ==================== Funciones botones ====================
def leer_botones():
    global opcion

    # === BOTON 1 ===
    if GPIO.input(Btn1) == GPIO.LOW and last_states[Btn1] == 1:
        if opcion == 1:
               while GPIO.input(Btn1) == GPIO.LOW:
              		GPIO.output(Buzz, GPIO.HIGH)
        elif opcion == 2:
            GPIO.output(Buzz, GPIO.HIGH)

    last_states[Btn1] = GPIO.input(Btn1)

    # === BOTON 2 ===
    if GPIO.input(Btn2) == GPIO.LOW and last_states[Btn2] == 1:
        if opcion == 2:
            GPIO.output(Buzz, GPIO.LOW)
    last_states[Btn2] = GPIO.input(Btn2)

    # === BOTON 3 ===
    if GPIO.input(Btn3) == GPIO.LOW and last_states[Btn3] == 1:
        opcion = int(input("Ingrese el laboratorio que quiere ejecutar (1-3): "))
        print(f"Seleccionaste laboratorio {opcion}")
    last_states[Btn3] = GPIO.input(Btn3)


# ==================== Funciones para cada laboratorio ====================

def labo1():
    GPIO.output(Buzz, GPIO.LOW)

def labo2():
    leer_botones()
    time.sleep(0.1)

def labo3():
    global estado_led

    # Apagar todos los LEDs antes de encender el siguiente
    for pin in PinsLed:
        GPIO.output(pin, GPIO.LOW)

    if estado_led == 0:
        GPIO.output(Led1, GPIO.HIGH)
    elif estado_led == 1:
        GPIO.output(Led2, GPIO.HIGH)
    elif estado_led == 2:
        GPIO.output(Led3, GPIO.HIGH)
    elif estado_led == 3:
        GPIO.output(Led4, GPIO.HIGH)

    estado_led = (estado_led + 1) % 4   # ciclo de 4 LEDs


# ==================== MAIN ====================
labos = {1: labo1, 2: labo2, 3: labo3}
opcion = int(input("Ingrese el laboratorio que quiere ejecutar (1-3): "))

try:
    while True:
        leer_botones()
        if opcion in labos:
            labos[opcion]()  # ejecuta el laboratorio seleccionado
        time.sleep(0.2)
except KeyboardInterrupt:
    pass
finally:
    GPIO.cleanup()
