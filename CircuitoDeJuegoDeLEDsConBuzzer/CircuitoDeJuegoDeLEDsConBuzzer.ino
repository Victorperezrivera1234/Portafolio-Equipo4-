int leds[] = {13, 12, 11}; 
// Se definen los diferentes pines que van a ser utilizados para alimentar a los LEDs.

int botones[] = {8, 7, 6}; 
// En este arreglo se definen los pines que se van a encargar de leer la señal de los botones.

int buzzer = 3;
// Se define el pin que será utilizado para conectar y controlar el buzzer.

int ledActual;
// Esta variable se define para luego darle un valor aleatorio y así determinar cuál LED estará encendido.

int ContadorBoton = 0;
// Esta variable se utiliza para llevar la cuenta de cuántas veces el usuario ha presionado correctamente el botón correspondiente al LED.

unsigned long tiempoInicio = 0;
// Esta variable almacena el momento exacto en el que comienza el temporizador.
// Se utiliza unsigned long porque la función millis() devuelve este tipo de dato.

bool timerActivo = false;
// Esta variable sirve para saber si el temporizador ya está funcionando.
// Cuando es false, el temporizador está apagado.
// Cuando es true, significa que el temporizador está activo.


void setup() {
  Serial.begin(9600);
  // Se inicia la comunicación serial a una velocidad de 9600 baudios.
  // Esto permite mostrar información en el monitor serial, como el contador de aciertos.

  pinMode(buzzer, OUTPUT);
  // Se configura el pin del buzzer como OUTPUT porque Arduino enviará una señal para producir sonido.

  for (int i = 0; i < 3; i++) {
    // Se utiliza esta estructura para poder configurar todos los LEDs del arreglo.
    
    pinMode(leds[i], OUTPUT);
    // Se pone cada pin de los LEDs en función de salida (OUTPUT).
  }

  for (int i = 0; i < 3; i++) {
    // Mismo concepto que en el anterior, pero ahora se recorren los botones.
    
    pinMode(botones[i], INPUT_PULLUP);
    // Los pines de los botones se configuran como entradas utilizando la resistencia interna PULLUP.
    // Por esta configuración, cuando el botón no está presionado se obtiene HIGH
    // y cuando el botón es presionado se obtiene LOW.

  }

  randomSeed(analogRead(A0));
  // La función randomSeed se utiliza para generar una secuencia de números aleatorios diferente
  // cada vez que se ejecuta el programa.
  // Se utiliza una lectura del pin analógico A0 como una semilla aleatoria.

  ledActual = random(0, 3);
  // Se utiliza la variable ledActual para asignarle un valor aleatorio entre 0 y 2.
  // Esto permite seleccionar uno de los tres LEDs al iniciar el programa.

  digitalWrite(leds[ledActual], HIGH);
  // Después de definir el valor de ledActual, se utiliza digitalWrite para encender
  // uno de los LEDs del arreglo en base al valor aleatorio obtenido.
}


void loop() {

  if (timerActivo && millis() - tiempoInicio > 5000) {
    // Esta condición verifica si el temporizador está activo y si han pasado más de 5 segundos
    // desde el momento almacenado en tiempoInicio.
    // millis() permite medir el tiempo que lleva funcionando Arduino sin detener el programa.

    Serial.println("Tiempo agotado. Programa terminado.");
    // Se muestra un mensaje en el Monitor Serial indicando que se terminó el tiempo del juego.

    for (int i = 0; i < 3; i++) {
      // Se recorren todos los LEDs del arreglo para apagarlos.

      digitalWrite(leds[i], LOW);
      // Se apaga cada uno de los LEDs.
    }

    while (true) {
      // Este ciclo infinito detiene el funcionamiento del programa.
      // Al llegar aquí, el juego termina y Arduino deja de ejecutar el resto del código.
    }
  }


  for (int i = 0; i < 3; i++) {
    // Se recorre el arreglo de botones para revisar cuál está siendo presionado.

    if (digitalRead(botones[i]) == LOW) {
      // Se abre una estructura if para leer cuál de los botones está siendo presionado.
      // Debido a INPUT_PULLUP, LOW significa que el botón está presionado.

      if (i == ledActual) {
        // Este if se utiliza para confirmar que el botón presionado sea correspondiente al LED que está encendido.


        tiempoInicio = millis();
        // Se guarda el momento en el que el jugador presionó correctamente el botón.
        // Esto permite comenzar o reiniciar el conteo de los 5 segundos.

        timerActivo = true;
        // Se activa el temporizador para indicar que el juego ya comenzó.


        digitalWrite(leds[ledActual], LOW);
        // En el caso de que el LED y el botón sean correspondientes,
        // se apagará el LED para continuar el juego.


        tone(buzzer, 440);
        // Se activa el buzzer y se genera un tono de 440 Hz para indicar que
        // el jugador presionó correctamente el botón.

        delay(100);
        // Se mantiene el sonido durante 100 milisegundos para que sea perceptible.

        noTone(buzzer);
        // Se detiene el sonido del buzzer.


        ContadorBoton = ContadorBoton + 1;
        // Se aumenta en uno el contador cada vez que el usuario presiona correctamente el botón.
        // Esto permite saber cuántos aciertos ha conseguido el jugador.


        Serial.print("Contador de boton: ");
        // Se imprime en el Monitor Serial el texto que identifica el contador.

        Serial.println(ContadorBoton);
        // Se muestra el número actual de aciertos y se salta a la siguiente línea.


        int nuevoLED;
        // Se define una nueva variable que se utilizará para seleccionar aleatoriamente
        // el siguiente LED que deberá presionar el usuario.


        do {
          // Esta estructura lógica se ejecuta obligatoriamente al menos una vez.
          // Después se repetirá mientras se cumpla la condición del while.

          nuevoLED = random(0, 3);
          // Se asigna un valor aleatorio entre 0 y 2 a nuevoLED.
          // Cada número representa una posición diferente dentro del arreglo de LEDs.

        } while (nuevoLED == ledActual);
        // La condición hace que se genere otro número si el nuevo LED coincide con el anterior.
        // De esta manera, el mismo LED nunca se prende dos veces seguidas.


        ledActual = nuevoLED;
        // Se actualiza el valor de ledActual con el nuevo LED seleccionado aleatoriamente.


        digitalWrite(leds[ledActual], HIGH);
        // Se prende el nuevo LED para indicarle al usuario cuál botón debe presionar.


        delay(200);
        // Se utiliza un pequeño delay para darle tiempo al usuario de reaccionar
        // antes de continuar con la siguiente lectura de los botones.
      }
    }
  }
}
