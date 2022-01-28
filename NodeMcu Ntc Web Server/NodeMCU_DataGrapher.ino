#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "index.h" 

const double VCC = 3.3;             
const double R2 = 250;           
const double adc_resolution = 1023; 

const double A = 0.001129148;  
const double B = 0.000234125;
const double C = 0.0000000876741; 




int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;


//SSID and Password of your WiFi router
const char* ssid = "NTC TEMP.SENSOR(192.168.0.157)";
  IPAddress IPaddr (192,168,0,157);
  IPAddress IPmask(255, 255, 255, 0);
ESP8266WebServer server(80); //Server on port 80
 
//===============================================================
// This routine is executed when you open it's IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleADC() {
 double Vout, Rth, temperature, adc_value; 

  adc_value = analogRead(A0);
  Vout = (adc_value * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;

/*  Steinhart-Hart Thermistor Equation:
 *  Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
 *  where A = 0.001129148, B = 0.000234125 and C = 8.76741*10^-8  */
  temperature = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)),3))));   // Temperature in kelvin
  temperature = temperature - 273.15;  // Temperature in degree celsius
  int a = temperature;

 double Ax, Ay, Az, T, Gx, Gy, Gz;
  

  //divide each with their sensitivity scale factor
 
  T = (double)Temperature/340+36.53; //temperature formula


 String data = "{\"ADC\":\""+String(a)+"\", \"Temperature\":\""+ String(T) +"\", \"Ax\":\""+ String(Ax) +"\", \"Ay\":\""+ String(Ay) +"\", \"Az\":\""+ String(Az) +"\", \"Gx\":\""+ String(Gx) +"\", \"Gy\":\""+ String(Gy) +"\", \"Gz\":\""+ String(Gz) +"\"}";
 server.send(200, "text/plane", data); //Send values' JSON to client ajax request
 /*
  Serial.print("Ax: "); Serial.print(Ax);
  Serial.print(" Ay: "); Serial.print(Ay);
  Serial.print(" Az: "); Serial.print(Az);
  Serial.print(" T: "); Serial.print(T);
  Serial.print(" Gx: "); Serial.print(Gx);
  Serial.print(" Gy: "); Serial.print(Gy);
  Serial.print(" Gz: "); Serial.println(Gz);
  
  Serial.print(temperature, 1);
  */
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
   WiFi.softAP(ssid);     
  WiFi.softAPConfig(IPaddr, IPaddr, IPmask);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("");
  server.on("/", handleRoot);      
  server.on("/readADC", handleADC); 
  server.begin(); 
  Serial.println("HTTP server started");
    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Hata[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Yetki Hatası");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Baslatma Basarisiz");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Baglanti Basarisiz");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Veri Alinamadi");
    } else if (error == OTA_END_ERROR) {
      Serial.println("Sonlandirma Basarisiz");
    }
  });
    ArduinoOTA.begin();//OTA'yi başlatıyor
  Serial.println("Sistem Hazir");
  Serial.print("IP Adresiniz: ");
  Serial.println(WiFi.localIP());

}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}



void loop()
{
  ArduinoOTA.handle();
  server.handleClient();          //Handle client requests
}
