#ifndef I2C_H
#define I2C_H

#include <inttypes.h>

void i2cSendRegister(uint8_t reg, uint8_t data);
void i2cReceiveData(void);

#endif //I2C_H
