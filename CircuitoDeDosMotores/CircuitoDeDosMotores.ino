// Motor delantero
// Se definen los pines utilizados para controlar el motor delantero.
// EN1 controla la velocidad mediante PWM.
// IN1 e IN2 controlan la dirección de giro.
int EN1 = 5;
int IN1 = 8;
int IN2 = 9;

// Motor trasero
// Se definen los pines utilizados para controlar el motor trasero.
// EN2 controla la velocidad mediante PWM.
// IN3 e IN4 controlan la dirección de giro.
int EN2 = 6;
int IN3 = 10;
int IN4 = 11;

void setup() {

  // Se configuran todos los pines utilizados como salidas.
  pinMode(EN1, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {

  // AVANZAR
  // Se establece la dirección de giro de ambos motores para avanzar.
  // Ambos motores reciben la misma configuración de dirección.
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Se asigna un valor PWM de 120 a ambos motores.
  // Esto representa aproximadamente un 47% de duty cycle.
  analogWrite(EN1, 120);
  analogWrite(EN2, 120);

  // Los motores permanecen avanzando durante 2 segundos.
  delay(2000);

  // ESPERA
  // Se establece el PWM en 0, deteniendo ambos motores.
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);

  // Permanecen detenidos durante 1 segundo.
  delay(1000);


  // RETROCEDER
  // Se invierte la configuración de las entradas para cambiar
  // la dirección de giro de ambos motores.
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Ambos motores vuelven a utilizar un PWM de 120,
  // manteniendo aproximadamente el mismo duty cycle.
  analogWrite(EN1, 120);
  analogWrite(EN2, 120);

  // Los motores permanecen retrocediendo durante 2 segundos.
  delay(2000);

  // ESPERA
  // Se detienen nuevamente ambos motores.
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);

  // Permanecen detenidos durante 1 segundo.
  delay(1000);


  // DIRECCIÓN OPUESTA CON DIFERENTE VELOCIDAD
  // Se configura cada motor para girar en una dirección diferente.
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Se asignan diferentes valores PWM a cada motor.
  // El motor delantero recibe 100 (~39% de duty cycle).
  // El motor trasero recibe 120 (~47% de duty cycle).
  analogWrite(EN1, 100);
  analogWrite(EN2, 120);

  // Ambos motores mantienen estas condiciones durante 2 segundos.
  delay(2000);


  // ESPERA
  // Se detienen ambos motores al establecer el PWM en 0.
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);

  // Permanecen detenidos durante 1 segundo.
  delay(1000);
}