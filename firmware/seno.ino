#include <driver/dac_oneshot.h> 
#include "esp_timer.h"

// Definiciones y constantes
const int dacPin = 25; // Pin del DAC (GPIO 25)
float frequency = 5000.0; // Frecuencia de salida deseada en Hz (1 kHz)
int amplitude = 48; // Amplitud máxima para una onda seno (0-255, donde 128 es 0V)
int sampleRate = 200000; // Frecuencia de muestreo en Hz (8000 Hz para una buena resolución)

dac_oneshot_handle_t dacHandle;

void setup() {
  // Configuración del DAC sin especificar el reloj
  dac_oneshot_config_t dacConfig = {
    .chan_id = DAC_CHAN_0, // Canal 0 (equivalente a GPIO 25)
  };

  // Creación del canal de salida de DAC
  dac_oneshot_new_channel(&dacConfig, &dacHandle);

  Serial.begin(230400);
  Serial.print("Frecuencia de salida: ");
  Serial.print(frequency);
  Serial.println(" Hz");
  Serial.print("Amplitud de salida: ");
  Serial.print(amplitude);
  Serial.println(" (0-255)");
  Serial.print("Frecuencia de muestreo: ");
  Serial.print(sampleRate);
  Serial.println(" Hz");
}

void loop() {
  // Generación de una señal senoidal
  for (int i = 0; i < sampleRate / frequency; i++) {
    // Cálculo del valor de la onda seno
    int value = amplitude + (amplitude * sin(2 * PI * frequency * i / sampleRate));
    
    // Generar salida en el DAC
    dac_oneshot_output_voltage(dacHandle, value); // Enviar el valor calculado al DAC
    delayMicroseconds(1000000 / sampleRate); // Espera para mantener la frecuencia de muestreo
  }
}
