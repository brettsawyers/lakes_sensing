#include "mbed.h"
#include "mDot.h"
#include <string>
#include <vector>
#include <algorithm>
#include "ChannelPlans.h"

#define PARAMSNUM 9
#define PARAMLENGTH 15
#define SONDEBUFFERLEN 256
char sonde_buffer[SONDEBUFFERLEN];
char parameters[PARAMSNUM][PARAMLENGTH];

lora::ChannelPlan* plan = NULL;
mDot* dot = NULL;

//static std::string config_network_name = "UQ_St_Lucia";
//static std::string config_network_pass = "L0raStLucia";
static uint8_t devAddr[] = { 0x26, 0x01, 0x12, 0xB7 };
static uint8_t appSKey[] = { 0x10, 0xD8, 0x7A, 0xCC, 0x0E, 0x39, 0x7F, 0x6D, 0x7A, 0xD9, 0x9F, 0xD5, 0x68, 0xFF, 0xF0, 0x11 };
static uint8_t nwkSKey[] = { 0x42, 0xCC, 0x3A, 0x3A, 0xBC, 0x17, 0x50, 0xC7, 0xDE, 0x0D, 0x16, 0x55, 0x37, 0x3C, 0xE0, 0x8B };

static uint8_t config_frequency_sub_band = 2;

const char firstidentifier = 'h';

int32_t ret;

Serial debugger(USBTX, USBRX); //will need to change this to the appropriate pins once connected ot the exosonde
Serial device(UART_TX,UART_RX);

//update this depending on the desired readings' identifier characters
char identifiers[PARAMSNUM];
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
    int commarecvd = 0;
    while(datachar = device -> getc()){
        if(datachar == ',') commarecvd++;
        sonde_buffer[charcount] = datachar;
        charcount++;
        if(commarecvd >= 9) break;
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
    for(int i = 0; i < 12; i++) {
        wait(0.05);
        //send eight times to ensure a message goes out on channel 917.4 Mhz
        if ((ret = dot->send(payload)) != mDot::MDOT_OK) {
            debugger.printf("\r\nFailed send, code: %s\r\n",mDot::getReturnCodeString(ret).c_str());
        }else {
            debugger.printf("\r\ndata sent\r\n");
        }        
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
    if ((ret = dot->setTxDataRate(mDot::DR3)) != mDot::MDOT_OK) {
        debugger.printf("Could not set data rate\r\n");
    }
    
}

/*
 * initilaise the identifier array used when sending data
 */
void setup_identifiers() {
    for(int i = 0; i < PARAMSNUM; i++) {
        identifiers[i] = firstidentifier + i;    
    }
}
int main() {
    // get an mDot handle
    wait(2); //stabilise
    device.printf("\r\n");//flush any buffer built up during start up
    plan = new lora::ChannelPlan_AU915();
    dot = mDot::getInstance(plan);
    assert(dot);
    setup_identifiers();
    Loraconfig();
    debugger.printf("data rate = %d\r\n", dot -> getTxDataRate());
    debugger.printf("max packet length = %d\r\n", dot -> getMaxPacketLength());
    while(1){
        wait(2);
        getsondedata(&device, &debugger);
        if(!checkforcomma(&debugger)){
            setcommadelim(&device);
            continue;    
        }
        parsesondedata();
        sendpacket();
    }
}

