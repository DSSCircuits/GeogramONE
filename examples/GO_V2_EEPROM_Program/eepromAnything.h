#include <EEPROM.h>
#include <WProgram.h>

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const char* p = (const char*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    char* p = (char*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
		*p++ = EEPROM.read(ee++);
    return i;
}

