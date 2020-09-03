// Ejercicio 2
// Implementar para una EEPROM 24LC1025 el comando de 
// lectura de un byte en una dirección elegida (Random read):


uint8_t i2cSlaveAddress = 0b01010010; // 7-bit addr

uint8_t transmitDataBuffer[3] = {
     0x01, 0x77, 0xCA
};

i2cReadWrite( I2C0, i2cSlaveAddress,
          transmitDataBuffer, 3, true );
          
bool_t i2cWriteRead( i2cMap_t i2cNumber,
         uint8_t  i2cSlaveAddress,
         uint8_t* dataToReadBuffer,
         uint16_t dataToReadBufferSize,
         bool_t   sendWriteStop,
         uint8_t* receiveDataBuffer,
         uint16_t receiveDataBufferSize,
         bool_t   sendReadStop );