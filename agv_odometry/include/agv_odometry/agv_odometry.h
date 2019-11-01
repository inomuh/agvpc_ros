#include <fstream>

#include <tf/transform_broadcaster.h>
#include "std_msgs/Int16.h"
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "std_msgs/Float64.h"
#include "agv_msgs/RobotLow.h"

#define a1 1.000000000000000
#define a2 -0.819806805307137
#define b1 0.090096597346431
#define b2 0.090096597346431

int encoder_left,encoder_right,encoder_left_diff,encoder_right_diff,encoder_left_pre,encoder_right_pre,robot_counter_diff;
unsigned int robot_counter,robot_counter_pre;
double delta_theta_left,delta_theta_right,delta_sigma,delta_l,delta_r,delta_c,delta_x,delta_y,ha_pre,ha_post,x_post,x_pre,y_post,y_pre;
double vx,vy,vz,wz,wz_delta,vx_post,vy_post,vx_pre,vy_pre,wz_post,wz_pre,v_x,v_y,v_ha,v_left_pre,v_right_pre,v_left_flt,v_right_flt;
double qz_post,qw_post;
double cov_of_pose_xy,cov_of_pose_xy_delta,cov_of_pose_yaw,cov_of_pose_yaw_delta,count;
double cov_pose[36],cov_cmd[36];
double linear_x,angular_z,v_left,v_right,robot_ha;//,robot_wl,robot_wr;
float robot_wl,robot_wr,robot_wl_ref,robot_wr_ref;
int robot_el_diff,robot_er_diff;
float wheel_radius,wheel_separation;
