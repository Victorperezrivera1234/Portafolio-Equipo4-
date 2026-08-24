const int pinLED = 13;
const int pinBOTON = 2;
void setup(){
  pinMode(pinBOTON,INPUT_PULLUP);
  pinMode(pinLED,OUTPUT);
}
void loop(){
  int estadoBoton = digitalRead(pinBOTON);
  if(estadoBoton == LOW){
    digitalWrite(pinLED,LOW);
  }else{
    digitalWrite(pinLED,HIGH);
  }
}

  
  