#include "mbed.h"
#include "mDot.h"
#include "MTSLog.h"
#include <string>
#include <vector>
#include <algorithm>

Serial device(USBTX, USBRX); //will need to change thistoi the appropriate pins once connected ot the exosonde
DigitalOut led1(LED1);
int main() {
        char datachar;
        char* recieved = (char *) malloc(sizeof(char) * 50);;
        int malloced = 50, charcount = 0;
        bool commaflag = false;
        while(datachar = device.getc(),datachar != '\r'){
            if(!((charcount+1)%malloced)){
                malloced = malloced*2;
                recieved = realloc(recieved, malloced);       
            } 
            recieved[charcount] = datachar;
            charcount++;  
        }
}