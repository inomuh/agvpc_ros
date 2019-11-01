#!/usr/bin/env python
import rospy
import json
from std_msgs.msg import String

def callback(data):
    gorev_sunucu = data.data
    gg = json.loads(gorev_sunucu)
    for i in range(len(gg["plan"])):
	robotId = gg["plan"][i]["robotID"]
	robotEquipmentID = gg["plan"][i]["robotEquipmentID"]
	#print("%d %d\n" % (robotId,robotEquipmentID))
	robotTaskList = gg["plan"][i]["taskList"]
	for y in range(len(gg["plan"][i]["taskList"])):
		print(y)
		robotTaskId = gg["plan"][i]["taskList"][y]["id"]
		robotTaskWorkBench = gg["plan"][i]["taskList"][y]["workbenchID"]
		robotTaskStationId = gg["plan"][i]["taskList"][y]["stationID"]
		robotTaskDuty = gg["plan"][i]["taskList"][y]["duty"]
		robotTaskProductName = gg["plan"][i]["taskList"][y]["productName"]
		robotTaskProductCount = gg["plan"][i]["taskList"][y]["productCount"]
		print("%d %d %d %s %s %d\n" %(robotTaskId,robotTaskWorkBench,robotTaskStationId,robotTaskDuty,robotTaskProductName,robotTaskProductCount))
		print("***********************")
		for z in range(len(gg["plan"][i]["taskList"][y]["route"])):
			print(z)
			if len(gg["plan"][i]["taskList"][y]["route"][z]["tags"]) == 3:
				robotTaskRouteTagType = gg["plan"][i]["taskList"][y]["route"][z]["tags"]["type"]
				robotTaskRouteTagTypeCode = gg["plan"][i]["taskList"][y]["route"][z]["tags"]["type_code"]
				robotTaskRouteTagTypeDef = gg["plan"][i]["taskList"][y]["route"][z]["tags"]["type_definition"]
				robotTaskRouteX = gg["plan"][i]["taskList"][y]["route"][z]["x"]
				robotTaskRouteY = gg["plan"][i]["taskList"][y]["route"][z]["y"]
				robotTaskRouteXYId = gg["plan"][i]["taskList"][y]["route"][z]["id"]
				waypoint = [robotTaskRouteTagType,robotTaskRouteTagTypeCode,robotTaskRouteTagTypeDef,robotTaskRouteX,robotTaskRouteY,robotTaskRouteXYId]
				print(waypoint)
		#		print("%s %s %s %f %f %d\n" %(robotTaskRouteTagType,robotTaskRouteTagTypeCode,robotTaskRouteTagTypeDef,robotTaskRouteX,robotTaskRouteY,robotTaskRouteXYId))
			if len(gg["plan"][i]["taskList"][y]["route"][z]["tags"]) == 4:
				robotTaskRouteTagIntersection = gg["plan"][i]["taskList"][y]["route"][z]["tags"]["intersection"]
				robotTaskRouteTagType = gg["plan"][i]["taskList"][y]["route"][z]["tags"]["type"]
				robotTaskRouteTagTypeCode = gg["plan"][i]["taskList"][y]["route"][z]["tags"]["type_code"]
				robotTaskRouteTagTypeDef = gg["plan"][i]["taskList"][y]["route"][z]["tags"]["type_definition"]
				robotTaskRouteX = gg["plan"][i]["taskList"][y]["route"][z]["x"]
				robotTaskRouteY = gg["plan"][i]["taskList"][y]["route"][z]["y"]
				robotTaskRouteXYId = gg["plan"][i]["taskList"][y]["route"][z]["id"]
		#		print("%s %s %s %s %f %f %d\n" %(robotTaskRouteTagIntersection,robotTaskRouteTagType,robotTaskRouteTagTypeCode,robotTaskRouteTagTypeDef,robotTaskRouteX,robotTaskRouteY,robotTaskRouteXYId))
				waypoint = [robotTaskRouteTagType,robotTaskRouteTagTypeCode,robotTaskRouteTagTypeDef,robotTaskRouteX,robotTaskRouteY,robotTaskRouteXYId]
				print(waypoint)
print("---------------------------------")
    
def listener():

    rospy.init_node('gorev_parse', anonymous=True)

    rospy.Subscriber("task", String, callback)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
