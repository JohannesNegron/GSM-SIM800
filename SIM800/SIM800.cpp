#include "SIM800.h"
#define HTTP_PARA_URL "AT+HTTPPARA=\"URL\",\"%s\"\r\n"
#define HTTP_PARA_CONTENT "AT+HTTPPARA=\"CONTENT\",\"%s\"\r\n"
#define HTTP_PARA_DATA "AT+HTTPDATA=%i,10000\r\n"
int SIM800::preInit(void)
{
    pinMode(resetPin, OUTPUT);

    digitalWrite(resetPin, HIGH);
    delay(200);
    digitalWrite(resetPin, LOW);
    delay(2000);
    digitalWrite(resetPin, HIGH);
    delay(3000);

    purgeSerial();

    return TRUE;
}

int SIM800::checkReadable(void)
{
    return available();
}

int SIM800::readBuffer(char *buffer, int count, unsigned int timeOut)
{
    int i = 0;
    unsigned long timerStart,timerEnd;
    timerStart = millis();
    while(1) {
        while (available()) {
            char c = read();
            buffer[i] = c;
            buffer[i + 1] = '\0';
            ++i;
            if(i > count-1)break;
        }
        if(i > count-1)break;
        timerEnd = millis();
        if(timerEnd - timerStart > timeOut) {
            break;
        }
    }
    delay(500);
    while(available()) {
        read();
    }
    return TRUE;
}


void SIM800::cleanBuffer(char *buffer, int count)
{
    for(int i=0; i < count; i++) {
        buffer[i] = '\0';
    }
}

String SIM800::post(const char* url, const char* contype, String data2send)
{
    char c_data[200];
    data2send.toCharArray(c_data,data2send.length());
    post(url, contype, c_data);
}

String SIM800::post(const char* url, const char* contype, char* data2send)
{
    int size_contype = strlen(contype);
    int size_userdata = strlen(data2send);
    sendAT("AT+HTTPINIT\r\n","OK");
    sendAT("AT+HTTPPARA=\"CID\",1\r\n","OK");
    //DEFINE URL 
    char httpParaUrl[128];
    sprintf(httpParaUrl, HTTP_PARA_URL, url);
    sendAT(httpParaUrl,"OK");
    //DEFINE CONTYPE
    if(size_contype > 0)
    {
        char httpParaContent[128];
        sprintf(httpParaContent, HTTP_PARA_CONTENT, contype);
        sendAT(httpParaContent,"OK");
    }
    //DEFINE DATA
    if(size_userdata > 0)
    {
        char httpParaData[128];
        sprintf(httpParaData, HTTP_PARA_DATA, size_userdata);
        sendAT(httpParaData,"DOWNLOAD");
        //Serial.println(buffer);
        sendAT(data2send, "OK");
        //Serial.println(buffer);
    }
    //Action!!
    sendAT("AT+HTTPACTION=1\r\n","-1",10000);
    String status = buffer;
    int i = buffer.indexOf(",")+1;
    status = status.substring(i);
    status = status.substring(0,status.indexOf("\n")-1);
    int index_coma = status.indexOf(",");
    String status_s = status.substring(0,index_coma);
    String size_data = status.substring(index_coma+1);
    String data = "";
    if(size_data == "0")
    {
        data = "NULL";
    }
    else
    {
        sendAT("AT+HTTPREAD\r\n","OK");
        data = buffer;
        data = data.substring(buffer.indexOf(size_data)+size_data.length()+2, buffer.indexOf("OK")-1);
    }
    
    sendAT("AT+HTTPTERM\r\n","OK");
    return status_s+","+data;
}

String SIM800::get(const char* url)
{
    sendAT("AT+HTTPINIT\r\n","OK");
    sendAT("AT+HTTPPARA=\"CID\",1\r\n","OK");
    char httpParaUrl[128];
    sprintf(httpParaUrl, HTTP_PARA_URL, url);
    sendAT(httpParaUrl,"OK");
    sendAT("AT+HTTPACTION=0\r\n","-1",10000);
    String status = buffer;
    int i = buffer.indexOf(",")+1;
    status = status.substring(i);
    status = status.substring(0,status.indexOf("\n")-1);
    int index_coma = status.indexOf(",");
    String status_s = status.substring(0,index_coma);
    String size_data = status.substring(index_coma+1);
    String data;
    if(size_data == "0")
    {
        data = "NULL";
    }
    else
    {
        sendAT("AT+HTTPREAD\r\n","OK");
        data = buffer;
        data = data.substring(buffer.indexOf(size_data)+size_data.length()+2, buffer.indexOf("OK")-1);
    }
    
    sendAT("AT+HTTPTERM\r\n","OK");
    return status_s+","+data;
}

void SIM800::sendCmd(const char* cmd)
{
    listen();
    write(cmd);
}

int SIM800::sendATTest(void)
{
    int ret = sendCmdAndWaitForResp("AT\r\n", "OK", DEFAULT_TIMEOUT);
    return ret;
}

int SIM800::sendAT(const char* cmd, const char* resp, unsigned int timeout = DEFAULT_TIMEOUT)
{
    int ret = sendCmdAndWaitForResp(cmd, resp, timeout);
    return ret;
}

int SIM800::waitForResp(const char *resp, unsigned int timeout)
{
    buffer = "";
    int len = strlen(resp);
    int sum=0;
    unsigned long timerStart,timerEnd;
    timerStart = millis();

    while(1) {
        if(available()) {
            char c = read();
            if (debugMode) Serial.print(c);
            buffer += String(c);
            sum = (c==resp[sum]) ? sum+1 : 0;
            if(sum == len)break;
        }
        timerEnd = millis();
        if(timerEnd - timerStart > timeout) {
            return FALSE;
        }
    }

    while(available()) {
        read();
    }

    return TRUE;
}

void SIM800::sendEndMark(void)
{
    println((char)26);
}

int SIM800::sendCmdAndWaitForResp(const char* cmd, const char *resp, unsigned timeout)
{
    delay(1000);
    sendCmd(cmd);
    return waitForResp(resp,timeout);
}

void SIM800::serialDebug(void)
{
    while(1) {
        if(available()){
            Serial.write(read());
        }
        if(Serial.available()){
            write(Serial.read());
        }
    }
}

void SIM800::purgeSerial()
{
  while (available()) read();
}

String SIM800::getTime()
{
    sendAT("AT+CCLK?\r\n", "OK");
    String s_time = buffer;
    int index_com = s_time.indexOf("\"");
    int index_gui = s_time.indexOf("-");
    s_time = s_time.substring(index_com+1, index_gui);
    index_com = s_time.indexOf(",");
    s_time.replace("/", "-");
    return "20"+s_time.substring(0,index_com)+" "+s_time.substring(index_com+1);
}