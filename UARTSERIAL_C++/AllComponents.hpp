//
//  AllComponents.hpp
//  deneme.cpp
//
//  Created by Doruk Arslan on 5.07.2021.
//

#ifndef AllComponents_hpp
#define AllComponents_hpp

#include <stdio.h>

int openAndConfigureSerialPort(const char* portPath, int baudRate);
int serialPortIsOpen(void);
ssize_t writeSerialData(const char* bytesToWrite, size_t numBytesToWrite);
ssize_t readSerialData(char* const rxBuffer, size_t numBytesToReceive);
ssize_t closeSerialPort(void);
int getSerialFileDescriptor(void);
int firstControl(char* const rxBuffer);
int IsReadyToStart(void);
void clear(void);
#endif /* AllComponents_hpp */
//
//  AllComponents.hpp
//  deneme.cpp
//
//  Created by Doruk Arslan on 5.07.2021.
//

#ifndef AllComponents_hpp
#define AllComponents_hpp

#include <stdio.h>

int openAndConfigureSerialPort(const char* portPath, int baudRate);
int serialPortIsOpen(void);
ssize_t writeSerialData(const char* bytesToWrite, size_t numBytesToWrite);
ssize_t readSerialData(char* const rxBuffer, size_t numBytesToReceive);
ssize_t closeSerialPort(void);
int getSerialFileDescriptor(void);
int firstControl(char* const rxBuffer);
int IsReadyToStart(void);
void clear(void);
#endif /* AllComponents_hpp */
