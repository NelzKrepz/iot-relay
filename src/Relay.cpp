#include "Relay.h"

Relay::Relay(byte pin, String displayName, std::function<void(byte)> cb) {
  relay_pin = pin;
  relay_state = HIGH;
  callback = cb;
  
  display_name = (displayName == "") ? "IO" + String(pin) : displayName;

  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void Relay::setState(byte new_state) {
  relay_state = new_state;
  digitalWrite(relay_pin, new_state);

  // Jika callback ada → panggil bersama state barunya
  if (callback) {
    callback(new_state);
  }
}

byte Relay::getState() {
  return relay_state;
}

byte Relay::getPin() {
  return relay_pin;
}
