#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include "jsoncpp/json/json.h"

#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <sys/ioctl.h>

#include <fstream>

#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
//Device name , buffer length, baudrate

double cmdvel_lx, cmdvel_az;

using namespace std;

	struct ReadInfoStruct{
		
		string buffer_to_string;
		int bytes_read;
		
	};

class SerialDevice
{	

	public:

	SerialDevice(const char *device_name,const char * baudrate,int buffer_length );

	int SerialDeviceOpen(const char *device_name);
	void SerialDeviceConfig(int fd, const char * baudrate);
	ReadInfoStruct SerialDeviceRead(int fd,int buffer_length);
	
};

SerialDevice::SerialDevice(const char *device_name,const char * baudrate,int buffer_length ){

	cout<<"OBJECT HAVE BEEN CREATED"<<endl;

}
int SerialDevice::SerialDeviceOpen(const char *device_name)
{
	int fd = open(device_name, O_RDONLY | O_NONBLOCK | O_NOCTTY);
	//cout<<"IT OPENED"<<endl;
	return fd;
}

void SerialDevice::SerialDeviceConfig(int fd, const char * baudrate){
	
	struct termios config;
	
	int devicecheck= tcgetattr(fd, &config);         // Get the current attributes of the Serial port 
	
	cfsetispeed(&config,*baudrate); 
	cfsetospeed(&config,*baudrate); 

	
	config.c_iflag &= (IXON | IXOFF | ISTRIP);
    	config.c_oflag = 0;// (OPOST | OFILL | TABDLY);
   	config.c_lflag |= (ECHO | ECHONL | ICANON | ISIG | IEXTEN | PENDIN);
    	config.c_cflag &= (CSIZE | PARENB);
	config.c_cflag |= (CS8 | CLOCAL | CREAD | CSTOPB);
    	config.c_cc[VMIN]=1;
    	config.c_cc[VTIME]=0;

    	fcntl(fd,F_SETFL,0);
      
	tcsetattr(fd, TCSADRAIN, &config);


	if(tcsetattr(fd, TCSANOW, &config) < 0) { printf(" error handling ..." );};

	cout<<"CONFIGURATION IS SET"<<endl;
}

ReadInfoStruct SerialDevice::SerialDeviceRead(int fd,int buffer_length){
	
	ReadInfoStruct read_info;
	char read_buffer[255];// Stringe çevirelebilmesi için pointer tanımlanmamalı.
	read_info.bytes_read = read(fd,&read_buffer,255); // Read the data and number of bytes       				
	read_info.buffer_to_string=read_buffer;
	//cout<<"buffer_to_string"<<read_info.buffer_to_string.size()<<endl;
//	cout<<"fd : "<<fd<<endl;
	
	return read_info;
}
	
class Parse{
	
	public:
	
	float* parseSerialJson(string buffer_to_string, string json_data_objects[],int size_json_data_objects){
	
		Json::Reader reader;
		Json::Value val;

		//float data[size_data_objects];
		
		float *data = (float*)malloc(size_json_data_objects);	

		int json_read_value=reader.parse(buffer_to_string,val); //JSON kütüphanesinde reader(string,value)

		for(int index=0;index<size_json_data_objects;index++){
			data[index]=strtof(val[json_data_objects[index]].asString().c_str(),0);
			//cout<<"data[index] "<<data[index]<<endl;
		}

		if (json_read_value!=1){
			printf("JSON can not read data.");
			}
		return data;
	}
	
};
/*
float outlier_rejection_w_moving_average(std::array<int,10> arr){
	int i=0;
	int s_wind=0;
	int arr_size;
	int index=0;
	int sort_arr[3][10];
	sort(arr.begin(),arr.end());
	
	for(i=0;i<10;i++){
		sort_arr[0][i] = arr[i];
		sort_arr[1][i] = i;
		sort_arr[2][i] = 1000;
	}

	arr_size = sizeof(sort_arr[0])/sizeof(*sort_arr[0]);

	for(index=0,index<(arr_size/2)+1;index++){
		sort_arr[2][index] = sort_arr[0][index+(arr_size/2)-1] - sort_arr[0][index];
	}
	
	index = 0;
	
	for(index=0;index<arr_size;index++){
		if(sort_arr[2][index]==*a){
			s_wind = sort_arr[1][index];
		}
	}

	float sum = 0;
	
	for(index=s_wind;index<s_wind+5;index++){
		sum = sum+sort_arr[0][index];
	}
	float value = sum/500;

	return value;
}*/
