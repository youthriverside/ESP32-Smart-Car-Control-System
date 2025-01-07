# ESP32 Smart Car Control System
## Introduction
The ESP32 Smart Car Control System is an innovative project that realizes hardware-software interaction between a computer and a microcontroller based on the C++ serial port module and the ESP32 microprocessor chip. This system mediates the transmission of string commands or corresponding data frames recognizable by the microcontroller through serial communication, enabling the car to move forward, backward, turn quickly, and adjust speed via PWM (Pulse Width Modulation). The system is easy to operate with a high fault tolerance rate, and it is equipped with an XOR bit self-identification, self-calculation, and self-verification module for data frames, reducing the probability of misidentifying commands. Additionally, this system can efficiently receive status data transmitted back from the ESP32 Smart Car, simplify the processing of the returned data, and clearly display it on the console, achieving precise control of the ESP32 Smart Car.  
## Features 
The design results of the ESP32 Smart Car Control System basically meet the expected requirements. The commands are sent accurately and successfully achieve operations such as moving forward, backward, turning, and PWM speed adjustment. The XOR checksum calculation for data frames is precise and efficient. The parsing of the car's status is basically correct, and it can clearly display the car's speed and direction on the console. The system exhibits excellent robustness, promptly reporting error messages for out-of-bound data, and possesses high memory safety. However, the system still faces occasional errors in car status discrimination and delays in receiving data transmitted back from the car, which are currently difficult to resolve. In the future, we will further improve the system's functionality by delving deeper into hardware logic and software algorithm knowledge to rewrite and refactor the code.

## Issues and Solutions
### The single-digit numbers in the data frame lack a leading "0" placeholder, making it unrecognizable by the car.
   Solution: By consulting literature, it was decided to use the stringstream class from the C++ standard library, analogous to the usage of the setfill() function in the iostream stream for homework assignments. Specifically, a string stream was used to add a leading "0" before single digits to achieve precise placeholder formatting.
   Specific implementation code:
   ```cpp
   ss << hex << setfill('0') << setw(2) << decimal;
   int hexValue;
   ss >> hex >> hexValue;
   hexArray[index] = hexValue;
   ```

### The original serial port sending function could not send integer data frames.
   Solution: A new serial port sending function was written to send an integer array to the ESP32 microcontroller. This function accepts the base address of an integer array and removes carriage returns when sending information.
   Specific implementation code:
   ```cpp
   void sendHexCommandToESP32(const int* command) {
       if (hSerial == INVALID_HANDLE_VALUE) {
           std::cerr << "Serial port not initialized." << std::endl;
           return;
       }
       DWORD bytesWritten;
       for (int i = 0; i < 6; i++) {
           WriteFile(hSerial, command + i, 1, &bytesWritten, NULL);
       }
   }
   ```

### Incorrect data transmitted back from the smart car simulator.
The smart car simulator transmits data like this: PWM_L = 255, Speed_L = 38, STOP; PWM_R = 255, Speed_R = 38, STOP. The status is STOP, but the speed is not zero, indicating incorrect data transmission.  
Solution: In the car status judgment part, recognition of "STOP" was added to improve the accuracy of status discrimination.  
   ```cpp
   else if ((PWM_L == 0 && PWM_R == 0) || receivedData.find("STOP") != std::string::npos)
       carStatus = "Stopped";
   ```

### Delay in receiving and judging the car's status.
   The serial port information receiving function receiveDataFromESP32() has significant delay in receiving information.
   Solution: A "Refresh Status" option was added to the menu to improve the timeliness of status recognition by multiple acquisitions of data transmitted back from the microcontroller. However, there is currently no feasible solution for the delay itself, and the system functionality needs further improvement.


## Reflection
During the design process of the ESP32 smart car control system, I deeply experienced the charm of combining hardware and software. By programming to control the behavior of hardware, realizing functions such as the car's forward, backward, turning, and PWM speed adjustment, I felt the infinite possibilities of technology. However, in the actual design process, I also encountered many challenges, such as how to accurately read the car's status data, how to accurately calculate the XOR checksum, and how to stably control the motors. Through consulting materials, debugging code, and algorithm optimization, I gradually overcame these difficulties. During the design process, I also constantly tried out new ideas in my mind, cultivated innovation and exploration spirit, and improved my problem-solving abilities and practical skills in C++ programming.

## Suggestions
 - More practical sessions should be added to the course, giving students more opportunities to hands-on operate hardware, write code, and debug systems. Only through practice can students gain a deeper understanding of theoretical knowledge and improve their innovative practical abilities.
 -  Some team collaboration projects should be set up in the course, giving students opportunities to exercise their team collaboration and communication skills in practice. Teachers can also provide some team collaboration tips and methods to help students better cooperate with others.
 - Some application case analyses of smart car control systems can be added to the course, allowing students to understand practical application scenarios. Through discussion and analysis, students can gain a deeper understanding of the design principles and implementation methods of the system, and also stimulate their innovative thinking and improve their problem-solving abilities.
