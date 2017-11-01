#include "EdpPacket.h"
#include "dht11.h"
#include <WiFiLink.h>
#include <inttypes.h>


#define BAUDRATE 9600
#define DEV_KEY "wxp=B2dRn9z5SBtWlC1Sf25UtUk="
#define DID  "20143408"
#define PID "100412"
#define AUTHINFO "test233"


char ssid[] = "dongyi320";     //  your network SSID (name)
char pass[] = "dongyi320";  // your network password
char hostname[] = "jjfaedp.hedevice.com";
int port = 876;
int ASignal_hmdty = A0;
int ASignal_itsty = A1;
int DHT11PIN = 9;

int index = 0;
int soilHmdty = 0;
int intensity = 0;
int airHmdty = 0;
int airTemp = 0;
int status = WL_IDLE_STATUS;     // the Wifi radio's status
WiFiClient client;
EdpPacketClass edp;
dht11 DHT11;


void buzzle()
{
    int i = 0;
    for(i=0;i<10;i++)//辒出一个频率的声音
      { 
        digitalWrite(13,HIGH);//发声音
        delay(100);//延时1ms 
        digitalWrite(13,LOW);//不发声音
        delay(100);//延时ms 
      }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connectWifi(){
  //Initialize serial and wait for port to open:
  Serial.begin(BAUDRATE);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Check if communication with the wifi module has been established
  if (WiFi.status() == WL_NO_WIFI_MODULE_COMM) {
    Serial.println("Communication with WiFi module not established.");
    while (true);// don't continue:
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);  
    Serial.println("Connected to wifi");
    printWifiStatus();
    
}
}

void setup() {
    pinMode(ASignal_hmdty,INPUT); 
    pinMode(ASignal_itsty,INPUT);
    Serial.begin(BAUDRATE);  
    Serial.setTimeout(3000);   //设置find超时时间
    //try to connect WiFi
    connectWifi();

    //try to connect TCP server
    Serial.println("\nStarting connection to server...");
    if(client.connect(hostname, port)){
      Serial.println("connected to server");  
    }
    else{
      Serial.println("connection failed");    
    }    
    delay(200);

    //build EDP connection
    edp.PacketConnect2(PID, AUTHINFO);
    client.write(edp.GetData(),edp.GetWritepos());
    edp.ClearParameter();
}

void loop() 
{
  //read and display datas
  int soilHmdty_tmp = analogRead(ASignal_hmdty);//soil humidity
  int intensity_tmp = analogRead(ASignal_itsty);//light intensity
  DHT11.read(DHT11PIN);//air temperature and humidity
  int airHmdty_tmp = (int)DHT11.humidity;
  int airTemp_tmp = (int)DHT11.temperature;
  Serial.print("soilHmdty_tmp = ");
  Serial.println(soilHmdty_tmp);
  Serial.print("intensity_tmp = ");
  Serial.println(intensity_tmp);
  Serial.print("airHmdty_tmp = ");
  Serial.println(airHmdty_tmp);
  Serial.print("airTemp_tmp = ");
  Serial.println(airTemp_tmp);
  soilHmdty += soilHmdty_tmp;
  intensity += intensity_tmp;
  airHmdty += airHmdty_tmp;
  airTemp += airTemp_tmp;
  index++;

  //send data
  if(index == 10){
    soilHmdty = soilHmdty/index;
    intensity = intensity/index;
    airHmdty = airHmdty/index;
    airTemp = airTemp/index;
    char send_buf[512];
    sprintf(send_buf, ",;humidity,%d;intensity,%d;airHumidity,%d;airTemperature,%d;",
        soilHmdty, intensity, airHmdty, airTemp);
    edp.PacketSavedataSimpleString(NULL, send_buf);
    client.write(edp.GetData(), edp.GetWritepos());
    edp.ClearParameter();
    Serial.print("data sended: soilHmdty = ");  
    Serial.println(soilHmdty);
    Serial.print("data sended: intensity = ");  
    Serial.println(intensity);
    Serial.print("data sended: airHmdty = ");  
    Serial.println(airHmdty);
    Serial.print("data sended: airTemp =  ");  
    Serial.println(airTemp);
    soilHmdty = 0;
    intensity = 0;
    airHmdty = 0;
    airTemp = 0;
    index = 0;
  }
  delay(6000);
}



