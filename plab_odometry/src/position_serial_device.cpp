#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include "jsoncpp/json/json.h"
#include <string.h>
#include <stdlib.h>
#include <list>
#include <libusb-1.0/libusb.h>
#include <sys/ioctl.h>
#include <cassert>
#include <cstdio>
#include <ros/ros.h>
#include <fabrika_msgs/Konumxy.h>
//Device name , buffer length, baudrate

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
	cout<<"IT OPENED"<<endl;
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
//	cout<<read_info.buffer_to_string<<endl;
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
		}

		if (json_read_value!=1){
			printf("JSON can not read data.");
			}
		return data;
	}
	
};


int main(int argc, char **argv)
{
	
	ros::init(argc, argv, "serial_posxy");

	ros::NodeHandle n;
	ros::NodeHandle nodePub;

	const char *device = "/dev/ttyACM0";
	int fd;
	ReadInfoStruct read_info;
	
	const char *baudrate="B19200";
	int bytes_read;
	string buffer_to_string;	
	double x;
	double y;	
	
	SerialDevice serial(device,baudrate,29);
	fd = serial.SerialDeviceOpen(device);


	while(fd==-1){
		fd = serial.SerialDeviceOpen(device); //The returned file handle for the device. -1 if an error occurred
		printf( "failed to open port\n" );		

	}

	serial.SerialDeviceConfig(fd,"B19200");

	string json_data_objects[]={"TX","TY"};
	int size_json_data_objects=sizeof(json_data_objects)/sizeof(*json_data_objects);
	float *data = (float*)malloc(size_json_data_objects);
	Parse obje;
	ros::Publisher konumxy = nodePub.advertise<fabrika_msgs::Konumxy>("Konumxy", 1000);

	ros::Rate loop_rate(10);

	while(ros::ok()){


		
		read_info = serial.SerialDeviceRead(fd,29);
		cout<<"Byte : "<<read_info.bytes_read<<endl;
		cout<<"String "<<read_info.buffer_to_string<<endl;
		
		cout<<"FD : "<<fd<<endl;
		if(read_info.bytes_read == 29){
			
			data = obje.parseSerialJson(read_info.buffer_to_string,json_data_objects,size_json_data_objects);
			int data_size = sizeof(data)/sizeof(*data);
			x =data[0];
			y =data[1];				
			cout<<"x : "<<x<<endl;
			cout<<"y : "<<y<<endl;						
			fabrika_msgs::Konumxy msgKonum;
			msgKonum.position_x=x;
			msgKonum.position_y=y;				

			konumxy.publish(msgKonum);
		}
			
		printf("\n +----------------------------------+\n\n\n");
			
		ros::spinOnce();

		loop_rate.sleep();
		
	 }
	 close(fd);
	return 0;
}
