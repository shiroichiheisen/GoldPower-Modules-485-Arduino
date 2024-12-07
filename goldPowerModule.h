#ifndef GOLD_POWER_MODULE_H
#define GOLD_POWER_MODULE_H

#include <Arduino.h>
#include <AsyncDelay.h>

class goldPowerModule
{
public:
    void begin(uint8_t tx, uint8_t rx, uint8_t waitResponseInterval = 50, uint16_t speed = 4800),
        setReadInterval(uint16_t interval),
        setTimeOutInterval(uint16_t interval),
        setLoopInterval(uint16_t interval);

    uint16_t readDefaultOutputVoltage(),
        readDefaultOutputCurrent(),

        outputVoltage,
        fanSpeed,
        outputCurrent;

    uint8_t
        alarm,
        protection;

    int8_t
    loop();

    bool readOutputConfig(uint16_t &voltage, uint16_t &current),
        setDefaultOutputVoltage(uint16_t voltage),
        setDefaultOutputCurrent(uint16_t current),
        setOutputOnOff(bool onOff),
        setOutput(uint16_t voltage, uint16_t current),
        setOutputFloat(float voltage, float current),
        readManufacturerInfo(char *info),
        readBarcode(char *barcode, uint8_t &factoryCode),
        readVersion(uint8_t &boardStatus, uint8_t &softMacro, uint8_t &softMinor, uint8_t &pcbVersion, uint8_t &hardProgVersion, uint8_t &yearManufacture, uint8_t &monthManufacture, uint8_t &dayManufacture);

private:
    void get16out2hex(uint16_t value, uint8_t *buffer),
        getFloatout2hex(float value, uint8_t *buffer);

    HardwareSerial RS485{1};

    AsyncDelay
        loopReadInterval{5000, AsyncDelay::MILLIS},
        timeOutInterval;

    uint16_t
        timeOutIntervalTime = 1000;

    uint8_t
        vArray[2],
        iArray[2],
        waitResponseInterval,
        readLoopDataArray[6] = {0x7e, 0x99, 0x01, 0x03, 0x96, 0x0d};

    bool sendMessageBruteForce(uint8_t *buffer, uint8_t size, bool clearRx = 1),
        sendMessage(uint8_t *buffer, uint8_t size, bool clearRx = 1),
        readMessage(uint8_t *response);
};

#endif