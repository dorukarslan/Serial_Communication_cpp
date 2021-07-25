//
//  main.cpp
//  deneme.cpp
//
//  Created by Doruk Arslan on 5.07.2021.
//

#include <iostream>
#include "AllComponents.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> //open()
#include <stdio.h>
#include <termios.h>
#include <unistd.h> //write(), read(), close()
#include <errno.h> //errno
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <thread>
#include <csignal>










time_t currentR ,endR = 0;
time_t currentS ,endS = 0;
time_t currentW ,endW = 0;



void is_Data_Received(void);
int WaitingForMessage(void);
int WelcomeCheck(void);
int ReadyCheck(void);
bool RXCheck(std::string Str);
void PrintRx (void);
bool bufferCheck(char *Buff, char * String);
char rxBuf[40];




typedef enum{
    READY, WAITING_WELCOME, WELCOME_RECIEVED,
} CONDITION;

CONDITION currentCase = READY;




int SignalCount = 0;
bool SignalBool = false;
bool MessageComing = false;
bool ReadyMessage = false;


void ReadPeriodic(){
    
    
    puts("Read thread \n");
    
    time_t inteded = 2.00 ; // 2 second
    
    if(0 == endR){
        endR = currentR;
    }
    
    
    while (1) {
        currentR = time(NULL);
        
        time_t diffR = difftime(currentR, endR);
        
        if( diffR >= inteded){
            char toCheckReady[] = "Ready";
            if(bufferCheck(rxBuf, toCheckReady) && currentCase!=  WAITING_WELCOME){
                currentCase = READY;
            }
            
    //        printf("current case inside read  %d  rx : %s \n",currentCase,rxBuf);
//
            
            endR = time(NULL);
            
            ssize_t isMessageValid = readSerialData(rxBuf, sizeof(rxBuf));
            printf("Suanda rx buf %s \n",rxBuf);
            if(isMessageValid > 0){
                
                if (RXCheck("Ready")) {
                    ReadyMessage = true;
                }
                
                
                MessageComing = true;
                PrintRx();
                if(RXCheck("UI begin")){
                    printf("########################################################\n");
                    printf("#                                                      #\n");
                    printf("#   Type 'On' to turn on, 'Off' to turn of the led     #\n");
                    printf("#      or Type 'Signal' to see it's effect             #\n");
                    printf("#                                                      #\n");
                    printf("########################################################\n");
                    
                }
                
                memset(rxBuf, 0, strlen(rxBuf));
                clear();
                
            }
            else{
            //    printf("Message invalid \n");
                MessageComing = false;
            }
            
        }
    }
}

void WritePeriodic(){
    
    time_t inteded = 2.00 ; // 2 second
    
    if(0 == endW){
        endW = currentW;
    }
    
    
    while (1) {
        currentW = time(NULL);
        
        time_t diffW = difftime(currentW, endW);
        
        if( diffW >= inteded){
            
            
      //      puts("Inside Write thread \n");
            
            endW = time(NULL);
            
            
            if(MessageComing && !RXCheck("Ready")){
                
                char ArrayToWritten[30]  = "I don't know latin";
       //         puts(ArrayToWritten);
                writeSerialData(ArrayToWritten, strlen(ArrayToWritten));

                
            }

        }
    }
    
}


bool bufferCheck(char *Buff, char * String){
    bool check = false
    ;
    for(int i=0;i<sizeof(Buff);i++){
        
        if(Buff[i] == String[0]){
            check = true;
            for (int j=0; j<strlen(String); j++) {
                printf("%c- ",Buff[i+j]);
                if(Buff[i+j] != String[j]){
                    
                    check = false;
                }
            }
            printf("\n");
            
            

            break;
           
            
        }
        
    }
    if(strcmp(Buff, String) == 0){
        check = true;
    }
  //  printf("Buffer check girdi %s %s sonuc : %d",Buff,String,check);
    return check;
}


bool RXCheck(std::string Str){
    bool check = false;

    std::string s1 = rxBuf;
    std::string s2 = Str;
    if(s1.find(s2) != std::string::npos) {
        check = true;
}
    
    return check;
}



int count = 0;
void PrintRx (){
    if (MessageComing) {
        count ++;
        
        // In order to get rid off unecessary prints during led toggling
        if(!RXCheck("On") && !RXCheck("Off")){
            puts("Message recieved from stm32 :   ");
             printf("%s %d  \n" ,rxBuf,count);
        }
        MessageComing = false;
    }
    
}




int WaitingForMessage(){
    
    readSerialData(rxBuf, sizeof(rxBuf));
    int compare = 0;
    std::string ReadyArr = "Ready";
   
 //   printf("Girdi temp :  %s  \n",tempBuf);
    
    if (RXCheck(ReadyArr)) {
        compare = 1;
        ReadyMessage = true;
    }
   
    
   
    return compare ;
}
void signalHandler( int signum ) {
    std::cout << " signal (" << signum << ") received.\n";
    SignalBool = true;
    SignalCount ++;
    if(SignalCount>=4){
        exit(signum);
    }
    printf("########################################################\n");
    printf("#                                                      #\n");
    printf("#   Type 'On' to turn on, 'Off' to turn of the led     #\n");
    printf("#      or Type 'Signal' to see it's effect             #\n");
    printf("#                                                      #\n");
    printf("########################################################\n");



}




void StateMachineThread(){
    
    
    
    time_t inteded = 2.00 ; // 2 second
    bool loopCheck = false;
    if(0 == endS){
        endS = currentS;
    }
    
    int caseResult;
    while (1) {
        currentS = time(NULL);
        
        time_t diffS = difftime(currentS, endS);
        
        if( diffS >= inteded){
            endS = time(NULL);
            
            
            
     //       printf("Current case type %u \n", currentCase);
            
            
            
            switch (currentCase) {
                case READY:
                    caseResult = ReadyCheck();
                    if(1 == caseResult){
                        
                        ReadyMessage = false;
                        currentCase = WAITING_WELCOME;
                    }
                    break;
                case WAITING_WELCOME:
                    caseResult = WelcomeCheck();
                    if(1 == caseResult){
                        currentCase = WELCOME_RECIEVED;
                    }
                    break;
                    
                case WELCOME_RECIEVED:
                    
                    
                    
                    char LoopContArr[30];
                    readSerialData(LoopContArr, sizeof(LoopContArr));
                    char  hReady[10] = "Ready";
                    loopCheck = bufferCheck(LoopContArr,hReady);
              //      printf("Case ici rx : %s \n",rxBuf);

                    if (!loopCheck) {
                        char UItransmit[] = "UI begin";
                        if (bufferCheck(rxBuf, UItransmit)) {
                      //      printf("içeride miyiz ? \n  ");
                            strcat(UItransmit, "2");
                            
                            writeSerialData(UItransmit, strlen(UItransmit));
                            
                        }
                        char UserInput[20];
                        std::cin >>UserInput;
                        if (strcmp(UserInput, "On") == 0) {
                            writeSerialData(UserInput, strlen(UserInput));
                            clear();
                            
                            memset( UserInput, 0, strlen(UserInput));
                            clear();

                            readSerialData(UserInput, sizeof(UserInput));
                            
                            
                        }
                        else if(strcmp(UserInput, "Off") == 0) {
                            printf("Led turned off \n");
                            clear();
                            writeSerialData(UserInput, strlen(UserInput));
                           
                        }
                        else if (!strcmp(UserInput, "Signal")) {
                            signal(SIGINT, signalHandler);
                            char signal[]= "Signal";
                            writeSerialData(signal, strlen(signal));
                            printf("type a message  : \n");
                            char  message[30];
                            std::cin >>message;
                            clear();
                            while (!SignalBool) {
                                printf("%s \n",message);
                                std::this_thread::sleep_for( std::chrono::milliseconds(50));
                            }
                            printf("\n");
                            SignalBool = false;
                            
                        }
                        else if (!strcmp(UserInput,"Flash"))
                        {
                            printf("Flash geldi \n");
                        }
                        


                        else {
                            printf("Invalid command! \n");
                        }
                       memset( UserInput, 0, strlen(UserInput));
                       
                       readSerialData(UserInput, sizeof(UserInput));
                        
                        
              //          std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ));

                    }
                   
                    
                    
                    break;
            }
           
            
            
        }
        
        
    }
    
    
    
    
}



int ReadyCheck(){
    
    
    int  result = WaitingForMessage();
    
    
    if(1 == result && ReadyMessage){
        char sendReady[20]  = "Ready2";
        writeSerialData(sendReady, strlen(sendReady));
        std::this_thread::sleep_for( std::chrono::milliseconds(100));

        ReadyMessage = false;
        return 1;
    }
    else{
        return 0;
    }
    clear();
}
int WelcomeCheck(){
    if(rxBuf[0] == '\0' ){
        readSerialData(rxBuf,sizeof(rxBuf));
        
        
     //   printf("Total received :  %s  \n",rxBuf);
        
        
        
        int isValid = firstControl(rxBuf);
        
        if(isValid == 1){
            strcpy(rxBuf, "Welcome");
        //    printf("Valid version %s \n",rxBuf);
            char txBuffer[30];
            strcpy(txBuffer, rxBuf);
            strcat(txBuffer, " received from stm32");
            writeSerialData(txBuffer,strlen(txBuffer));
            return 1;
            
        }
        else {
            printf("Data invalid, terminating process \n");
            memset(rxBuf, 0, strlen(rxBuf));
            writeSerialData("Error", strlen("Error"));
            return 0;
        }
        
    }
    else{
        return 0;
    }
}










int main (int argc, const char * argv[]) {
  

    
    
    const char devicePathStr[] = "/dev/tty.usbserial-14130";
    const int baudRate = 9600;
    printf("Trying to open serial \n");
    int sfd = openAndConfigureSerialPort(devicePathStr, baudRate);
    
    
    if (sfd < 0) {
        if (sfd == -1) {
            printf("Unable to connect to serial port.\n");
        }
        else { //sfd == -2
            printf("Error setting serial port attributes.\n");
        }
    }
    
    
    std::thread StateThread(StateMachineThread);
    std::thread ReadThread (ReadPeriodic);
    std::thread WriteThread(WritePeriodic);
    StateThread.join();
    ReadThread.join();
    WriteThread.join();
    
    
    
    
    
}
