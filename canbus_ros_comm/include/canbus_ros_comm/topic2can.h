#include <string>
#include <sstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip> 

#include <ros/ros.h>
#include "geometry_msgs/Twist.h"
#include "agv_msgs/YskParam.h"
#include "can_msgs/Frame.h"

double vel_u, vel_w;
char uHexArray[8], wHexArray[8];

float max_lvel, min_lvel, max_avel, min_avel, max_temp, min_temp, hornData, lampData, counterHeartbeat;

std::string maxLVelHex, minLVelHex, maxAVelHex, minAVelHex, maxTempHex, minTempHex;
std::stringstream maxTempHex2string,minTempHex2string;

int flag,reset;
