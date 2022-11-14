#include <Arduino.h>

#include "firebaseListener.h"
#include "data/dataHolder.h"

DataHolder data;
FirebaseListener firebaseListener;

void setup() {

  firebaseListener.init(DATA_FIELDS_COUNT);

  FirebaseListener::setDataParsingCallback([](const char* key, const char* value) -> DataItem {
    return data.parseDataFromKeyValue(key, value);
  });
  firebaseListener.registerDataChangeTask([](DataItem dataItem) {
    Serial.printf("Data changed: key: %d, value: %d", dataItem.key, (int) dataItem.value);
  });
  firebaseListener.start();

  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}