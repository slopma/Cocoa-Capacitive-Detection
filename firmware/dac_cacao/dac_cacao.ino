#include <DacESP32.h>
#include "esp_timer.h"
#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include <math.h>

// ----- DAC (pin 26) -----
#define DAC_PIN 26 // DAC channel 1 -> GPIO26
#define FREQUENCY 500000.0 // Frecuencia de onda seno
#define AMPLITUDE 48.0       // Pico de la onda   
#define SAMPLE_RATE 200000.0    // Muestreo (más alto = mayor resolución)

dac_oneshot_handle_t dacHandle; // Manejador del canal DAC

// ----- ADC -----
Adafruit_ADS1115 ads;  // Objeto para interactuar con el ADC
int16_t adc0;  // Variable para almacenar el valor del ADC
float volts0;   // Variable para almacenar el valor en voltios

// ----- Variables Globales -----
static int i = 0;  // Contador para el ciclo de la onda senoidal
const unsigned int SAMPLES_PER_CYCLE = (SAMPLE_RATE / FREQUENCY); // Cantidad de muestras por ciclo de la onda
unsigned long timerDAC = 0, timerADC = 0; // Temporizadores para DAC y ADC
DacESP32 dac2(GPIO_NUM_26);

// ----- Task Handles -----
TaskHandle_t dacTaskHandle; // Manejador de la tarea DAC
TaskHandle_t adcTaskHandle; // Manejador de la tarea ADC

// ----- DAC Task -----
void taskGenerateDAC(void *pvParameters) {
  // Configuración DAC
  dac2.outputCW(1000);

  // Ejecución del DAC
  while (true) {

    vTaskDelay(10);  // Permite que el ESP32 gestione otras tareas
  }
}

// ----- ADC Task -----
void taskReadADC(void *pvParameters) {
  // Configuración ADC
  if (!ads.begin()) {  // Inicializa el ADC
    Serial.println("Error: No se pudo iniciar ADS1115.");
    while (1); // Detiene la ejecución si no se inicia correctamente
  }

  // Ejecución del ADC
  while (true) {
    //   // Leer ADC
      adc0 = ads.readADC_SingleEnded(0);  // Lee el valor del canal 0 del ADC
      volts0 = ads.computeVolts(adc0);    // Convierte el valor del ADC a voltios

      // Mostrar valores
      Serial.print("DAC value: "); Serial.print(128 + AMPLITUDE * sin(2 * PI * FREQUENCY * i / SAMPLE_RATE)); // Imprime valor DAC
      Serial.print(" | ADC value: "); Serial.print(adc0); // Imprime valor ADC
      Serial.print(" | Voltage: "); Serial.print(volts0, 4); Serial.println(" V"); // Imprime el voltaje en formato de 4 decimales

    // }
    vTaskDelay(200 / portTICK_PERIOD_MS);  // Permite que el ESP32 gestione otras tareas
  }
}

void setup() {
  Serial.begin(115200);  // Inicia comunicación serial

  Serial.println("Sistema DAC-ADC iniciado con multitarea.");  // Mensaje de inicio

  // Creación de tareas
  // Tarea para la generación del DAC (Core 0)
  xTaskCreatePinnedToCore(
    taskGenerateDAC,   // Subrutina de la tarea
    "Tarea DAC",       // Nombre de la tarea
    4096,              // Tamaño del stack
    NULL,              // Parámetros
    1,                 // Prioridad
    &dacTaskHandle,    // Manejador de la tarea
    0                  // Núcleo 0
  );

  // Tarea para la lectura del ADC (Core 1)
  xTaskCreatePinnedToCore(
    taskReadADC,       // Subrutina de la tarea
    "Tarea ADC",       // Nombre de la tarea
    4096,              // Tamaño del stack
    NULL,              // Parámetros
    1,                 // Prioridad
    &adcTaskHandle,    // Manejador de la tarea
    1                  // Núcleo 1
  );
}

void loop() {
  // Nada aquí, ya todo se gestiona en las tareas
}
