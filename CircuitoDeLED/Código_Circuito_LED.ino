const int pinLED = 13; // Se definen los pines como constantes para poder recordarlas de manera más sencilla.
const int pinBOTON = 2;
void setup(){
  pinMode(pinBOTON,INPUT_PULLUP); // Aquí se utiliza la función INPUT_PULLUP para poder eliminar cualquier ruido del pulsador.
  pinMode(pinLED,OUTPUT); //Se utiliza la función OUTPUT para abrir o cerrar el LED; en base al pulsador.
}
void loop(){
  int estadoBoton = digitalRead(pinBOTON); // En esta línea se crea una variable para abrir un if dependiendo de que si el pulsador esta cerrado o abierto.
  if(estadoBoton == LOW){ //Se define la condición que tiene que seguir el if; en este caso el LED se apagara cuando el botón este presionado, puesto que esta normalmente abierto.
    digitalWrite(pinLED,LOW);
  }else{
    digitalWrite(pinLED,HIGH); // En caso de que no este presionado el botón, el circuito estara abierto y por ende el LED seguirá encendido.
  }
}
