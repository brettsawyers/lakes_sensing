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

//static std::string config_network_name = "UQ_St_Lucia";
//static std::string config_network_pass = "L0raStLucia";
static uint8_t devAddr[] = { 0x26, 0x04, 0x1E, 0xDB };
static uint8_t appSKey[] = {0x2B, 0x2A, 0xDC, 0x26, 0xE2, 0x6F, 0x99, 0x75, 0xCF, 0xBC, 0xA6, 0x20, 0x68, 0xFF, 0xCF, 0xCE};
static uint8_t nwkSKey[] = {0x17, 0x01, 0x0D, 0x95, 0x9C, 0x72, 0x9F, 0x84, 0x5C, 0x03, 0x1A, 0xBB, 0xAC, 0xF1, 0x33, 0x11};       
static uint8_t config_frequency_sub_band = 2;


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
    std::string dummystring("hello");
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
    std::copy(dummystring.begin(),dummystring.end(),std::back_inserter(dummypayload));
    debugger.printf("%d\r\n", dummypayload.size());
    // send the data
    if ((ret = dot->send(dummypayload)) != mDot::MDOT_OK) {
        debugger.printf("\r\nFailed send, code: %s\r\n",mDot::getReturnCodeString(ret).c_str());
    } else {
        debugger.printf("\r\ndata sent\r\n");
    }

    
}

void Loraconfig(void){
    debugger.printf("default frequency band %d\n\r",dot -> getDefaultFrequencyBand());
    if ((ret = dot->setFrequencySubBand(config_frequency_sub_band)) != mDot::MDOT_OK) {
        debugger.printf("Could not set FSB\r\n");
    }
    
    std::vector<uint8_t> temp;
    for (int i = 0; i < 4; i++) {
        temp.push_back(devAddr[i]);    
    }
    // set join mode 
    if ((ret = dot->setJoinMode(mDot::MANUAL)) != mDot::MDOT_OK) {
        debugger.printf("Could not set join mode\r\n");
    } 
    //set network address
    if ((ret = dot->setNetworkAddress(temp)) != mDot::MDOT_OK) {
        debugger.printf("Could not set network address\r\n");
    }
    //ser public netowkr to true
    if ((ret = dot->setPublicNetwork(true)) != mDot::MDOT_OK) {
        debugger.printf("Could not set public network\r\n");
    }
    //make a string to be input later
    temp.clear();
    for (int i = 0; i < 16; i++) {
        temp.push_back(nwkSKey[i]);    
    }
    //set the netowrk session key
    if ((ret = dot->setNetworkSessionKey(temp)) != mDot::MDOT_OK) {
        debugger.printf("Could not set network session key\r\n");
    }
    
    //make a string to be input later
    temp.clear();
    for (int i = 0; i < 16; i++) {
        temp.push_back(appSKey[i]);    
    }
    
    if ((ret = dot->setDataSessionKey(temp)) != mDot::MDOT_OK) {
        debugger.printf("Could not set data session key (app session key)\n\r");    
    }  
        
    // request receive confirmation of packets from the gateway
    if ((ret = dot->setAck(0)) != mDot::MDOT_OK) {
        debugger.printf("Could not set ACK\r\n");
    }

    //set data rate to dr2, this gives us 126 bytes of payload space
    if ((ret = dot->setTxDataRate(mDot::DR0)) != mDot::MDOT_OK) {
        debugger.printf("Could not set data rate\r\n");
    }
    
}

int main() {
    // get an mDot handle
    plan = new lora::ChannelPlan_US915();
    dot = mDot::getInstance(plan);
    assert(dot);
    Loraconfig();
    debugger.printf("data rate= %d\r\n", dot -> getTxDataRate());
    debugger.printf("max packet length = %d\r\n", dot -> getMaxPacketLength());
    while(1){
        wait(0.05);
        /*
        getsondedata(&device, &debugger);
        if(!checkforcomma(&debugger)){
            setcommadelim(&device);
            continue;    
        }
        parsesondedata();
        */
        sendpacket();
    }
}

