static unsigned long led_timer = 0;
static bool led_state = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  if((led_timer < now) && (now - led_timer >= 1000)) {
    led_state = !led_state;
    led_timer += 1000;

    digitalWrite(LED_BUILTIN, led_state);
  }
}

