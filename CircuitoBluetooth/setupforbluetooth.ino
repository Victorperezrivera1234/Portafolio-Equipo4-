#include <SoftwareSerial.h> // Incluye la librería para establecer comunicación serial mediante otros pines digitales.

// Configura los pines 10 y 11 como RX y TX, respectivamente.
SoftwareSerial BT(10, 11);

void setup() {

  // Inicia la comunicación serial entre el Arduino y la computadora a 9600 baudios.
  Serial.begin(9600);

  // Inicia la comunicación serial entre el Arduino y el módulo Bluetooth HC-05 a 38400 baudios.
  // Esta velocidad se utiliza para la configuración del módulo mediante comandos AT.
  BT.begin(38400);

  // Muestra un mensaje en el monitor serial indicando que se pueden enviar comandos AT.
  Serial.println("Enviar comandos AT: ");

}

void loop() {

  // Comprueba si existen datos disponibles provenientes de la computadora.
  if (Serial.available())

    // Lee un carácter del monitor serial y lo envía al módulo Bluetooth.
    BT.write(Serial.read());

  // Comprueba si el módulo Bluetooth tiene datos disponibles para enviar.
  if (BT.available())

    // Lee el carácter recibido por Bluetooth y lo muestra en el monitor serial.
    Serial.write(BT.read());

}