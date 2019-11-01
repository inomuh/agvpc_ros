#include "canbus_ros_comm/topic2can.h"

void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msgcmd)
{
	vel_u=msgcmd->linear.x;
	vel_w=msgcmd->angular.z;
	
	char *uDouble2HexValue= (char *)&vel_u;
	memcpy(uHexArray,uDouble2HexValue,8);

	char *wDouble2HexValue= (char *)&vel_w;
	memcpy(wHexArray,wDouble2HexValue,8);

}

void parameterCallback(const agv_msgs::YskParam::ConstPtr& msgparameter)
{
	max_lvel=msgparameter->max_lin_vel;
	max_avel=msgparameter->max_ang_vel;
	reset=msgparameter->reset;
	max_temp=msgparameter->max_temp;
	min_temp=msgparameter->min_temp;
}


int main(int argc,char **argv)
{
  ros::init(argc, argv, "send_to_canbus");

  ros::NodeHandle n;
  ros::NodeHandle nodeparam;
  ros::Subscriber sub_vel = n.subscribe("cmd_vel", 1, cmdVelCallback);  
  ros::Subscriber sub_parameter = n.subscribe("max_min_parameter_data", 10, parameterCallback);  
  ros::Publisher velUPublisher = n.advertise<can_msgs::Frame>("sent_messages", 10);
  ros::Publisher velWPublisher = n.advertise<can_msgs::Frame>("sent_messages", 10);  
  ros::Publisher parameterPublisher = n.advertise<can_msgs::Frame>("sent_messages", 10);  
  //system("roslaunch canbus_ros_comm socketcan.launch &");  

  ros::Rate loop_rate(10);
  flag=0;
  char *ab[8];
  while (ros::ok())
  {
// Lineer hızın canbus ile aktarımı
 
	can_msgs::Frame msg_u;

	msg_u.id=0x205; // can id
	msg_u.dlc=8; // veri uzunluğu
	msg_u.is_rtr = false; // 
	msg_u.is_extended = false; // verinin 8 byte'tan uzun olup olmadığını belirtir
	msg_u.is_error = false;
	memcpy(&msg_u.data,(char *) uHexArray, 8); // Verinin aktarıldığı alan. 

// Açısal hızın canbus ile aktarımı
	can_msgs::Frame msg_w;

	msg_w.id=0x215;
	msg_w.dlc=8;
	msg_w.is_rtr = false;
	msg_w.is_extended = false;
	msg_w.is_error = false;
	memcpy(&msg_w.data,(char *) wHexArray, 8);

// Max-min parametrelerinin canbus ile aktarımı
	can_msgs::Frame msg_parameter;

	msg_parameter.id=0x201;
	msg_parameter.dlc=8;
	msg_parameter.is_rtr = false;
	msg_parameter.is_extended = false;
	msg_parameter.is_error = false;
	msg_parameter.data[0]=180; // max lineer hız (1.8 m/s)
	msg_parameter.data[1]=50;  //
	msg_parameter.data[2]=255; // Alt seviye resetleri için kullanılır (Enkoder değerleri sıfırlanır.)
	msg_parameter.data[3]=0;
	msg_parameter.data[4]=0;
	msg_parameter.data[5]=60; // max temperature
	msg_parameter.data[6]=0;
	msg_parameter.data[7]=50; // min temperature

	velUPublisher.publish(msg_u);
	velWPublisher.publish(msg_w);
	
// Ota start launch başlatıldığında başlangıçta canbus ile yayınlanıp sonra duruyor
	if(flag<20){
		parameterPublisher.publish(msg_parameter);
	}
	flag=flag+1;
	
	//std::cout<<flag<<std::endl;
	
    ros::spinOnce();

    loop_rate.sleep();
  }
  ros::spin();


  return 0;
}
