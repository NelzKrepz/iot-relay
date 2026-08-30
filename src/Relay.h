#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>
#include <functional>

class Relay {
  private:
    byte relay_pin;
    byte relay_state;
    std::function<void(byte)> callback;   // callback saat state berubah

  public:
    String display_name;

    // Constructor dengan callback optional
    Relay(byte pin, String displayName = "", std::function<void(byte)> cb = nullptr);

    void setState(byte new_state);
    byte getState();
    byte getPin();
};

#endif
