#include <stdint.h>

#define DATA_FIELDS_COUNT 2


class DataHolder {
    private :
        uint8_t _data[DATA_FIELDS_COUNT];
    
    public :
        void parseDataFromKeyValue(const char* key, const char* value);
};

// create enum for the data fields
enum dataFields {
    // add the fields here
    // for example:
    // field1,
    // field2,
    // field3
    // create member called temprature that is equal to 0
    temprature = 0,
    switch1 = 1,
};

