//
//  AllComponents.cpp
//  deneme.cpp
//
//  Created by Doruk Arslan on 5.07.2021.
//

#include "AllComponents.hpp"

#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> //open()
#include <stdio.h>
#include <termios.h>
#include <unistd.h> //write(), read(), close()
#include <errno.h> //errno






static const int SFD_UNAVAILABLE = -1;
static int sfd = SFD_UNAVAILABLE;



int IsReadyToStart(void);
int serialPortIsOpen(void);


int openAndConfigureSerialPort(const char* portPath, int baudRate) {
    
    
    //If port is already open, close it
    if (serialPortIsOpen()) {
        close(sfd);
    }
    
    //Open port, checking for errors
    sfd = open(portPath, (O_RDWR | O_NOCTTY | O_NDELAY));
    printf("open    sfd : %d   char : %s -- baud :  %d   \n",sfd,portPath,baudRate);
    
    if (sfd == -1) {
        printf("Unable to open serial port: %s at baud rate: %d\n", portPath, baudRate);
        return sfd;
    }
    
    struct termios options;
    tcgetattr(sfd, &options);
    switch (baudRate) {
        case 9600:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            break;
        case 19200:
            cfsetispeed(&options, B19200);
            cfsetospeed(&options,B19200);
            break;
        case 38400:
            cfsetispeed(&options, B38400);
            cfsetospeed(&options, B38400);
            break;
        case 57600:
            cfsetispeed(&options, B57600);
            cfsetospeed(&options, B57600);
            break;
        default:
            printf("Requested baud rate %d not currently supported. Defaulting to 9,600.\n", baudRate);
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            break;
    }
    
    
    options.c_iflag &= ~(INLCR | ICRNL);
    options.c_iflag |= IGNPAR | IGNBRK;
    options.c_oflag &= ~(OPOST | ONLCR | OCRNL);
    options.c_cflag &= ~(PARENB | PARODD | CSTOPB | CSIZE | CRTSCTS);
    options.c_cflag |= CLOCAL | CREAD | CS8;
    options.c_lflag &= ~(ICANON | ISIG | ECHO);
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN]  = 0;
    
    
    if (tcsetattr(sfd, TCSANOW, &options) < 0) {
        printf("Error setting serial port attributes.\n");
        close(sfd);
        return -2;
    }
    
    return sfd;
}




int serialPortIsOpen() {
    if(sfd == SFD_UNAVAILABLE){
        return -1 ;
    }
    else {
        return 1;
    }
    
}



ssize_t writeSerialData(const char* bytesToWrite, size_t numBytesToWrite) {
   
    printf("%s \n",bytesToWrite);
    ssize_t numBytesWritten = write(sfd, bytesToWrite, numBytesToWrite);
 //   printf("Inside write sfd  %d \n",sfd);
    
    if (numBytesWritten < 0) {
        printf("Serial port write() failed. Error: %s", strerror(errno));
    }
    
    return numBytesWritten;
}



ssize_t readSerialData(char* const rxBuffer, size_t numBytesToReceive) {
    
    ssize_t numBytesRead = read(sfd, rxBuffer, numBytesToReceive);
    if (numBytesRead < 0) {
        
        printf("Serial port read() failed. %d Error:%s   s", sfd,strerror(errno));
    }
    
    return numBytesRead;
}

ssize_t closeSerialPort(void) {
    ssize_t result = 0;
    if (serialPortIsOpen()) {
        result = close(sfd);
        sfd = SFD_UNAVAILABLE;
    }
    return result;
}



int getSerialFileDescriptor(void) {
    return sfd;
}



int firstControl(char*  rxBuffer){
   int  result = 1;
    char welcome[10] = "Welcome";
    
    
    for(int i=0;i<sizeof(rxBuffer); i++){
        if(rxBuffer[i] == 'W'){
            result = 1;
            for (int j=0; j<strlen(welcome); j++) {
                if(welcome[j] != rxBuffer[j+i]){
                    
                    result = 0;
            }
            }
            break;
        }
    }
    
    
    
    
   

    if (result != 0) {
        printf("Condition satisfied recieved data : %s  \n",rxBuffer);
        return 1;
    }
    else {
        printf("Invalid data : %s \n",rxBuffer);
        return 0;
    }
}

int IsReadyToStart(){
    char firstData[10];
    readSerialData(firstData, sizeof(firstData));
    
    if(!strcmp(firstData, "Ready")){
        
        return 1;
    }
    else {
        return 0;
        
    }
    
    
}

void clear()
{

    tcflush(sfd, TCIFLUSH);
    tcflush(sfd, TCOFLUSH);
}

