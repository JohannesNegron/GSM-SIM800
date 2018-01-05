#include <SIM800.h>

SIM800 gsm(11, 12, 13, true);
void setup() {
  char myConcat[30];
  gsm.begin(115200);
  Serial.begin(9600);
  while (!Serial);
  while (!gsm);
  gsm.print("AT+IPR=9600");
  gsm.begin(9600);
  while (!gsm);
  
  Serial.println("Definiendo parametros");
  
  gsm.sendAT("AT+CFUN=1,1\r\n","PSUTTZ",30000);
  
  gsm.sendAT("AT+CPIN?\r\n", "OK");
  gsm.sendAT("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n","OK");
  gsm.sendAT("AT+SAPBR=3,1,\"APN\",\"internet.itelcel.com\"\r\n","OK");
  gsm.sendAT("AT+SAPBR=3,1,\"USER\",webgprs\r\n","OK");
  gsm.sendAT("AT+SAPBR=3,1,\"PWD\",webgprs2003\r\n","OK");
  gsm.sendAT("AT+SAPBR=1,1\r\n","OK");
  gsm.sendAT("AT+SAPBR=2,1\r\n","OK");
  Serial.println("Configuracion de zona horaria");
  gsm.sendAT("AT+CLTS=1\r\n","OK");
  gsm.sendAT("AT+CFUN=1,1\r\n","PSUTTZ",30000);
  Serial.println("READY");
  
  while(1)
  {
    String s_data = createXML("1","90", gsm.getTime());
    Serial.println(gsm.post("http://4faf9a81.ngrok.io","aplication/xml",s_data));
    delay(5000);
  }
}
String createXML(String id, String potencia, String date)
{
  return "<SunSpecData v=\"1.0\">\n<d id=\""+id+"\" t=\""+date+"\">\n<m id=\"potenciometro\" x=\"1\">\n<p id=\"potencia\" t=\""+date+"\">"+potencia+"</p>\n</m>\n</d>\n</SunSpecData>";
}
void loop() {
    if(Serial.available()>0)
    {
      char char_in = (char)Serial.read();
      Serial1.print(char_in);
      Serial.print(char_in);
      gsm.print(char_in);
    }
    if(gsm.available()>0)
    {
      Serial.print((char)gsm.read());
    }
}
