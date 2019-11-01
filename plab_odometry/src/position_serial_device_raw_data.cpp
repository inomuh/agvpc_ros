#include "plab_odometry/plab_odometry.h"
int main(int argc, char **argv)
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"%d-%m-%Y_%H:%M:%S",timeinfo);
  std::string textname(buffer);
  std::ofstream counter_txt;
  counter_txt.open(textname+"-plab.csv");
  std::ostringstream s_counter_txt;
	
	counter_txt << " sec,nsec,DATA,DAS,DBS,DCS\n";
	ros::init(argc, argv, "plab_raw");
	ros::NodeHandle nodePub;
    //ros::Subscriber sub = nodePub.subscribe("/agv1/YSK_M/cmd_vel", 100, cmdvelCallback);
	//ros::Publisher konumxy = nodePub.advertise<nav_msgs::Odometry>("odom", 10);
    double das,dbs,dcs;
    std::string data_raw;
	int32_t secs,nsecs; 
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

	string json_data_objects[]={"DATA","DAS","DBS","DCS"};
	int size_json_data_objects=sizeof(json_data_objects)/sizeof(*json_data_objects);
	float *data = (float*)malloc(size_json_data_objects);
	Parse obje;

	ros::Rate loop_rate(100);

	while(ros::ok()){
		secs = ros::Time::now().sec;
		nsecs = ros::Time::now().nsec;
		read_info = serial.SerialDeviceRead(fd,255);
		cout<<"Byte : "<<read_info.bytes_read<<endl;
		//cout<<"String "<<read_info.buffer_to_string<<endl;
		//cout<<"FD : "<<fd<<endl;
	
		if(read_info.bytes_read >40){
			data = obje.parseSerialJson(read_info.buffer_to_string,json_data_objects,size_json_data_objects);
			data_size = sizeof(data)/sizeof(*data);
			cout<<"data_size : "<<data_size<<endl;
			data_raw = data[0];
			das = data[1];
			dbs = data[2];	
			dcs = data[3];
	}
		/*nav_msgs::Odometry msgKonum;
			cout<<"x : "<<x<<endl;
			msgKonum.header.frame_id = "odom";
			msgKonum.pose.pose.position.x=x/100;
			msgKonum.pose.pose.position.y=y/100;
			msgKonum.twist.twist.linear.x=cmdvel_lx;
			msgKonum.twist.twist.angular.z=cmdvel_az;	
			konumxy.publish(msgKonum);*/
//		printf("\n +----------------------------------+\n\n\n");
	counter_txt << secs <<","<<nsecs<<","<< data[0] << ","<< data[1] << "," << data[2]<<","<<data[3]<<std::endl;; //motor1Energy

		ros::spinOnce();

		loop_rate.sleep();
		
	 }
	 close(fd);
	return 0;
}
