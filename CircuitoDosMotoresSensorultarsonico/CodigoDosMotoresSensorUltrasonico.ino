
/*
  Proyecto: Control de dos motores DC con sensor ultrasónico HC-SR04
  y buzzer para advertencia durante la reversa.

  Funcionamiento:
  - Los motores avanzan durante 2 segundos.
  - Después, retroceden durante 2 segundos.
  - El sensor mide la distancia continuamente.
  - Durante la reversa, el buzzer emite pitidos si detecta
    un objeto a 350 cm o menos.
  - Los pitidos se vuelven más frecuentes conforme disminuye
    la distancia al objeto.
*/

// Pines del primer motor
int EN1 = 5;  // Habilitación y control de velocidad del motor 1
int IN1 = 8;  // Entrada de dirección 1
int IN2 = 9;  // Entrada de dirección 2

// Pines del segundo motor
int EN2 = 6;   // Habilitación y control de velocidad del motor 2
int IN3 = 10;  // Entrada de dirección 3
int IN4 = 11;  // Entrada de dirección 4

// Pines del sensor ultrasónico HC-SR04
int trigPin = 2;  // Pin que envía el pulso ultrasónico
int echoPin = 3;  // Pin que recibe el eco del pulso

// Pin del buzzer
int buzzer = 13;  // Salida para generar los sonidos de advertencia

// Indica si los motores están configurados para ir en reversa
// false = adelante; true = atrás
bool vaAtras = false;

// Variables para controlar el tiempo
unsigned long tiempoFase = 0;   // Momento del último cambio de dirección
unsigned long tiempoBuzzer = 0; // Momento del último pitido
unsigned long duracion;         // Duración del pulso de eco en microsegundos

// Variables para medir la distancia y controlar los pitidos
unsigned int distancia;  // Distancia calculada en centímetros
int intervalo;           // Intervalo entre pitidos en milisegundos


// ---------- CONFIGURACIÓN INICIAL ----------
void setup() {
  // Configura los pines del controlador de motores como salidas
  pinMode(EN1, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(EN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Configura los pines del sensor ultrasónico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Configura el buzzer como salida
  pinMode(buzzer, OUTPUT);

  // Inicia el funcionamiento con los motores hacia adelante
  moverAdelante();

  // Guarda el tiempo inicial para controlar las fases
  tiempoFase = millis();
}


// ---------- CICLO PRINCIPAL ----------
void loop() {
  // Comprueba si han transcurrido al menos 2000 ms (2 segundos)
  // desde el último cambio de dirección
  if (millis() - tiempoFase >= 2000) {
    // Actualiza el momento del último cambio de dirección
    tiempoFase = millis();

    // Alterna entre avanzar y retroceder
    if (vaAtras) {
      moverAdelante();  // Si iba atrás, ahora avanza
    } else {
      moverAtras();     // Si iba adelante, ahora retrocede
    }
  }

  // Actualiza la medición del sensor y el funcionamiento del buzzer
  // en cada iteración del ciclo principal
  actualizarBuzzer();
}


// ---------- MOVIMIENTO HACIA ADELANTE ----------
void moverAdelante() {
  // Configura el sentido de giro de ambos motores
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Establece la velocidad mediante señales PWM
  // El valor 120 corresponde a un nivel PWM de 120 sobre 255
  analogWrite(EN1, 120);
  analogWrite(EN2, 120);

  // Actualiza el estado del movimiento
  vaAtras = false;
}


// ---------- MOVIMIENTO EN REVERSA ----------
void moverAtras() {
  // Invierte las señales de dirección de ambos motores
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Mantiene el mismo nivel PWM que durante el avance
  analogWrite(EN1, 120);
  analogWrite(EN2, 120);

  // Indica que los motores están configurados para retroceder
  vaAtras = true;
}


// ---------- ACTUALIZACIÓN DEL BUZZER ----------
void actualizarBuzzer() {
  // Obtiene una nueva medición de distancia
  distancia = medirDistancia();

  // Apaga el buzzer si:
  // 1. Los motores no están en reversa.
  // 2. La medición no es válida (distancia igual a cero).
  // 3. El objeto está a más de 350 cm.
  if (!vaAtras || distancia == 0 || distancia > 350) {
    noTone(buzzer);
    return;  // Finaliza esta función sin generar un pitido
  }

  // Si se cumplen las condiciones, calcula el intervalo
  // entre pitidos según la distancia detectada
  intervaloBuzzer(distancia);
}


// ---------- INTERVALO ENTRE PITIDOS ----------
void intervaloBuzzer(int cm) {
  // Limita la distancia al rango de 5 a 350 cm y establece
  // una relación lineal entre distancia e intervalo:
  // 5 cm o menos = 100 ms entre pitidos
  // 350 cm = 800 ms entre pitidos
  int intervalo = map(constrain(cm, 5, 350),
                      5, 350, 100, 800);

  // Comprueba si ya transcurrió el intervalo calculado
  // desde el último pitido
  if (millis() - tiempoBuzzer >= (unsigned long)intervalo) {
    // Genera un sonido de 500 Hz durante 50 milisegundos
    tone(buzzer, 500, 50);

    // Registra el momento en que se generó el pitido
    tiempoBuzzer = millis();
  }
}


// ---------- MEDICIÓN DE DISTANCIA ----------
unsigned int medirDistancia() {
  // Mantiene TRIG en LOW durante 2 microsegundos
  // para preparar el envío del pulso
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Envía un pulso HIGH de 10 microsegundos
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  // Finaliza el pulso de activación del sensor
  digitalWrite(trigPin, LOW);

  // Mide cuánto tiempo permanece ECHO en HIGH.
  // El tiempo máximo de espera es de 25000 microsegundos (25 ms)
  duracion = pulseIn(echoPin, HIGH, 25000);

  // Si no se recibe un eco, devuelve cero para indicar
  // que la medición no es válida
  if (duracion == 0) {
    return 0;
  }

  // Calcula la distancia en centímetros:
  // duración × velocidad del sonido / 2.
  // Se divide entre 2 porque el sonido recorre la distancia
  // de ida hasta el objeto y de regreso al sensor.
  return (unsigned int)(duracion * 0.0343 / 2);
}
