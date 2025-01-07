#ifndef SERIAL_H
#define SERIAL_H

#include <windows.h>
#include <string>

// 初始化指定的串口并返回成功与否
bool initSerialPort(const std::wstring& portName, DWORD baudRate);

// 发送命令到ESP32
void sendCommandToESP32(const char* command);
void sendHexCommandToESP32(const int* command);
// 接受命令
bool receiveDataFromESP32(std::string& receivedData);
// 
void processSerialData(std::string& carStatus,double& PWM_L,double& PWM_R,double& Speed_L,double& Speed_R);
// 关闭串口
void closeSerialPort();

#endif // SERIAL_H
