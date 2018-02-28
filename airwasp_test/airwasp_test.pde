// Step 1. Includes of the Sensor Board and Communications modules used
#include <WaspSensorGas_v20.h>
#include <Wasp232.h>
#include <stdlib.h>
// Step 2. Variables declaration
char  CONNECTOR_A[3] = "CA";      
char  CONNECTOR_B[3] = "CB";    
char  CONNECTOR_C[3] = "CC";
char  CONNECTOR_D[3] = "CD";
char  CONNECTOR_E[3] = "CE";
char  CONNECTOR_F[3] = "CF";
 
                                               
//char  nodeID[10] = "403472985";   

char* sleepTime = "00:00:00:10";           

char data[100];     

float connectorAFloatValue; 
float connectorBFloatValue;  
float connectorCFloatValue;    
float connectorDFloatValue;   
float connectorEFloatValue;
float connectorFFloatValue;

int connectorAIntValue;
int connectorBIntValue;
int connectorCIntValue;
int connectorDIntValue;
int connectorEIntValue;
int connectorFIntValue;

char  connectorAString[10];  
char  connectorBString[10];   
char  connectorCString[10];
char  connectorDString[10];
char  connectorEString[10];
char  connectorFString[10];

char  TIME_STAMP[3] = "TS";

const char appEui[] = "70B3D57ED00074FE";
const char appKey[] = "35AC3CD8615D97F43B82E2B350626ACA";

void receive_from_mdot(void) {
  while(1) {
    if(W232.receive() > 0) {
      USB.println(W232._buffer, W232._length);
      break;
    }

  }
}

void send_config_str(const char *str) {
  W232.send(str);
  receive_from_mdot();
  delay(50);
}

void lora_setup() {
  W232.ON(SOCKET0);
  delay(100);
  W232.parityBit(NONE);
  W232.stopBitConfig(1);
  W232.baudRateConfig(115200);
  delay(100);

  //connect to network here and all that jazz, look at saving in NVM
  send_config_str("AT&F\n");
  send_config_str("AT+PN=1\n");
  send_config_str("AT+FSB=2\n");
  send_config_str("AT+NJM=0\n");
  send_config_str("AT+NA=26:01:17:20\n");
  send_config_str("AT+NSK=44:C5:43:D4:68:F4:15:C4:DE:37:19:BE:1B:97:55:4D\n");
  send_config_str("AT+DSK=2D:38:C0:8A:C3:F7:DE:C6:01:0C:33:83:FF:51:4D:27\n");
  send_config_str("AT+TXDR=DR3\n");
  send_config_str("AT&W\n");
}

void Configure_Sensors(void) {
// Step 8. Turn on the Sensor Board

    //Turn on the sensor board
    SensorGasv20.ON();
    //Turn on the RTC
    RTC.ON();
    //supply stabilization delay
    delay(100);

// Step 9. Turn on the sensors
    USB.println("waiting 10 seconds for stabilisation");
    delay(10000);

    //Turning on NO2 Sensor
    SensorGasv20.setSensorMode(SENS_ON, SENS_SOCKET3B);
    USB.println("Turning on NO2");
    delay(30000);

    //Turning on and configuring O3 Sensor
    SensorGasv20.setSensorMode(SENS_ON, SENS_SOCKET2B);
    SensorGasv20.configureSensor(SENS_SOCKET2B, 1, 10);
    USB.println("Turning on O3");
    delay(30000);
    SensorGasv20.configureSensor(SENS_SOCKET4CO, 1, 100);
    USB.println("Turning on CO");
    delay(30000);
    USB.println("Turning on CO2");       
    SensorGasv20.configureSensor(SENS_CO2, 7);
    SensorGasv20.setSensorMode(SENS_ON, SENS_CO2);    
    delay(30000);
}

void setup() 
{

// Step 3. Communication module initialization

// Step 4. Communication module to ON

    USB.ON();
    delay(100);
    //USB.baudRateConfig(9600);
// Step 5. Initial message composition

    sprintf(data,"Hello, this is Waspmote Plug & Sense!\r\n");

// Step 6. Initial message transmission

    USB.println(data);

// Step 7. Communication module to OFF
}



void Read_Sensors(void) {
 // Step 10. Read the sensors
    USB.println("Reading Temp");
    //First dummy reading for analog-to-digital converter channel selection
    SensorGasv20.readValue(SENS_TEMPERATURE);
    //Sensor temperature reading
    connectorAFloatValue = SensorGasv20.readValue(SENS_TEMPERATURE);
    //Conversion into a string
    Utils.float2String(connectorAFloatValue, connectorAString, 2);
    USB.println("Reading CO");
    SensorGasv20.readValue(SENS_SOCKET4A);
    connectorFFloatValue = SensorGasv20.readValue(SENS_SOCKET4CO);    
    //Conversion into a string
    Utils.float2String(connectorFFloatValue, connectorFString, 2);
    USB.println("Reading Humidity");
    //First dummy reading for analog-to-digital converter channel selection
    SensorGasv20.readValue(SENS_HUMIDITY);
    //Sensor temperature reading
    connectorBFloatValue = SensorGasv20.readValue(SENS_HUMIDITY);
    //Conversion into a string
    Utils.float2String(connectorBFloatValue, connectorBString, 2);
    USB.println("Reading CO2");
    //Configure and turn on the CO2 sensor  
    //First dummy reading to set analog-to-digital channel
    SensorGasv20.readValue(SENS_CO2);
    connectorCFloatValue = SensorGasv20.readValue(SENS_CO2);  
    SensorGasv20.setSensorMode(SENS_OFF, SENS_CO2);        
    //Conversion into a string
    Utils.float2String(connectorCFloatValue, connectorCString, 2);
    USB.println("Reading NO2");
    // Configuring NO2 sensor
    SensorGasv20.configureSensor(SENS_SOCKET3B, 1, 2);
    delay(10);
    //First dummy reading to set analog-to-digital channel
    SensorGasv20.readValue(SENS_SOCKET3B);
    connectorDFloatValue = SensorGasv20.readValue(SENS_SOCKET3B);    
    //Conversion into a string
    Utils.float2String(connectorDFloatValue, connectorDString, 2);
    USB.println("Reading Ozone");
    //First dummy reading to set analog-to-digital channel
    SensorGasv20.readValue(SENS_SOCKET2B);
    connectorEFloatValue = SensorGasv20.readValue(SENS_SOCKET2B);    
    //Conversion into a string
    Utils.float2String(connectorEFloatValue, connectorEString, 2); 
}

void shutdown_sensors(void) {
 // Step 11. Turn off the sensors

    SensorGasv20.setSensorMode(SENS_OFF, SENS_SOCKET3B);

    SensorGasv20.setSensorMode(SENS_OFF, SENS_SOCKET2B);
}

char messagetogw[256];
int i = 0;
void loop()
{
  /*
  Configure_Sensors();
  Read_Sensors();
  shutdown_sensors();
  */
  USB.println("ready to send");
  // Step 12. Message composition
  //Data payload composition
  /*
   *
  sprintf(data,"a:%s,b:%s,f:%s,c:%s,d:%s,e:%s",
  connectorAString,
  connectorBString,
  connectorCString,
  connectorDString,
  connectorEString,
  connectorFString);
  *
  */
  sprintf(data, "a:1,b:2,c:3,d:4");
// Step 13. Communication module to ON
  sprintf(messagetogw, "AT+SEND=%s\n", data);
  for(i=0; i < 15; i++) {
    lora_setup();
    W232.send(messagetogw);
    receive_from_mdot();
// Step 14. Message transmission
    USB.printf("send %d %s\r\n", i, data);

    USB.print(F("Battery Level: "));
    USB.print(PWR.getBatteryLevel(),DEC);
    USB.print(F(" %\n"));
    delay(5000); // 3 second delay and then the mdot seems to take around 30 seconds to respond even after, 
  }
  // Step 16. Entering Sleep Mode
  //USB.OFF();
  //PWR.deepSleep(sleepTime,RTC_OFFSET,RTC_ALM1_MODE2,ALL_OFF);
  //USB.ON();
  delay(10000);
  USB.print("turned back on\r\n");
}
