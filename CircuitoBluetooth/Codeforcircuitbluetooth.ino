#include <SoftwareSerial.h> // Incluye la librería para establecer comunicación serial mediante otros pines digitales.

// Configura los pines 10 y 11 como RX y TX, respectivamente.
SoftwareSerial BTSerial(10, 11);

void setup() {

  // Configura el pin 13 como salida para controlar el LED.
  pinMode(13, OUTPUT);

  // Inicia la comunicación serial entre el Arduino y la computadora a 9600 baudios.
  Serial.begin(9600);

  // Inicia la comunicación entre el Arduino y el módulo Bluetooth a 9600 baudios.
  BTSerial.begin(9600);

}

void loop() {

  // Comprueba continuamente si existen datos disponibles provenientes del módulo Bluetooth.
  while (BTSerial.available() > 0) {

    // Lee el carácter recibido mediante Bluetooth y lo almacena en una variable.
    char receivedChar = BTSerial.read();

    // Muestra en el monitor serial el carácter que fue recibido.
    Serial.print("Mensaje recibido: ");
    Serial.println(receivedChar);

    // Comprueba si el carácter recibido es '1'.
    if (receivedChar == '1') {

      // Indica en el monitor serial que se recibió el comando para encender el LED.
      Serial.println("Se recibio comando de encendido");

      // Enciende el LED conectado al pin 13.
      digitalWrite(13, HIGH);

      // Envía una confirmación de encendido a través del módulo Bluetooth.
      BTSerial.println("Se encendio el LED");

    // Comprueba si el carácter recibido es '0'.
    } else if (receivedChar == '0') {

      // Indica en el monitor serial que se recibió el comando para apagar el LED.
      Serial.println("Se recibio comando de apagado");

      // Apaga el LED conectado al pin 13.
      digitalWrite(13, LOW);

      // Envía una confirmación de apagado a través del módulo Bluetooth.
      BTSerial.println("Se apago el LED");

    // Si el carácter no es '1' ni '0', se considera un comando inválido.
    } else {

      // Indica en el monitor serial que se recibió un comando no reconocido.
      Serial.println("Se recibio comando invalido");

      // Envía un mensaje de error a través del módulo Bluetooth.
      BTSerial.println("No hubo accion, comando invalido");
    }
  }
}