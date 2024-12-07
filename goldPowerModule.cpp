#include "goldPowerModule.h"

void goldPowerModule::begin(uint8_t tx, uint8_t rx, uint8_t waitResponseInterval, uint16_t speed)
{
    RS485.begin(speed, SERIAL_8N1, rx, tx);
    this->waitResponseInterval = waitResponseInterval;
}

int8_t goldPowerModule::loop()
{
    if (!loopReadInterval.isExpired())
        return -1;

    sendMessageBruteForce(readLoopDataArray, sizeof(readLoopDataArray), 0);

    delay(waitResponseInterval);

    uint8_t *response;
    if (readMessage(response))
    {
        outputVoltage = (response[5] << 8) | response[6];
        outputCurrent = (response[7] << 8) | response[8];
        fanSpeed = (response[9] << 8) | response[10];
        alarm = response[11];
        protection = response[12];
        delete[] response;
        return 1;
    }
    else
        return 0;
}

bool goldPowerModule::readManufacturerInfo(char *info) // Verificar depois
{
    uint8_t buffer[] = {0x7e, 0x99, 0x01, 0x011, 0x00, 0x0d};

    sendMessageBruteForce(buffer, sizeof(buffer), 0);

    delay(waitResponseInterval);

    char *response;
    if (readMessage((uint8_t *)response))
    {
        if (!info)
            info = new char[38];

        strncpy(info, response + 4, 37); // Verificar depois
        info[37] = '\0';
        delete[] response;
        return true;
    }
    else
        return false;
}

bool goldPowerModule::readBarcode(char *barcode, uint8_t &factoryCode) // Verificar depois
{
    uint8_t buffer[] = {0x7e, 0x99, 0x01, 0x05, 0x00, 0x0d};

    sendMessageBruteForce(buffer, sizeof(buffer), 0);

    delay(waitResponseInterval);

    char *response;
    if (readMessage((uint8_t *)response))
    {
        if (!barcode)
            barcode = new char[28];

        strncpy(barcode, response + 5, 27); // Verificar depois
        barcode[27] = '\0';
        factoryCode = response[32];
        delete[] response;
        return true;
    }
    else
        return false;
}

bool goldPowerModule::readVersion(uint8_t &boardStatus, uint8_t &softMacro, uint8_t &softMinor, uint8_t &pcbVersion, uint8_t &hardProgVersion, uint8_t &yearManufacture, uint8_t &monthManufacture, uint8_t &dayManufacture)
{
    uint8_t buffer[] = {0x7e, 0x99, 0x01, 0x01, 0x00, 0x0d};

    sendMessageBruteForce(buffer, sizeof(buffer), 0);

    delay(waitResponseInterval);

    uint8_t *response;
    if (readMessage(response))
    {
        boardStatus = response[5];
        softMacro = response[6];
        softMinor = response[7];
        pcbVersion = response[8];
        hardProgVersion = response[9];
        yearManufacture = response[10];
        monthManufacture = response[11];
        dayManufacture = response[12];
        delete[] response;
        return true;
    }
    else
        return false;
}

bool goldPowerModule::setDefaultOutputVoltage(uint16_t voltage)
{
    getFloatout2hex(voltage, vArray);

    uint8_t buffer[] = {0x7e, 0x99, 0x01, 0x15, vArray[0], vArray[1], 0x00, 0x0d};

    return sendMessageBruteForce(buffer, sizeof(buffer));
}

bool goldPowerModule::setDefaultOutputCurrent(uint16_t current)
{
    getFloatout2hex(current, vArray);

    uint8_t buffer[] = {0x7e, 0x99, 0x01, 0x18, vArray[0], vArray[1], 0x00, 0x0d};

    return sendMessageBruteForce(buffer, sizeof(buffer));
}

uint16_t goldPowerModule::readDefaultOutputVoltage()
{
    uint8_t buffer[] = {0x7e, 0x01, 0x01, 0x16, 0x00, 0x0d};

    sendMessageBruteForce(buffer, sizeof(buffer), 0);

    delay(waitResponseInterval);

    uint8_t *response;
    if (readMessage(response))
    {
        uint16_t voltage = (response[5] << 8) | response[6];
        delete[] response;
        return voltage;
    }
    else
        return 0;
}

uint16_t goldPowerModule::readDefaultOutputCurrent()
{
    uint8_t buffer[] = {0x7e, 0x01, 0x01, 0x17, 0x00, 0x0d};

    sendMessageBruteForce(buffer, sizeof(buffer), 0);

    delay(waitResponseInterval);

    uint8_t *response;
    if (readMessage(response))
    {
        uint16_t voltage = (response[5] << 8) | response[6];
        delete[] response;
        return voltage;
    }
    else
        return 0;
}

bool goldPowerModule::readOutputConfig(uint16_t &voltage, uint16_t &current)
{
    uint8_t buffer[] = {0x7e, 0x01, 0x01, 0x07, 0x00, 0x0d};

    sendMessageBruteForce(buffer, sizeof(buffer), 0);

    delay(waitResponseInterval);

    uint8_t *response;
    if (readMessage(response))
    {
        voltage = (response[5] << 8) | response[6];
        current = (response[7] << 8) | response[8];
        delete[] response;
        return true;
    }
    else
        return false;
}

bool goldPowerModule::setOutputFloat(float voltage, float current)
{
    getFloatout2hex(voltage, vArray);
    getFloatout2hex(current, iArray);

    uint8_t buffer[] = {0x7e, 0x99, 0x05, 0x06, vArray[0], vArray[1], iArray[0], iArray[1], 0x00, 0x0d};

    return sendMessageBruteForce(buffer, sizeof(buffer));
}

bool goldPowerModule::setOutput(uint16_t voltage, uint16_t current)
{
    get16out2hex(voltage, vArray);
    get16out2hex(current, iArray);

    uint8_t buffer[] = {0x7e, 0x99, 0x05, 0x06, vArray[0], vArray[1], iArray[0], iArray[1], 0x00, 0x0d};

    return sendMessageBruteForce(buffer, sizeof(buffer));
}

bool goldPowerModule::setOutputOnOff(bool onOff)
{
    if (onOff)
    {
        uint8_t powerOnArray[8] = {0x7e, 0x01, 0x03, 0x04, 0x00, 0x00, 0x08, 0x0d};
        return sendMessageBruteForce(powerOnArray, sizeof(powerOnArray));
    }
    else
    {
        uint8_t powerOffArray[8] = {0x7e, 0x01, 0x03, 0x04, 0x01, 0x00, 0x09, 0x0d};
        return sendMessageBruteForce(powerOffArray, sizeof(powerOffArray));
    }
}

bool goldPowerModule::sendMessageBruteForce(uint8_t *buffer, uint8_t size, bool clearRx)
{
    timeOutInterval.start(timeOutIntervalTime, AsyncDelay::MILLIS);

    while (!RS485.available())
    {
        RS485.write(readLoopDataArray, size);

        readLoopDataArray[size - 2] = readLoopDataArray[size - 2] + 1;
        if (readLoopDataArray[size - 2] == 0xFF)
            readLoopDataArray[size - 2] = 0x00;

        delay(waitResponseInterval);

        if (timeOutInterval.isExpired())
            return false;
    }

    if (clearRx)
        while (RS485.available())
            RS485.read();

    return true;
}

bool goldPowerModule::sendMessage(uint8_t *buffer, uint8_t size, bool clearRx)
{
    RS485.write(readLoopDataArray, 10);

    delay(waitResponseInterval);

    if (RS485.available())
    {
        if (clearRx)
            while (RS485.available())
                RS485.read();
        return true;
    }
    else
        return false;
}

bool goldPowerModule::readMessage(uint8_t *response)
{
    if (RS485.available())
    {
        response = new uint8_t[RS485.available() + 1];
        RS485.readBytes(response, RS485.available());
        return true;
    }
    else
        return false;
}

void goldPowerModule::get16out2hex(uint16_t value, uint8_t *buffer)
{
    memset(buffer, 0, 2);
    float tensaoBat = value;
    tensaoBat /= 100;

    char battVoltChar[10];
    dtostrf(tensaoBat, 4, 2, battVoltChar);

    char
        *ponto = strchr(battVoltChar, '.'),
        parteInteira[10],
        parteDecimal[10];

    strncpy(parteInteira, battVoltChar, ponto - battVoltChar);
    parteInteira[ponto - battVoltChar] = '\0';
    strcpy(parteDecimal, ponto + 1);

    buffer[0] = strtol(parteInteira, nullptr, 16);
    buffer[1] = strtol(parteDecimal, nullptr, 16);
}

void goldPowerModule::getFloatout2hex(float value, uint8_t *buffer)
{
    memset(buffer, 0, 2);
    char battVoltChar[10];
    dtostrf(value, 4, 2, battVoltChar);

    char
        *ponto = strchr(battVoltChar, '.'),
        parteInteira[10],
        parteDecimal[10];

    strncpy(parteInteira, battVoltChar, ponto - battVoltChar);
    parteInteira[ponto - battVoltChar] = '\0';
    strcpy(parteDecimal, ponto + 1);

    buffer[0] = strtol(parteInteira, nullptr, 16);
    buffer[1] = strtol(parteDecimal, nullptr, 16);
}

void goldPowerModule::setReadInterval(uint16_t interval)
{
    loopReadInterval.start(interval, AsyncDelay::MILLIS);
}

void goldPowerModule::setTimeOutInterval(uint16_t interval)
{
    timeOutIntervalTime = interval;
}

void goldPowerModule::setLoopInterval(uint16_t interval)
{
    loopReadInterval.start(interval, AsyncDelay::MILLIS);
}