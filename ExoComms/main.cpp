#include "mbed.h"
#include "mDot.h"
#include <string>
#include <vector>
#include <algorithm>
#include "ChannelPlans.h"

#define PARAMSNUM 10
#define PARAMLENGTH 15
#define SONDEBUFFERLEN 256
char sonde_buffer[SONDEBUFFERLEN];
char parameters[PARAMSNUM][PARAMLENGTH];

lora::ChannelPlan* plan = NULL;
mDot* dot = NULL;

static std::string config_network_name = "UQ_St_Lucia";
static std::string config_network_pass = "L0raStLucia";
static uint8_t config_frequency_sub_band = 7;

int32_t ret;

Serial device(USBTX, USBRX); //will need to change this to the appropriate pins once connected ot the exosonde
Serial debugger(USBTX,USBRX);

//update this depending on the desired readings' identifier characters
const char identifiers[PARAMSNUM] = {'a','b','c','d','e','f','g','h','i','j'};
void flushRXbuffer(Serial *serial){
    while(serial -> readable()) serial -> getc();    
}
 
bool checkforcomma(Serial *debugger){
    debugger -> printf("received: %s\r\n",sonde_buffer);
    for(int i = 0; i < strlen(sonde_buffer); i++) {
        if(sonde_buffer[i] == ','){
            return true;       
        }       
    } 
    return false;      
}
 
void getsondedata(Serial *device, Serial *debugger){
    char datachar;
    device -> printf("data\r\n");
    int charcount = 0;
    while(datachar = device -> getc(),datachar != '\r' && datachar != '\n'){
        sonde_buffer[charcount] = datachar;
        charcount++; 
    }
    //flush the remaining '\n' character from the buffer
    flushRXbuffer(device);
    sonde_buffer[charcount] = '\0';
}

void setcommadelim(Serial *device){
    device -> printf("delim 2\r\n");       
}

void parsesondedata(void){
    int parametercount = 0;
    int charcount;
    char currentcheck;
    for (int i = 0; i < strlen(sonde_buffer); i++){
        currentcheck = sonde_buffer[i];
        if(currentcheck == ',') {
            parameters[parametercount][charcount] = '\0';
            parametercount++;
            charcount = 0;
            continue;    
        }
        parameters[parametercount][charcount] = sonde_buffer[i];
        charcount++;
    }
    parameters[parametercount][charcount] = '\0'; //end the final string
}

void joinnetwork(void){
    debugger.printf("joining network\r\n");
    if ((ret = dot->joinNetwork()) != mDot::MDOT_OK) {
        debugger.printf("\r\n---------\r\nJoin Failed, code: %s\r\n---------\r\n",mDot::getReturnCodeString(ret).c_str());
    }
    else{
        debugger.printf("joined successfully\r\n");
    }    
}

void sendpacket(){
    std::vector<uint8_t> payload; 
    std::vector<uint8_t> dummypayload;      
    std::string dummystring("hellotherem");
    for (int i = 0; i < PARAMSNUM; i++){
        int j = 0;
        payload.push_back((uint8_t)identifiers[i]);
        payload.push_back((uint8_t)':');
        while(parameters[i][j] != '\0'){
            payload.push_back(parameters[i][j]);
            j++;              
        }
        if(i != PARAMSNUM-1){
            payload.push_back((uint8_t)',');    
        }
    }
    payload.push_back((uint8_t)'\0');
    debugger.printf("made packet %s\r\n", payload);
    // join the network if not joined
    while (!dot->getNetworkJoinStatus()) {
        joinnetwork();
    }
    std::copy(dummystring.begin(),dummystring.end(),std::back_inserter(dummypayload));
    if (dot->getNetworkJoinStatus()) {
        // send the data
        // ACKs are enabled by default, so we're expecting to get one back
        if ((ret = dot->send(payload)) != mDot::MDOT_OK) {
            debugger.printf("\r\nFailed send, code: %s\r\n",mDot::getReturnCodeString(ret).c_str());
        } else {
            debugger.printf("\r\ndata sent\r\n");
        }
    }
    
}

void Loraconfig(void){
    if ((ret = dot->setFrequencySubBand(config_frequency_sub_band)) != mDot::MDOT_OK) {
        debugger.printf("Could not set FSB\r\n");
    }
    if ((ret = dot->setNetworkName(config_network_name)) != mDot::MDOT_OK) {
        debugger.printf("Could not set network name\r\n");
    }
    if ((ret = dot->setNetworkPassphrase(config_network_pass)) != mDot::MDOT_OK) {
        debugger.printf("Could not set network passcode\r\n");
    }
    //set the number of retries for each sub band before giving up
    if ((ret = dot->setJoinRetries(100)) != mDot::MDOT_OK) {
        debugger.printf("Could not set retries\r\n");
    }
    // request receive confirmation of packets from the gateway
    if ((ret = dot->setAck(1)) != mDot::MDOT_OK) {
        debugger.printf("Could not set ACK\r\n");
    }
    // set join mode 
    if ((ret = dot->setJoinMode(mDot::AUTO_OTA)) != mDot::MDOT_OK) {
        debugger.printf("Could not set join mode\r\n");
    } 
    //set data rate to dr2, this gives us 126 bytes of payload space
    if ((ret = dot->setTxDataRate(mDot::DR2)) != mDot::MDOT_OK) {
        debugger.printf("Could not set data rate\r\n");
    } 
    while(!dot->getNetworkJoinStatus()) {
        joinnetwork();
        if(!dot->getNetworkJoinStatus()) wait(5);
    } 
}

int main() {
    // get an mDot handle
    plan = new lora::ChannelPlan_AU915();
    dot = mDot::getInstance(plan);
    assert(dot);
    Loraconfig();
    debugger.printf("data rate= %d\r\n", dot -> getTxDataRate());
    debugger.printf("max packet length = %d\r\n", dot -> getMaxPacketLength());
    while(1){
        wait(0.5);
        getsondedata(&device, &debugger);
        if(!checkforcomma(&debugger)){
            setcommadelim(&device);
            continue;    
        }
        parsesondedata();
        sendpacket();
    }
}

