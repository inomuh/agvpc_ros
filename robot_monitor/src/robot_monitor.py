#!/usr/bin/env python
import rospy
import rosnode

def check():

    while not rospy.is_shutdown():

        bakim_node_check = rosnode.rosnode_ping("/agv1/bakim_ihtiyaci",2,False)
        can2topic_node_check = rosnode.rosnode_ping("agv1/can2topic",2,False)
        topic2can_node_check = rosnode.rosnode_ping("/agv1/topic2can",2,False)
        durum_node_check = rosnode.rosnode_ping("/agv1/durum",2,False)
        odom_node_check = rosnode.rosnode_ping("agv1/ota_odometry",2,False)
        sick_node_check = rosnode.rosnode_ping("/agv1/sick_s300",2,False)
        socketcan_node_check = rosnode.rosnode_ping("/agv1/socketcanbridge",2,False)

        if bakim_node_check == False:
            print("Bakim node failed")

        if can2topic_node_check == False:
            print("Can2topic node failed")

        if topic2can_node_check == False:
            print("Topic2can node failed")

        if durum_node_check == False:
            print("Durum node failed")

        if odom_node_check == False:
            print("Odom node failed")

        if sick_node_check == False:
            print("Sick node failed")

        if socketcan_node_check == False:
            print("Socketcan node failed")


def node_check():
    rospy.init_node('node_check',anonymous=True)
    check()
    rospy.spin()

if __name__ == '__main__':
    node_check()
