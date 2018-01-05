#ifndef __SIM800_H__
#define __SIM800_H__

#include "Arduino.h"
#include <SoftwareSerial.h>

#define TRUE                    1
#define FALSE                   0

#define DEFAULT_TIMEOUT         5000


/** SIM800 class.
 *  Used for SIM800 communication. attention that SIM800 module communicate with MCU in serial protocol
 */
class SIM800 : public  SoftwareSerial
{

public:
    /** Create SIM800 Instance
     *  @param baudRate baud rate of uart communication
     *  @param rxPin uart receive pin to communicate with SIM800
     *  @param txPin uart transmit pin to communicate with SIM800
     */
    SIM800(unsigned int rxPin, unsigned int txPin, unsigned int rstPin=13, bool debug=true):SoftwareSerial(rxPin, txPin) {
        debugMode = debug;
        resetPin = rstPin;
    };

    /** Power on SIM800
     */
    int preInit(void);

    /** Check if SIM800 readable
     */
    int checkReadable(void);

    /** read from SIM800 module and save to buffer array
     *  @param  buffer  buffer array to save what read from SIM800 module
     *  @param  count   the maximal bytes number read from SIM800 module
     *  @param  timeOut time to wait for reading from SIM800 module
     *  @returns
     *      TRUE on success
     *      ERROR on error
     */
    int readBuffer(char* buffer,int count, unsigned int timeOut = DEFAULT_TIMEOUT);


    /** clean Buffer
     *  @param buffer   buffer to clean
     *  @param count    number of bytes to clean
     */
    void cleanBuffer(char* buffer, int count);

    /** send AT command to SIM800 module
     *  @param cmd  command array which will be send to GPRS module
     */
    void sendCmd(const char* cmd);

    /**send "AT" to SIM800 module
     */
    int sendATTest(void);

    /**send "AT" to SIM800 module
     */
    int sendAT(const char* cmd, const char* resp, unsigned int timeout = DEFAULT_TIMEOUT);
    String post(const char* url, const char* contype="", char* data="");
    String post(const char* url, const char* contype="", String data="");
    String get(const char* cmd);


    /**send '0x1A' to SIM800 Module
     */
    void sendEndMark(void);

    /** check SIM800 module response before time out
     *  @param  *resp   correct response which SIM800 module will return
     *  @param  *timeout    waiting seconds till timeout
     *  @returns
     *      TRUE on success
     *      ERROR on error
     */
    int waitForResp(const char* resp, unsigned timeout);

    /** send AT command to GPRS module and wait for correct response
     *  @param  *cmd    AT command which will be send to GPRS module
     *  @param  *resp   correct response which GPRS module will return
     *  @param  *timeout    waiting seconds till timeout
     *  @returns
     *      TRUE on success
     *      ERROR on error
     */
    int sendCmdAndWaitForResp(const char* cmd, const char *resp, unsigned timeout);


    /** used for serial debug, you can specify tx and rx pin and then communicate with GPRS module with common AT commands
     */
    void serialDebug(void);

    void purgeSerial();
    String getTime();

    String buffer;

private:

    //SoftwareSerial serialSIM800;
    bool debugMode;
    unsigned int resetPin;

};

#endif