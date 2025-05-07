void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

unsigned long uart_tim { 0 };

enum {
  TURN_OFF = 'f',
  TURN_ON = 'o'
} availableCommands;

void loop() {
  auto now = millis();

  if((now > uart_tim) && (now - uart_tim)) {
    if(Serial.available() > 0) {
      char c = Serial.read();

      switch(c) {
        case TURN_OFF:
        case TURN_ON:
          Serial.print("CMD OK, LED: ");
          break;
        
        default:
          Serial.println("CMD unknown");
          break;
      }

      switch(c) {
        case TURN_OFF:
          digitalWrite(LED_BUILTIN, LOW);
          Serial.println("OFF");
          break;
        
        case TURN_ON:
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println("ON");
          break;
      }
    }
  }
}
