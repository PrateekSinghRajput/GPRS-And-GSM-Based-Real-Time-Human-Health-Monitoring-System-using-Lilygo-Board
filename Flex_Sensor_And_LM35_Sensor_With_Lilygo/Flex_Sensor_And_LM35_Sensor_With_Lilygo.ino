//Prateek
//https://justdoelectronics.com
//https://www.youtube.com/@JustDoElectronics/videos

#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

#define BLYNK_PRINT Serial
#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>
#include <Blynk.h>
#include <Wire.h>
#include <TinyGsmClient.h>
#include "utilities.h"
#define SerialMon Serial
#define SerialAT Serial1 
#define REPORTING_PERIOD_MS 1000
#define SMS_TARGET  "+918830584864"
const int analogIn = A6;

const char apn[]  = "www.ideagprs.com";
const char user[] = "";
const char pass[] = "";
const char auth[] = "apFsC_Ae_K216Y70-LCb0b0_4RF2pK9h";

TinyGsm modem(SerialAT);

int RawValue = 0;
double Voltage = 0;
double tempC = 0;
double tempF = 0;

unsigned int f;

void setup()
{

  SerialMon.begin(115200);
  delay(10);
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  SerialMon.println("Initializing modem...");
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem: ");
  SerialMon.println(modemInfo);
  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork(240000L)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  SerialMon.print(F("Connecting to APN: "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  Blynk.begin(auth, modem, apn, user, pass);
}

void loop()
{
  f = analogRead(A4);
  Serial.println(f);
  Blynk.run();

  RawValue = analogRead(analogIn);
  Voltage = (RawValue / 2048.0) * 3300; 
  tempC = Voltage * 0.1;
  tempF = (tempC * 1.8) + 32; 
  Serial.print("Raw Value = " ); 
  Serial.print(RawValue);
  Serial.print("\t milli volts = "); 
  Serial.print(Voltage, 0); //
  Serial.print("\t Temperature in C = ");
  Serial.print(tempC, 1);
  Serial.print("\t Temperature in F = ");
  Serial.println(tempF, 1);
  delay(500);
  Blynk.virtualWrite(V3, tempC);
  Blynk.virtualWrite(V2, tempF);

  if (f > 2520)            
  {
    Serial.println("Please Give Me Water");
    
    String smsMessage = "Please Give Me Water";
    if (modem.sendSMS(SMS_TARGET, smsMessage)) {
      SerialMon.println(smsMessage);
    }
    else {
      SerialMon.println("SMS failed to send");
    }
    WidgetLED led1(V6);
    WidgetLED led2(V5);
    WidgetLED led3(V4);
    led1.on();
    led2.off();
    led3.off();
    delay(1000);
  }
  else if (f < 2420)  
  {
    Serial.println("Please Give Me Food");
    
    String smsMessage = "Please Give Me Food";
    if (modem.sendSMS(SMS_TARGET, smsMessage)) {
      SerialMon.println(smsMessage);
    }
    else {
      SerialMon.println("SMS failed to send");
    }
    
    WidgetLED led2(V5);
    WidgetLED led1(V6);
    WidgetLED led3(V4);
    led2.on();
    led1.off();
    led3.off();
    delay(1000);
  }
  else                           
  {
    if (f > 2440)
      Serial.println("No Sign Language");
    WidgetLED led3(V4);
    WidgetLED led1(V6);
    WidgetLED led2(V5);
    led3.on();
    led1.off();
    led2.off();
    delay(1000);
  }

}
