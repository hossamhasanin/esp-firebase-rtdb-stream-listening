#include <stdint.h>
#include "dataItem.h"

#define DATA_FIELDS_COUNT 2


class DataHolder {
    private :
        uint8_t _data[DATA_FIELDS_COUNT];

        void setIfChanged(int key , uint8_t valueByte , DataItem* changedDataItem);
    
    public :
        DataItem parseDataFromKeyValue(int key, uint8_t value);
        uint8_t get(int key);
        void set(int key , uint8_t value);

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

