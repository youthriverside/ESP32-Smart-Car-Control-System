#include <iostream>
#include <cstdio>
#include <iomanip> 
#include <Windows.h>
#include <sstream> 
#include "serial.h"
using namespace std;
extern string receivedData;
void displayMenuAndStatus(const string& carStatus, double PWM_L, double PWM_R, double Speed_L, double Speed_R) {
	//显示菜单和小车状态
    system("cls");
    cout << "选择操作:\n";
    cout << "1. 前进\n";
    cout << "2. 后退\n";
    cout << "3. 左转\n";
    cout << "4. 右转\n";
    cout << "5. 快速左转\n";
    cout << "6. 快速右转\n";
    cout << "7. 设置速度（单轮/PWM)\n";
    cout << "8. 停止\n";
	cout << "9. 刷新状态\n";
    cout << "0. 退出\n\n";
    cout << "状态: " << carStatus << endl;
    cout << "左轮PWM: " << PWM_L << endl;
	cout << "左轮速度: " << Speed_L << endl;
    cout << "右轮PWM: " << PWM_R << endl;
    cout << "右轮速度: " << Speed_R << endl;
	cout << "接收数据：" << receivedData <<endl;  
    cout << "请输入选项: ";
}
int calculateChecksum(int* hexArray, int length) {
	// 计算异或校验和
    int checksum = 0;
    for (int i = 0; i < length; ++i) {
        checksum ^= hexArray[i];
    }
    stringstream ss;
	//使用字符串流操作数据帧
    ss << hex << checksum;
    int hexChecksum;
    ss >> hex >> hexChecksum;  
    return hexChecksum;
}
void decimalToHexArray(int decimal, int* hexArray, int index) {
	// 进制转换，十进制转为十六进制，并添加前导0
    stringstream ss;
    ss << hex << setfill('0') << setw(2) << decimal;
    int hexValue;
    ss >> hex >> hexValue;  
    hexArray[index] = hexValue;
}
int main() {
    wstring comPort;
    cout << "请输入COM端口（如COM3）：";
    getline(wcin, comPort);

    int baudRate;
    cout << "请输入波特率（如9600）：";
    cin >> baudRate;

    if (!initSerialPort(comPort, baudRate)) {
        cerr << "初始化串口失败!\n";
        return 1;
    }
    string carStatus = "未知";
	
    double PWM_L = 0.0, PWM_R = 0.0, Speed_L = 0.0, Speed_R = 0.0;
	//初始化小车状态
    displayMenuAndStatus(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
    while (true) {
		res:
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		//处理串口回传数据
        displayMenuAndStatus(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		//菜单与小车状态显示
        int choice;
        cin >> choice;
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
		processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
        switch (choice) {
            case 1:
                sendCommandToESP32("FORWARD");
                break;
            case 2:
                sendCommandToESP32("BACKWARD");
                break;
            case 3:
                sendCommandToESP32("LEFT");
                break;
            case 4:
                sendCommandToESP32("RIGHT");
                break;
            case 5: {
                int hexValue[] = {0xAA, 0x01, 0xFF, 0x00, 0xFF, 0xAB};
                sendHexCommandToESP32(hexValue);
                break;
            }
            case 6: {
                int hexValue[] = {0xAA, 0x00, 0xFF, 0x01, 0xFF, 0xAB};
                sendHexCommandToESP32(hexValue);
				//向串口发送数据帧
                break;
            }
            case 7: {
				//调速二级菜单
				int leftPWM, rightPWM, flag = 1, inp;
                int leftStatus, rightStatus;
				while(flag) {
					system("cls");
					cout << "调速菜单" <<endl;
					cout << "1. pwm双轮调速\n";
					cout << "2. 数据帧单轮调速\n";
					cout << "3. 返回上一级\n请选择序号：";
					cin >> inp;
					if(inp == 2){
						cout << "请输入左轮方向 1:正，0：反：";
						cin >> leftStatus;
						cout << "请输入左轮速度 (0-255): ";
						cin >> leftPWM;
						cout << "请输入右轮方向 1:正，0：反：";
						cin >> rightStatus;
						cout << "请输入右轮速度 (0-255): ";
						cin >> rightPWM;   
						//使用数据帧对左右轮分别调速
						int hexValue[6];
						hexValue[0] = 0XAA;
						hexValue[1] = (leftStatus == 1)? 0x01 : 0x00;  //左轮转动状态
						decimalToHexArray(leftPWM, hexValue, 2);       //左轮速度
						hexValue[3] = (rightStatus == 1)? 0x01 : 0x00; //右轮转动状态
						decimalToHexArray(rightPWM, hexValue, 4);      //右轮速度
						hexValue[5] = calculateChecksum(hexValue, 5);  //异或校验位					
						sendHexCommandToESP32(hexValue);
					}
					else if(inp == 1){
						int pwm;
						cout << "请输入速度:";
						cin >> pwm;
						if (pwm >= 0 && pwm <= 255) {
						char command[50];
						sprintf_s(command, sizeof(command), "PWM %d", pwm);
						sendCommandToESP32(command);
						} else {
						cout << "速度超出范围，请重新输入。\n";
						}
					}
					else if(inp == 3) flag = 0;
				}
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
					processSerialData(carStatus, PWM_L, PWM_R, Speed_L, Speed_R);
                break;
            }
			case 8:
                sendCommandToESP32("STOP");
                break;
			case 9: goto res; break;
			//刷新小车状态
            case 0:
                closeSerialPort(); 
				// 在退出前关闭串口
                return 0;
            default:
                cout << "无效的选项，请重新选择。\n";
                break;
        }
    }

    return 0;
}
