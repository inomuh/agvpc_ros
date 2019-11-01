#include "plab_odometry/plab_odometry.h"



void cmdvelCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
	
    cmdvel_lx = msg->linear.x;
    cmdvel_az = msg->angular.z;
    
}


int main(int argc, char **argv)
{
	
	ros::init(argc, argv, "serial_posxy");
	ros::NodeHandle nodePub;
    ros::Subscriber sub = nodePub.subscribe("/agv1/YSK_M/cmd_vel", 100, cmdvelCallback);
	ros::Publisher konumxy = nodePub.advertise<nav_msgs::Odometry>("odom", 10);

	const char *device = "/dev/ttyACM0";
	const char *baudrate="B19200";
	string buffer_to_string;	
	double x,y;		
	int fd, bytes_read, data_size;
	
	ReadInfoStruct read_info;
	SerialDevice serial(device,baudrate,29);
	fd = serial.SerialDeviceOpen(device);

	while(fd==-1){
		fd = serial.SerialDeviceOpen("/dev/ttyACM1"); //The returned file handle for the device. -1 if an error occurred
		printf( "failed to open port\n" );
		sleep(100);
	}

	serial.SerialDeviceConfig(fd,"B19200");

	string json_data_objects[]={"DATA","TX","TY","SA","SB","SC"};
	int size_json_data_objects=sizeof(json_data_objects)/sizeof(*json_data_objects);
	float *data = (float*)malloc(size_json_data_objects);
	Parse obje;

	ros::Rate loop_rate(5);

	while(ros::ok()){
		read_info = serial.SerialDeviceRead(fd,255);
		cout<<"Byte : "<<read_info.bytes_read<<endl;
		//cout<<"String "<<read_info.buffer_to_string<<endl;
		//cout<<"FD : "<<fd<<endl;
		if(read_info.bytes_read>=80){
			data = obje.parseSerialJson(read_info.buffer_to_string,json_data_objects,size_json_data_objects);
			data_size = sizeof(data)/sizeof(*data);
			cout<<"data_size : "<<data_size<<endl;
			x =data[1];
			y =data[2];	
			cout<<"y : "<<y<<endl;
		};
		nav_msgs::Odometry msgKonum;
			cout<<"x : "<<x<<endl;
			msgKonum.header.frame_id = "odom";
			msgKonum.pose.pose.position.x=x/100;
			msgKonum.pose.pose.position.y=y/100;
			msgKonum.twist.twist.linear.x=cmdvel_lx;
			msgKonum.twist.twist.angular.z=cmdvel_az;	
			konumxy.publish(msgKonum);
//		printf("\n +----------------------------------+\n\n\n");

		ros::spinOnce();

		loop_rate.sleep();
		
	 }
	 close(fd);
	return 0;
}
