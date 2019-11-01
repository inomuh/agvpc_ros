#include "agv_odometry/agv_odometry.h"

void robotCallback(const agv_msgs::RobotLow msglow){
    encoder_left = msglow.encoder_left;
    encoder_right = msglow.encoder_right;
    robot_counter = msglow.robot_counter;
    robot_wl = msglow.robot_wl;
    robot_wr = msglow.robot_wr;
    robot_wl_ref = msglow.robot_wl_ref;
    robot_wr_ref = msglow.robot_wr_ref;
	robot_el_diff = msglow.encoder_left_diff;
	robot_er_diff = msglow.encoder_right_diff;
	std::cout<<encoder_left<<std::endl;
}

void cmdvelCallback(const geometry_msgs::Twist msgcmd){
	linear_x = msgcmd.linear.x;
	angular_z = msgcmd.angular.z;
}

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
  counter_txt.open(textname+"-robot-data.csv");
  std::ostringstream s_counter_txt;
  //counter_txt << "ROS time,U_ref,W_ref,X,Y,Heading Ang,U_robot,W_robot,Encoder_Left,Encoder_Right,Robot_HA,Robot_wl,Robot_wr,Robot_wl_ref,Robot_wr_ref,V_Left,V_Right\n";
  counter_txt<<"robot_wr,robot_wl,robot_wr_ref,robot_wl_ref,robot_counter,encoder_right,encoder_left\n";

  ros::init(argc, argv, "encoder_to_odom");

  ros::NodeHandle n;

  ros::Publisher wheel_odom_pub = n.advertise<nav_msgs::Odometry>("/agv1/odom", 100);
  tf::TransformBroadcaster odom_broadcaster;

  ros::Subscriber roblow = n.subscribe("/agv1/robot_low",1000,robotCallback);
  ros::Subscriber robcmd_vel = n.subscribe("/agv1/cmd_vel",1000,cmdvelCallback);

  ros::Time current_time, last_time;
  ros::Duration time_diff;

  wheel_radius = 0.125;
  wheel_separation = 0.71;

  //İlk değerlerin atanması
  encoder_left_pre = 0;
  encoder_right_pre = 0;
  v_x = 0; // Hız için kullanılan x konum değeri
  v_y = 0; // Hız için kullanılan y konum değeri
  v_ha = 0; // Hız için kullanılan baş açısı konum değeri
  v_left_pre = 0;
  v_right_pre =0;
  v_left_flt = 0;
  v_right_flt = 0;

/*  x_pre = 4.18;
  y_pre = 0.66;
  ha_pre =M_PI/2;*/

  x_pre = 5.21;
  y_pre = 2.79;
  ha_pre = 0;

  robot_counter_pre = 0;
  ros::Rate loop_rate(100);

  while(ros::ok()){


	current_time = ros::Time::now();
	time_diff = current_time - last_time;


        robot_counter_diff = robot_counter - robot_counter_pre;
        if(robot_counter_diff<0){
                robot_counter_diff = robot_counter_diff + 65536;
         }

	if(robot_counter_diff>0){

		//Enkoder değerleri 0 ile 1892 değerleri arasındadır. Anormal değerler alınmamalıdır.
		if(encoder_left <1892 && encoder_left>=0 && encoder_right<1892 && encoder_right>=0){


			std::cout<<robot_counter_diff<<std::endl;
	

			//Enkoder değerleri arasındaki farkların hesaplanması
			encoder_left_diff = encoder_left - encoder_left_pre;
			encoder_right_diff = encoder_right - encoder_right_pre;

			//Sol teker enkoder değerlerinin düzeltilmesi
			if(encoder_left_diff<(-1892/2)){
				encoder_left_diff = encoder_left_diff + 1892;
			}

			else if(encoder_left_diff>(1892/2)){
				encoder_left_diff = encoder_left_diff - 1892;
			}

			//Sağ teker enkoder değerlerinin düzeltilmesi
			if(encoder_right_diff<(-1892/2)){
				encoder_right_diff = encoder_right_diff + 1892;
			}

			else if(encoder_right_diff>(1892/2)){
				encoder_right_diff = encoder_right_diff - 1892;
			}


			//Enkoder verilerinden tekerlek açılarının hesaplanması
			delta_theta_left = ((2*M_PI)/1892)*encoder_left_diff;
			delta_theta_right = ((2*M_PI)/1892)*encoder_right_diff;

			//Hesaplanan açılardan anlık baş açısı ve tekerlek yer değiştirmesinin hesaplanması
			delta_sigma = atan((wheel_radius * (delta_theta_right - delta_theta_left)) / wheel_separation);
			delta_l = delta_theta_left * wheel_radius;
			delta_r = delta_theta_right * wheel_radius;

			//Anlık tekerlek yer değiştirmelerinden x ve y yer değişirmelerinin hesaplanması
			delta_c = (delta_l + delta_r)/2;
			delta_x = delta_c * cos(ha_pre);
			delta_y = delta_c * sin(ha_pre);

			//Konum verilerinin hesaplanması
			x_post = x_pre + delta_x;
			y_post = y_pre + delta_y;
			ha_post = ha_pre + delta_sigma;

			//Baş açısının quaterniona çevrilmesi
			qz_post = sin(ha_post * 0.5);
			qw_post = cos(ha_post * 0.5);
			std::cout<<"---------------"<<std::endl;

			/*std::cout<<"x_post "<<x_post<<std::endl;
			std::cout<<"y_post "<<y_post<<std::endl;
		std::cout<<"qz_post "<<qz_post<<std::endl;
		std::cout<<"qw_post "<<qw_post<<std::endl;
		std::cout<<"---------------"<<std::endl;*/

		//Saniyede hız verisi basma

/*		v_left = (robot_el_diff*(2*M_PI/1892))/0.01;
		v_right = (robot_er_diff*(2*M_PI/1892))/0.01;
*/
	                v_left = delta_l/(robot_counter_diff/1000000.0);
	                v_right = delta_r/(robot_counter_diff/1000000.0);
			std::cout<<"el "<<encoder_left<<std::endl;
                        std::cout<<"er "<<encoder_right_diff<<std::endl;	

			v_left_flt = (b1/a1) * v_left + (b2/a1) * v_left_pre - (a2/a1) * v_left_pre;
			v_right_flt = (b1/a1) * v_right + (b2/a1) * v_right_pre - (a2/a1) * v_right_pre;
			if(fabs(v_left_flt * wheel_radius - v_left_pre * wheel_radius)>0.3){
				v_left_flt = v_left_pre;
			}
			if(fabs(v_right_flt * wheel_radius - v_right_pre * wheel_radius)>0.3){
				v_right_flt = v_right_pre;
			}
			v_left_pre = v_left_flt;
			v_right_pre = v_right_flt;

			v_x = (v_left_flt + v_right_flt)/2;
	                v_ha = (v_right_flt - v_left_flt)/wheel_separation;
			std::cout<<"v_left "<<v_left<<std::endl;
			std::cout<<"v_right"<<v_right<<std::endl;
			if(ha_post>M_PI){
				ha_post = ha_post - 2*M_PI;
			}
			else if(ha_post<-M_PI){
				ha_post = ha_post + 2*M_PI;
			}
			cov_of_pose_xy_delta = 0.01 * 0.125;
			cov_of_pose_xy = cov_of_pose_xy + cov_of_pose_xy_delta;

			cov_of_pose_yaw_delta = atan(2 * (0.01 * 0.125)/wheel_separation);
			cov_of_pose_yaw =  cov_of_pose_yaw + cov_of_pose_yaw_delta;
	//        s_counter_txt << secs <<","<<nsecs<<","<< encoder_left<<","<<encoder_right<<","<<x_post << ","<< y_post<<","<<ha_post;
	//	s_counter_txt << encoder_left <<","<<encoder_right<<","<<v_x<<","<<v_ha<<","<<x_post<<","<<y_post<<","<<ha_post;
//			s_counter_txt << current_time<<","<<linear_x<<","<<angular_z<<","<<x_post<<","<<y_post<<","<<ha_post<<","<<v_x<<","<<v_ha<<","<<encoder_left<<","<<encoder_right<<","<<robot_counter<<","<<robot_wl<<","<<robot_wr<<","<<robot_wl_ref<<","<<robot_wr_ref<<","<<v_left<<","<<v_right;
			s_counter_txt<<robot_wr<<","<<robot_wl<<","<<robot_wr_ref<<","<<robot_wl_ref<<","<<robot_counter<<","<<encoder_right<<","<<encoder_left;
			counter_txt<<s_counter_txt.str()<<std::endl;
			s_counter_txt.str("");
	}
	geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(ha_post);

	//first, we'll publish the transform over tf
	geometry_msgs::TransformStamped odom_trans;
	odom_trans.header.stamp = current_time;
	odom_trans.header.frame_id = "agv1/odom";
	odom_trans.child_frame_id = "/agv1/base_link";

	odom_trans.transform.translation.x = x_post;
	odom_trans.transform.translation.y = y_post;
	odom_trans.transform.translation.z = 0.0;
	odom_trans.transform.rotation = odom_quat;

	odom_broadcaster.sendTransform(odom_trans);

	// Değerlerin Odometry mesaj tipi ile ROS üzerinden yayınlanması
	nav_msgs::Odometry msg_wheel_odom;
	msg_wheel_odom.header.frame_id="agv1/odom";
	msg_wheel_odom.child_frame_id="agv1/base_link";
	msg_wheel_odom.header.stamp=current_time;
	msg_wheel_odom.pose.pose.position.x = x_post;
	msg_wheel_odom.pose.pose.position.y = y_post;
	msg_wheel_odom.pose.pose.orientation.z = qz_post;
	msg_wheel_odom.pose.pose.orientation.w = qw_post;
	msg_wheel_odom.pose.covariance[0] = cov_of_pose_xy;
	msg_wheel_odom.pose.covariance[7] = cov_of_pose_xy;
	msg_wheel_odom.pose.covariance[14] = 0;
	msg_wheel_odom.pose.covariance[21] = 0;
	msg_wheel_odom.pose.covariance[28] = 0;
	msg_wheel_odom.pose.covariance[35] = cov_of_pose_yaw;

	msg_wheel_odom.twist.twist.linear.x = v_x;
	msg_wheel_odom.twist.twist.linear.y = 0;
	msg_wheel_odom.twist.twist.angular.z = v_ha;
	msg_wheel_odom.twist.covariance[0] = 99999;
	msg_wheel_odom.twist.covariance[7] = 99999;
	msg_wheel_odom.twist.covariance[14] = 0;
	msg_wheel_odom.twist.covariance[21] = 0;
	msg_wheel_odom.twist.covariance[28] = 0;
	msg_wheel_odom.twist.covariance[35] = 99999;

	wheel_odom_pub.publish(msg_wheel_odom);

	x_pre = x_post;
	y_pre = y_post;
	ha_pre = ha_post;
	encoder_left_pre = encoder_left;
	encoder_right_pre = encoder_right;
	robot_counter_pre = robot_counter;
	last_time = ros::Time::now();
	}
        ros::spinOnce();
        loop_rate.sleep();
    }

  ros::spin();

  return 0;
}

