#include "mbed.h"
#include "mDot.h"
#include "MTSLog.h"
#include <string>
#include <vector>
#include <algorithm>

#define PARMSNUM 10
#define PARAMLENGTH 15
#define SONDEBUFFERLEN 256
char sonde_buffer[SONDEBUFFERLEN];
char parameters[PARAMSNUM][PARAMLENGTH];
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
        parameters[parametercount][charcount] = sonde_buffer;
        charcount++;
    }
    parameters[parametercount][charcount] = '\0';
    
}

int main() {
    Serial device(USBTX, USBRX); //will need to change this to the appropriate pins once connected ot the exosonde
    Serial debugger(USBTX,USBRX);
    while(1){
        wait(0.5);
        getsondedata(&device, &debugger);
        debugger.printf("sonde: %s\r\n",sonde_buffer); 
        if(!checkforcomma(&debugger)){
            setcommadelim(&device);
            continue;    
        }
        parsesondedata();
        
    }
}

