int leds[] = {13, 12, 11}; //Se definen los diferentes pines que van a ser utilizados para alimentar a los leds.
int botones[] = {8, 7, 6}; // En este arreglo se definen los pines que se van a encargar de leer la señal del botón.
int ledActual; //Esta variable se define para luego darle un valor aleatorio.

void setup() {
  for (int i = 0; i < 3; i++) { //Se utiliza esta estructura para poder poner todo el arreglo en modo OUTPUT.
    pinMode(leds[i], OUTPUT); //Se pone el arreglo completo en función de Salida.
  }
  for (int i = 0; i < 3; i++) { //Mismo concepto que en el anterior. 
    pinMode(botones[i], INPUT_PULLUP); //Los pines del botón detectan cuando el circuito esta abierto o cerrado.
  }
  randomSeed(analogRead(A0)); //La función de randomseed se utiliza para que cada vez que se ejecute el código el LED prendido inicalmente este en diferente posición.
  ledActual = random(0, 3); //Se utiliza la variable de LedActual, para que se le asigne un valor aleatorio.
  digitalWrite(leds[ledActual], HIGH); //Después de definir el valor deLedActual, se utiliza la función de digitalWrite para encender uno de los leds del arreglo en base al valor de la variable anterior.
}

void loop() {
  for (int i = 0; i < 3; i++) {
    if (digitalRead(botones[i]) == LOW) { //Se abre una estructura if para leer cual de los botones esta siendo presionado por el usuario.
      if (i == ledActual) { //Este if se utiliza para confirmar que el botón presionado sea correspondiente al LED.
        digitalWrite(leds[ledActual], LOW); //En el caso de que el LED y el botón sean correspondientes se apagará el LED para continuar el juego.
        int nuevoLED; //Se define una nueva variable para usarese luego.

        do { //Esta estructura lógica se ejecuta obligaoriamente una vez, y las demás veces se ejecutara cuando la condición este presente. 
          nuevoLED = random(0, 3); //Se le asgina un valor aleatorio a la nueva variable.
        } while (nuevoLED == ledActual); //En esta línea esta la condición que hara que se repita el bloque. En este caso es que si el valor de nuevoLED coincide con ledAcutal.
        ledActual = nuevoLED; //Como el código actualiza el valor de nuevoLED cada vez que conincide con ledActual, el mismo LED nunca se va a prender dos veces seguidas.
        digitalWrite(leds[ledActual], HIGH); //Se prende nuevamente un LED.
        delay(200); //Se utiliza un delay para que le de tiempo al usuario de reaccionar al nuevo LED prendido.
      }
    }
  }
}