/*
 * CANBUS PROTOCOL
 * Last digit of Can Id defines format of data.
 * 
 * 0 -> Int(32 bit)
 * 1 -> uint8_t(8 bit)
 * 2 -> uint16_t(16 bit)
 * 3 -> uint32_t(32 bit)
 * 4 -> float(32 bit)
 * 5 -> Double(64 bit)
 * 6 -> Char(16 bit)
 * 7 -> Long (64 bit)
 * 8 -> Short (16 bit)
 * 9 -> Look Table!!
 * 
 * */

#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <iomanip>  
#include <string>
#include <ctime>
#include <boost/bind.hpp>
#include <ros/ros.h>
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include "can_msgs/Frame.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Int16.h"
#include "std_msgs/String.h"
#include "std_msgs/Float64.h"
#include "std_msgs/Int16MultiArray.h"
#include "agv_msgs/BatteryData.h"
#include "agv_msgs/PowerData.h"
#include "agv_msgs/ControlData.h"
#include "agv_msgs/RobotLow.h"

int canDlc,canDataIndex;

double motor1Energy,motor2Energy;
double yskmEnergyData, yskgEnergyData, dskmdEnergyData, dskmEnergyData, laserSensorEnergyData, j5EnergyData, j6EnergyData, j7EnergyData;

char canHexArrayYskEncoder[8];
unsigned int encoder1data,encoder2data,robot_counter;
int encoder1diff,encoder2diff,fault_t,fault_a4940,emg_button;
float robot_wr,robot_wl,robot_wl_ref,robot_wr_ref;
float motor1VoltageData, motor1CurrentData, motor1PowerData;
float motor2VoltageData, motor2CurrentData, motor2PowerData;
float temperatureData, ledStatus, counter;
float yskmVoltageData, yskmCurrentData, yskmPowerData;
float yskgVoltageData, yskgCurrentData, yskgPowerData;
float dskmdVoltageData, dskmdCurrentData, dskmdPowerData;
float dskmVoltageData, dskmCurrentData, dskmPowerData;
float laserSensorVoltageData, laserSensorCurrentData, laserSensorPowerData;
float j5VoltageData, j5CurrentData, j5PowerData;
float j6VoltageData, j6CurrentData, j6PowerData;
float j7VoltageData, j7CurrentData, j7PowerData;
float controlBmpData, controlLampData, modeSelection, emgData;
float batteryVoltage, batteryTotalCurrent, batteryPercentage;	
float loadCell1Data, loadCell2Data, loadCell3Data, loadCell4Data, lampData;

std::stringstream canIdData;
std::string canId;
