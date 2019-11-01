#include "canbus_ros_comm/can2topic.h"
double linear_x;
void canbusCallback(const can_msgs::Frame::ConstPtr& msgcan)
{
// CAN ID 
	canIdData << std::hex<< msgcan->id;
	canId = canIdData.str();
	std::cout<<"ASIL : "<<canId[0]<<"   "<<canId[1]<<"   "<<canId[2]<<std::endl;

	canIdData.str(""); //clear stringstream -> id'nin dinamik değişmesi için stream temizlenmeli
// CAN DATA LENGTH canDlc
	canDlc=msgcan->dlc;
	//std::cout<<canDlc<<std::endl;

//*****************************************************************************
/*CAN BUS ÜZERİNDEN GELEN VERİLERİN STRİNG HEX OLARAK ALINMASI*/
//*****************************************************************************

	std::string canHexArray[canDlc];
// Reading Data from "received_messages" topic
	int canIntData[canDlc];
	for (int canDataIndex=0; canDataIndex<canDlc; canDataIndex++){
		canIntData[canDataIndex] = msgcan->data[canDataIndex];
		std::stringstream hexBuffer;
		hexBuffer<<std::setfill ('0') << std::setw (2)<<std::hex<<canIntData[canDataIndex];
		canHexArray[canDataIndex]=hexBuffer.str();
	}

//*****************************************************************************	
//************************** DSK-MD KONTROL DATA

//**************************         102*/
	if(canId[0]=='1' && canId[1]=='0' && canId[2]=='2'){
		//std::cout<<canId<<std::endl;
		temperatureData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
		ledStatus=std::stoul(canHexArray[2],nullptr,16);
		fault_t=std::stoul(canHexArray[3],nullptr,16);
		fault_a4940=std::stoul(canHexArray[4],nullptr,16);
		emg_button=std::stoul(canHexArray[5],nullptr,16);

	}

//**************************  phi - x - y*/
	else if(canId[0]=='1' && canId[2]=='5'){
		//**************************         135*/
		if(canId[1]=='3'){
		//std::cout<<canId<<std::endl;
			char canHexArrayDskMd[canDlc];
			for (int canDataIndexDskMd=0; canDataIndexDskMd<canDlc; canDataIndexDskMd++){
				canHexArrayDskMd[canDataIndexDskMd]=msgcan->data[canDataIndexDskMd];
			}			
		memcpy(&motor1Energy,(char *) canHexArrayDskMd, canDlc);
		}
		//**************************         145*/
		if(canId[1]=='4'){
		//std::cout<<canId<<std::endl;
			char canHexArrayDskMd[canDlc];
			for (int canDataIndexDskMd=0; canDataIndexDskMd<canDlc; canDataIndexDskMd++){
				canHexArrayDskMd[canDataIndexDskMd]=msgcan->data[canDataIndexDskMd];
			}
		memcpy(&motor2Energy,(char *) canHexArrayDskMd, canDlc);
		}
	}

//************************** MOTOR 1 BİLGİLERİ
//***************************     109 */

	else if(canId[0]=='1' && canId[2]=='9'){
		if(canId[1] == '0'){
		//std::cout<<canId<<std::endl;
			motor1VoltageData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			motor1CurrentData=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			motor1PowerData=std::stoul(canHexArray[7]+canHexArray[6]+canHexArray[5]+canHexArray[4],nullptr,16);
		}
//************************** MOTOR 2 BİLGİLERİ
//***************************     119 */
		if(canId[1] == '1'){
		//std::cout<<canId<<std::endl;

			motor2VoltageData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			motor2CurrentData=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			motor2PowerData=std::stoul(canHexArray[7]+canHexArray[6]+canHexArray[5]+canHexArray[4],nullptr,16);
		}
	}

//*********************** DSK-M POWER ELEMENT(YSK-M,YSK-G...) BİLGİLERİ (voltaj,akım,güç)*/
/*
//***********************		309---->379	
	else if(canId[0]=='3' && canId[2]=='9'){
//************************  309
		if(canId[1]=='0'){
			yskmVoltageData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			yskmCurrentData=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			yskmPowerData=std::stoul(canHexArray[7]+canHexArray[6]+canHexArray[5]+canHexArray[4],nullptr,16);
		}
//************************  319
		if(canId[1]=='1'){
			yskgVoltageData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			yskgCurrentData=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			yskgPowerData=std::stoul(canHexArray[7]+canHexArray[6]+canHexArray[5]+canHexArray[4],nullptr,16);
		}
//************************  329
		if(canId[1]=='2'){
			dskmdVoltageData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			dskmdCurrentData=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			dskmdPowerData=std::stoul(canHexArray[7]+canHexArray[6]+canHexArray[5]+canHexArray[4],nullptr,16);
		}
//************************  339
		if(canId[1]=='3'){
			dskmVoltageData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			dskmCurrentData=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			dskmPowerData=std::stoul(canHexArray[7]+canHexArray[6]+canHexArray[5]+canHexArray[4],nullptr,16);
		}
//************************  349
		if(canId[1]=='4'){
			laserSensorVoltageData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			laserSensorCurrentData=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			laserSensorPowerData=std::stoul(canHexArray[7]+canHexArray[6]+canHexArray[5]+canHexArray[4],nullptr,16);
		}
//************************  359
		if(canId[1]=='5'){
			j5VoltageData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			j5CurrentData=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			j5PowerData=std::stoul(canHexArray[7]+canHexArray[6]+canHexArray[5]+canHexArray[4],nullptr,16);
		}
//************************  369
		if(canId[1]=='6'){
			j6VoltageData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			j6CurrentData=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			j6PowerData=std::stoul(canHexArray[7]+canHexArray[6]+canHexArray[5]+canHexArray[4],nullptr,16);
		}
//************************  379
		if(canId[1]=='7'){
			j7VoltageData=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			j7CurrentData=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			j7PowerData=std::stoul(canHexArray[7]+canHexArray[6]+canHexArray[5]+canHexArray[4],nullptr,16);
		}
	}*/
/*
	else if(canId[0]=='3' && canId[2]=='5'){
//************************  305
		if(canId[1]=='0'){
			char canHexArrayYsk[canDlc];
			for (int canDataIndexYsk=0; canDataIndexYsk<canDlc; canDataIndexYsk++){
				canHexArrayYsk[canDataIndexYsk]=msgcan->data[canDataIndexYsk];
			}
		memcpy(&yskmEnergyData,(char *) canHexArrayYsk, canDlc);
		}
//************************  315
		if(canId[1]=='1'){
			char canHexArrayYsk[canDlc];
			for (int canDataIndexYsk=0; canDataIndexYsk<canDlc; canDataIndexYsk++){
				canHexArrayYsk[canDataIndexYsk]=msgcan->data[canDataIndexYsk];
			}
		memcpy(&yskgEnergyData,(char *) canHexArrayYsk, canDlc);
		}
//************************  325
		if(canId[1]=='2'){
			char canHexArrayYsk[canDlc];
			for (int canDataIndexYsk=0; canDataIndexYsk<canDlc; canDataIndexYsk++){
				canHexArrayYsk[canDataIndexYsk]=msgcan->data[canDataIndexYsk];
			}
		memcpy(&dskmdEnergyData,(char *) canHexArrayYsk, canDlc);
		}
//************************  335
		if(canId[1]=='3'){
			char canHexArrayYsk[canDlc];
			for (int canDataIndexYsk=0; canDataIndexYsk<canDlc; canDataIndexYsk++){
				canHexArrayYsk[canDataIndexYsk]=msgcan->data[canDataIndexYsk];
			}
		memcpy(&dskmEnergyData,(char *) canHexArrayYsk, canDlc);
		}
//************************  345
		if(canId[1]=='4'){
			char canHexArrayYsk[canDlc];
			for (int canDataIndexYsk=0; canDataIndexYsk<canDlc; canDataIndexYsk++){
				canHexArrayYsk[canDataIndexYsk]=msgcan->data[canDataIndexYsk];
			}
		memcpy(&laserSensorEnergyData,(char *) canHexArrayYsk, canDlc);
		}
//************************  355
		if(canId[1]=='5'){
			char canHexArrayYsk[canDlc];
			for (int canDataIndexYsk=0; canDataIndexYsk<canDlc; canDataIndexYsk++){
				canHexArrayYsk[canDataIndexYsk]=msgcan->data[canDataIndexYsk];
			}
		memcpy(&j5EnergyData,(char *) canHexArrayYsk, canDlc);
		}
//************************  365
		if(canId[1]=='6'){
			char canHexArrayYsk[canDlc];
			for (int canDataIndexYsk=0; canDataIndexYsk<canDlc; canDataIndexYsk++){
				canHexArrayYsk[canDataIndexYsk]=msgcan->data[canDataIndexYsk];
			}
		memcpy(&j6EnergyData,(char *) canHexArrayYsk, canDlc);
		}
//************************  375
		if(canId[1]=='7'){
			char canHexArrayYsk[canDlc];
			for (int canDataIndexYsk=0; canDataIndexYsk<canDlc; canDataIndexYsk++){
				canHexArrayYsk[canDataIndexYsk]=msgcan->data[canDataIndexYsk];
			}
		memcpy(&j7EnergyData,(char *) canHexArrayYsk, canDlc);
		}
	}*/

//************************ DSK-M */
/*
	else if(canId[0]=='4' && canId[2]=='9'){
//************************  409
		if(canId[1]=='0'){
			controlBmpData=std::stoul(canHexArray[0],nullptr,16);
			controlLampData=std::stoul(canHexArray[1],nullptr,16);
			modeSelection=std::stoul(canHexArray[2],nullptr,16);
			emgData=std::stoul(canHexArray[3],nullptr,16);
		}
//************************ 419
		else if(canId[1]=='1'){
			batteryVoltage=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
			batteryTotalCurrent=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
			batteryPercentage=std::stoul(canHexArray[4],nullptr,16);
		}
	}
//************************  422	
	else if(canId[0]=='4' && canId[1]=='2' && canId[2]=='2'){
		loadCell1Data=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
		loadCell2Data=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
		loadCell3Data=std::stoul(canHexArray[5]+canHexArray[4],nullptr,16);
		loadCell4Data=std::stoul(canHexArray[7]+canHexArray[6],nullptr,16);
	}
*/
	//**************************        700	*/
	else if(canId[0]=='7' && canId[1]=='0' && canId[2]=='0'){
		counter=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
		std::cout<<"t "<<counter<<std::endl;
	}

	//**************************        702 */
	else if(canId[0]=='7' && canId[1]=='0' && canId[2]=='2'){
		encoder1data=std::stoul(canHexArray[1]+canHexArray[0],nullptr,16);
		encoder2data=std::stoul(canHexArray[3]+canHexArray[2],nullptr,16);
		encoder1diff = std::stoul(canHexArray[4],nullptr,16);
		if(encoder1diff>=200){
			encoder1diff=(255-encoder1diff+1)*(-1);
		}
		encoder2diff = std::stoul(canHexArray[5],nullptr,16);
		if(encoder2diff>=200){
			encoder2diff=(255-encoder2diff+1)*(-1);}
		robot_counter=std::stoul(canHexArray[7]+canHexArray[6],nullptr,16);
		}
	//**************************        705 */
	else if(canId[0]=='7' && canId[1]=='0' && canId[2]=='5'){
	      char  canHexArrayYskwl[4],canHexArrayYskwr[4];
		for(int canIndexArrayYsk=0; canIndexArrayYsk<4;canIndexArrayYsk++){
			canHexArrayYskwr[canIndexArrayYsk] = msgcan ->data[canIndexArrayYsk];
			canHexArrayYskwl[canIndexArrayYsk] = msgcan ->data[canIndexArrayYsk+4]; 
		}
		memcpy(&robot_wr,(char *) canHexArrayYskwr,4);
		memcpy(&robot_wl,(char *) canHexArrayYskwl,4);

	}
	//**************************        706 */
	else if(canId[0]=='7' && canId[1]=='0' && canId[2]=='6'){
		char  canHexArrayYskwlRef[4],canHexArrayYskwrRef[4];
		for(int canIndexArrayYsk=0; canIndexArrayYsk<4;canIndexArrayYsk++){
			canHexArrayYskwrRef[canIndexArrayYsk] = msgcan ->data[canIndexArrayYsk];
			canHexArrayYskwlRef[canIndexArrayYsk] = msgcan ->data[canIndexArrayYsk+4]; 
			}
		memcpy(&robot_wr_ref,(char *) canHexArrayYskwrRef,4);
		memcpy(&robot_wl_ref,(char *) canHexArrayYskwlRef,4);
	}
	//sil

		else if(canId[0]=='2' && canId[1]=='1' && canId[2]=='5'){
			char canHexArrayYsk[canDlc];
			for (int canDataIndexYsk=0; canDataIndexYsk<canDlc; canDataIndexYsk++){
				canHexArrayYsk[canDataIndexYsk]=msgcan->data[canDataIndexYsk];
			}			
		memcpy(&linear_x,(char *) canHexArrayYsk, canDlc);
		std::cout<<linear_x<<std::endl;
		}
		
}

int main(int argc,char **argv)
{
  //	Txt dosyasının hazırlanması
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
  std::string textname(buffer);

  std::ofstream can_file;
  can_file.open(textname+"robot-can-data.csv");
  std::ostringstream can_txt;
  can_file << "sec,nsec,encoder1data,encoder2data,robot_counter,robot_wl,robot_wr\n";


  std::string tirnak; 
  tirnak=(char)34;
  ros::init(argc, argv, "recievecan");
  int32_t secs,nsecs; 

  ros::NodeHandle n;
  ros::NodeHandle nodePub;
  ros::Subscriber sub = n.subscribe("/received_messages", 1000, canbusCallback);
  
  std::string motor1TopicName="motor_1_data";
  std::string motor2TopicName="motor_2_data";
  std::string controlTopicName="control_data";
/*  
  
  std::string yskMTopicName="DSK_M_Power/YSK_M_data";
  std::string ysgGTopicName="DSK_M_Power/YSK_G_data";
  std::string dskMdTopicName="DSK_M_Power/DSK_MD_data";
  std::string dskMTopicName="DSK_M_Power/DSK_M_data";
  std::string laserSensorTopicName="DSK_M_Power/laser_sensor_data";
  std::string j5TopicName="DSK_M_Power/J5_data";
  std::string j6TopicName="DSK_M_Power/J6_data";
  std::string j7TopicName="DSK_M_Power/J7_data";

  std::string dskMDurumTopicName="DSK_M/DSK_M_data";
  std::string batteryTopicName="DSK_M/battery_data";
  std::string loadTopicName="DSK_M/load_data";
  */
  std::string yukBilgisiJson;
  std::string dskmBilgisiJson;
  
  ros::Publisher dskMdMotor1BilgileriPublisher = nodePub.advertise<agv_msgs::PowerData>(motor1TopicName, 10);
  ros::Publisher dskMdMotor2BilgileriPublisher = nodePub.advertise<agv_msgs::PowerData>(motor2TopicName, 10);
  ros::Publisher dskMdKontrolBilgileriPublisher = nodePub.advertise<agv_msgs::ControlData>(controlTopicName, 10);
  ros::Publisher heartbeatPublisher = nodePub.advertise<std_msgs::Int8>("canbus_heartbeat",1);
/*
  ros::Publisher yskMBilgileriPublisher = nodePub.advertise<agv_msgs::PowerData>(yskMTopicName, 10);
  ros::Publisher yskGBilgileriPublisher = nodePub.advertise<agv_msgs::PowerData>(ysgGTopicName, 10);

  ros::Publisher dskMdBilgileriPublisher = nodePub.advertise<agv_msgs::PowerData>(dskMdTopicName, 10);
  ros::Publisher dskMBilgileriPublisher = nodePub.advertise<agv_msgs::PowerData>(dskMTopicName, 10);
  ros::Publisher laserSensorBilgileriPublisher = nodePub.advertise<agv_msgs::PowerData>(laserSensorTopicName, 10);
  
  ros::Publisher j5BilgileriPublisher = nodePub.advertise<agv_msgs::PowerData>(j5TopicName, 10);
  ros::Publisher j6BilgileriPublisher = nodePub.advertise<agv_msgs::PowerData>(j6TopicName, 10);
  ros::Publisher j7BilgileriPublisher = nodePub.advertise<agv_msgs::PowerData>(j7TopicName, 10);

  ros::Publisher dskMDurumBilgileriPublisher = nodePub.advertise<std_msgs::String>(dskMDurumTopicName, 10);
  ros::Publisher bataryaBilgileriPublisher = nodePub.advertise<agv_msgs::BatteryDatas>(batteryTopicName, 10);
  ros::Publisher yukBilgileriPublisher = nodePub.advertise<std_msgs::String>(loadTopicName, 10);
  */
  ros::Publisher encoderLeftDataPublisher = nodePub.advertise<std_msgs::Int16>("encoder_left",100);
  ros::Publisher encoderRightDataPublisher = nodePub.advertise<std_msgs::Int16>("encoder_right",100);
  ros::Publisher robotLowPublisher = nodePub.advertise<agv_msgs::RobotLow>("robot_low",100);

  ros::Rate loop_rate(140);
  int count = 0;
  while (ros::ok())
  {
	secs = ros::Time::now().sec;
	nsecs = ros::Time::now().nsec;
//********************** Heartbeat */
	std_msgs::Int8 msgHeartbeat;
	msgHeartbeat.data = 1;
	heartbeatPublisher.publish(msgHeartbeat);
//*********************** 102	*/
	agv_msgs::ControlData msgDskMdControlData;
	msgDskMdControlData.temperature=temperatureData/1000;
	msgDskMdControlData.led_status=ledStatus;
	msgDskMdControlData.fault_t=fault_t;
	msgDskMdControlData.fault_a4940=fault_a4940;
	msgDskMdControlData.emg_button=emg_button;
	
	dskMdKontrolBilgileriPublisher.publish(msgDskMdControlData);
	  
//*********************** 109	  */
	agv_msgs::PowerData msgDskMdMotor1Data;
	msgDskMdMotor1Data.voltage=motor1VoltageData/1000;
	msgDskMdMotor1Data.current=motor1CurrentData/1000;
	msgDskMdMotor1Data.power=motor1PowerData/1000;
	msgDskMdMotor1Data.energy =motor2Energy;
	dskMdMotor1BilgileriPublisher.publish(msgDskMdMotor1Data);

//*********************** 119*/
	agv_msgs::PowerData msgDskMdMotor2Data;
	msgDskMdMotor2Data.voltage=motor2VoltageData/1000;
	msgDskMdMotor2Data.current=motor2CurrentData/1000;
	msgDskMdMotor2Data.power=motor2PowerData/1000;
	msgDskMdMotor2Data.energy =motor2Energy;
	dskMdMotor2BilgileriPublisher.publish(msgDskMdMotor2Data);
/*
//*********************** 309 - 305
	agv_msgs::PowerData msgYSKMData;
	msgYSKMData.voltage=yskmVoltageData/1000;
	msgYSKMData.current=yskmCurrentData/1000;
	msgYSKMData.power=yskmPowerData/1000;
	msgYSKMData.energy=yskmEnergyData;
	
	yskMBilgileriPublisher.publish(msgYSKMData);
//*********************** 319 - 315
	agv_msgs::PowerData msgYSKGData;
	msgYSKGData.voltage=yskgVoltageData/1000;
	msgYSKGData.current=yskgCurrentData/1000;
	msgYSKGData.power=yskgPowerData/1000;
	msgYSKGData.energy=yskgEnergyData;

	yskGBilgileriPublisher.publish(msgYSKGData);
//*********************** 329 - 325
	agv_msgs::PowerData msgDSKMDData;
	msgDSKMDData.voltage=dskmdVoltageData/1000;
	msgDSKMDData.current=dskmdCurrentData/1000;
	msgDSKMDData.power=dskmdPowerData/1000;
	msgDSKMDData.energy=dskmdEnergyData;

dskMdBilgileriPublisher.publish(msgDSKMDData);
//*********************** 339 - 335
	agv_msgs::PowerData msgDSKMData;
	msgDSKMData.voltage=dskmVoltageData/1000;
	msgDSKMData.current=dskmCurrentData/1000;
	msgDSKMData.power=dskmPowerData/1000;
	msgDSKMData.energy=dskmEnergyData;

dskMBilgileriPublisher.publish(msgDSKMData);
//*********************** 349 - 345
	agv_msgs::PowerData msgLaserSensorData;
	msgLaserSensorData.voltage=laserSensorVoltageData/1000;
	msgLaserSensorData.current=laserSensorCurrentData/1000;
	msgLaserSensorData.power=laserSensorPowerData/1000;
	msgLaserSensorData.energy=laserSensorEnergyData;

laserSensorBilgileriPublisher.publish(msgLaserSensorData);

//*********************** 359
	agv_msgs::PowerData msgJ5Data;
	msgJ5Data.voltage=j5VoltageData/1000;
	msgJ5Data.current=j5CurrentData/1000;
	msgJ5Data.power=j5PowerData/1000;
	msgJ5Data.energy=j5EnergyData;

j5BilgileriPublisher.publish(msgJ5Data);
//*********************** 369
	agv_msgs::PowerData msgJ6Data;
	msgJ6Data.voltage=j6VoltageData/1000;
	msgJ6Data.current=j6CurrentData/1000;
	msgJ6Data.power=j6PowerData/1000;
	msgJ6Data.energy=j6EnergyData;

j6BilgileriPublisher.publish(msgJ6Data);
//*********************** 379
	agv_msgs::PowerData msgJ7Data;
	msgJ7Data.voltage=j7VoltageData/1000;
	msgJ7Data.current=j7CurrentData/1000;
	msgJ7Data.power=j7PowerData/1000;
	msgJ7Data.energy=j7EnergyData;

j7BilgileriPublisher.publish(msgJ7Data);

//*********************** 409
	dskmBilgisiJson = "{" + tirnak + "Bmp Kontrol" + tirnak + ":" + tirnak + std::to_string(controlBmpData) + tirnak + ","
						+ tirnak + "Lamba Bilgisi" + tirnak + ":" +tirnak + std::to_string(controlLampData) + tirnak + ","
						+ tirnak + "Mod" + tirnak + ":" + tirnak + std::to_string(modeSelection) + tirnak + ","
						+ tirnak + "Acil Durum" + tirnak + ":" + tirnak + std::to_string(emgData) + tirnak + "}";
	std_msgs::String msgDSKMDurumData;
	msgDSKMDurumData.data = dskmBilgisiJson;

	dskMDurumBilgileriPublisher.publish(msgDSKMDurumData);
//*********************** 419
	agv_msgs::BatteryData msgBatteryData;
	msgBatteryData.voltage=batteryVoltage/1000;
	msgBatteryData.total_current=batteryTotalCurrent/1000;
	msgBatteryData.percentage=batteryPercentage/1000;

	bataryaBilgileriPublisher.publish(msgBatteryData);
//*********************** 422
	yukBilgisiJson = "{" + tirnak + "Hucre 1" + tirnak + ":" + tirnak + std::to_string(loadCell1Data/1000) + tirnak + ","
						 + tirnak + "Hucre 2" + tirnak + ":" + tirnak + std::to_string(loadCell2Data/1000) + tirnak + ","
						 + tirnak + "Hucre 3" + tirnak + ":" + tirnak + std::to_string(loadCell3Data/1000) + tirnak + ","
						 + tirnak + "Hucre 4" + tirnak + ":" + tirnak + std::to_string(loadCell4Data/1000) + tirnak + "}";
	std_msgs::String msgLoadData;
	msgLoadData.data = yukBilgisiJson;

	yukBilgileriPublisher.publish(msgLoadData);
*/
//********************702*/
	std_msgs::Int16 msgEncoderLeftData;
	msgEncoderLeftData.data = encoder2data;

	encoderLeftDataPublisher.publish(msgEncoderLeftData);

//*********************702*/
	std_msgs::Int16 msgEncoderRightData;
	msgEncoderRightData.data = encoder1data;

	encoderRightDataPublisher.publish(msgEncoderRightData);

//*******************702-705-706*/
	agv_msgs::RobotLow msgRobotLow;
	msgRobotLow.encoder_left = encoder2data;
	msgRobotLow.encoder_right = encoder1data;
	msgRobotLow.encoder_left_diff = encoder2diff;
	msgRobotLow.encoder_right_diff = encoder1diff;
	msgRobotLow.robot_counter = robot_counter;
	msgRobotLow.robot_wl = robot_wl;
	msgRobotLow.robot_wr = robot_wr;
	msgRobotLow.robot_wl_ref = robot_wl_ref;
	msgRobotLow.robot_wr_ref = robot_wr_ref;

	robotLowPublisher.publish(msgRobotLow);
	
	can_txt  << secs <<","<<nsecs<<","<< encoder1data << ","<< encoder2data << "," << robot_counter<<","<<robot_wl <<","<<linear_x;
	can_file << can_txt.str() << std::endl;
	can_txt.str("");
	
	ros::spinOnce();
	loop_rate.sleep();
	count++;
 }
  can_file.close();
  ros::spin();
  return 0;
}

