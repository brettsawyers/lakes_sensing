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

long  sequenceNumber = 0;   
                                               
char  nodeID[10] = "403472985";   

char* sleepTime = "00:00:01:00";           

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


void setup() 
{

// Step 3. Communication module initialization

// Step 4. Communication module to ON

    USB.ON();

// Step 5. Initial message composition

    sprintf(data,"Hello, this is Waspmote Plug & Sense!\r\n");

// Step 6. Initial message transmission

    USB.println(data);

// Step 7. Communication module to OFF

    USB.OFF();


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

    delay(10000);

    //Turning on NO2 Sensor
    SensorGasv20.setSensorMode(SENS_ON, SENS_SOCKET3B);
    delay(30000);

    //Turning on and configuring O3 Sensor
    SensorGasv20.setSensorMode(SENS_ON, SENS_SOCKET2B);
    SensorGasv20.configureSensor(SENS_SOCKET2B, 1, 10);
    delay(30000);
}

void Read_Sensors(void) {
 // Step 10. Read the sensors

    //First dummy reading for analog-to-digital converter channel selection
    SensorGasv20.readValue(SENS_TEMPERATURE);
    //Sensor temperature reading
    connectorAFloatValue = SensorGasv20.readValue(SENS_TEMPERATURE);
    //Conversion into a string
    Utils.float2String(connectorAFloatValue, connectorAString, 2);

    //First dummy reading for analog-to-digital converter channel selection
    SensorGasv20.readValue(SENS_HUMIDITY);
    //Sensor temperature reading
    connectorBFloatValue = SensorGasv20.readValue(SENS_HUMIDITY);
    //Conversion into a string
    Utils.float2String(connectorBFloatValue, connectorBString, 2);

    //Configure and turn on the CO2 sensor  
    SensorGasv20.configureSensor(SENS_CO2, 7);
    SensorGasv20.setSensorMode(SENS_ON, SENS_CO2);    
    delay(30000);
    //First dummy reading to set analog-to-digital channel
    SensorGasv20.readValue(SENS_CO2);
    connectorCFloatValue = SensorGasv20.readValue(SENS_CO2);  
    SensorGasv20.setSensorMode(SENS_OFF, SENS_CO2);        
    //Conversion into a string
    Utils.float2String(connectorCFloatValue, connectorCString, 2);

    // Configuring NO2 sensor
    SensorGasv20.configureSensor(SENS_SOCKET3B, 1, 2);
    delay(10);
    //First dummy reading to set analog-to-digital channel
    SensorGasv20.readValue(SENS_SOCKET3B);
    connectorDFloatValue = SensorGasv20.readValue(SENS_SOCKET3B);    
    //Conversion into a string
    Utils.float2String(connectorDFloatValue, connectorDString, 2);

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

void lora_setup() {
  W232.ON(SOCKET0);
  delay(100);
  W232.parityBit(NONE);
  W232.stopBitConfig(1);
  W232.baudRateConfig(115200);
  delay(300);

  //connect to network here and all that jazz, look at saving in NVM
  
}
void loop()
{
  Configure_Sensors();
  Read_Sensors();
  shutdown_sensors();
// Step 12. Message composition

    //Data payload composition
  sprintf(data,"I:%s#N:%li#%s:%s#%s:%s#%s:%s#%s:%s#%s:%s#%s:%s\r\n",
	nodeID ,
	sequenceNumber,
	TIME_STAMP, RTC.getTimestamp(),
	CONNECTOR_A , connectorAString,
	CONNECTOR_B , connectorBString,
	CONNECTOR_C , connectorCString,
	CONNECTOR_D , connectorDString,
	CONNECTOR_E , connectorEString);
// Step 13. Communication module to ON
    USB.ON();
// Step 14. Message transmission
    USB.println(data);
// Step 15. Communication module to OFF
    USB.OFF();
// Step 16. Entering Sleep Mode
    PWR.deepSleep(sleepTime,RTC_OFFSET,RTC_ALM1_MODE1,ALL_OFF);
    //Increase the sequence number after wake up
    sequenceNumber++;


}
