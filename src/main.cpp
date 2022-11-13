#include <Arduino.h>

#include "firebaseListener.h"
#include "data/dataHolder.h"

DataHolder data;
FirebaseListener firebaseListener;

void setup() {

  firebaseListener.init();

  FirebaseListener::setDataParsingCallback([](const char* key, const char* value) {
    data.parseDataFromKeyValue(key, value);
  });

  firebaseListener.registerDataChangeTask([]() {
    Serial.println("Data changed");
  });
  firebaseListener.start();

  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}