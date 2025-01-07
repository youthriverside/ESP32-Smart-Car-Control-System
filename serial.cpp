#include "serial.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>

// 串口句柄
HANDLE hSerial;
using namespace std;
bool initSerialPort(const wstring& portName, DWORD baudRate) {
    hSerial = CreateFile(portName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,    // 不共享
        NULL, // 默认安全属性
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        cerr << "Error opening serial port." << endl;
        return false;
    }

    // 设置波特率、数据位、停止位等
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    dcbSerialParams.BaudRate = baudRate; // 使用传入的波特率
    dcbSerialParams.ByteSize = 8;          // 8个数据位
    dcbSerialParams.StopBits = ONESTOPBIT; // 1个停止位
    dcbSerialParams.Parity = NOPARITY;     // 无奇偶校验

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error setting serial port parameters." << endl;
        CloseHandle(hSerial);
        return false;
    }

    return true;
}

void sendCommandToESP32(const char* command) {
    if (hSerial == INVALID_HANDLE_VALUE) {
        cerr << "Serial port not initialized." << endl;
        return;
    }

    DWORD bytesWritten;
    WriteFile(hSerial, command, strlen(command), &bytesWritten, NULL);
    // 发送命令后加一个换行符
    WriteFile(hSerial, "\n", 1, &bytesWritten, NULL);
}
void sendHexCommandToESP32(const int* command) {
     if (hSerial == INVALID_HANDLE_VALUE) {
        cerr << "Serial port not initialized." << endl;
        return;
    }

    DWORD bytesWritten;
	for(int i=0;i<6;i++){
		WriteFile(hSerial, command+i, 1, &bytesWritten, NULL);}
    // 发送命令后无回车符
}
void closeSerialPort() {
    CloseHandle(hSerial);
}

bool receiveDataFromESP32(string& receivedData) {
    char buffer[100] = "\0", ch = '\0';
    DWORD bytesRead = 0;
	DWORD total = 0;
	while(1){
		if (ReadFile(hSerial, &ch, 1, &bytesRead, NULL) && bytesRead > 0) {
			if(ch == '\r' || ch == '\n') {break;}
			buffer[total++] = ch;
		}
		else if(ReadFile(hSerial, &ch, 1, &bytesRead, NULL) || bytesRead == 0)
			return false;
	}
	buffer[total] = '\0';
	receivedData = buffer;
	for(int i = 0; buffer[i] != '\0'; i++)
		buffer[i] = '\0';
	return true;
}
string receivedData;
void processSerialData(string& carStatus, double& PWM_L, double& PWM_R, double& Speed_L, double& Speed_R) {
    if (receiveDataFromESP32(receivedData)) {
        //左轮转速
        if (receivedData.find("PWM_L = ") != string::npos) {
            size_t start = receivedData.find('=', receivedData.find("PWM_L = ")) + 1;
            size_t end = receivedData.find(',', start);
            string frequencyStr = receivedData.substr(start, end - start);
            PWM_L = atof(frequencyStr.c_str());
        }

        // 提取左轮速度
        if (receivedData.find("Speed_L = ") != string::npos) {
            size_t start = receivedData.find('=', receivedData.find("Speed_L = ")) + 1;
            size_t end = receivedData.find(',', start);
            string frequencyStr = receivedData.substr(start, end - start);
            Speed_L = atof(frequencyStr.c_str());
        }

        //右轮转速
        if (receivedData.find("PWM_R = ") != string::npos) {
            size_t start = receivedData.find('=', receivedData.find("PWM_R = ")) + 1;
            size_t end = receivedData.find(',', start);
            string frequencyStr = receivedData.substr(start, end - start);
            PWM_R = atof(frequencyStr.c_str());
        }

        //右轮速度
        if (receivedData.find("Speed_R = ") != string::npos) {
            size_t start = receivedData.find('=', receivedData.find("Speed_R = ")) + 1;
            size_t end = receivedData.find(',', start);
            string frequencyStr = receivedData.substr(start, end - start);
            Speed_R = atof(frequencyStr.c_str());
        }
        // 更新小车状态
        if (PWM_L != 0 && PWM_R != 0 && receivedData.find("FORWARD") != string::npos && receivedData.find("BACKWARD") == string::npos) {
            carStatus = "前进";
        } else if (PWM_L != 0 && PWM_R != 0 && receivedData.find("FORWARD") == string::npos && receivedData.find("BACKWARD") != string::npos) {
            carStatus = "后退";
        } else if ( (PWM_L == 0 && PWM_R != 0) ||  receivedData.find("PWM_L = 255, Speed_L = 38, FORWARD ; PWM_R = 255, Speed_R = 38, BACKWARD") != string::npos){
            carStatus = "左转";
        } else if (PWM_R == 0 && PWM_L != 0 || receivedData.find("PWM_L = 255, Speed_L = 38, BACKWARD ; PWM_R = 255, Speed_R = 38, FORWARD") != string::npos) {
            carStatus = "右转";
        } else if ((PWM_L == 0 && PWM_R == 0) || receivedData.find("STOP") != string::npos) {
            carStatus = "停止";
        }
    }
}
