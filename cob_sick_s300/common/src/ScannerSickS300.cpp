/*
 * Copyright 2017 Fraunhofer Institute for Manufacturing Engineering and Automation (IPA)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 

#include <cob_sick_s300/ScannerSickS300.h>

#include <stdint.h>

//-----------------------------------------------

typedef unsigned char BYTE;

const double ScannerSickS300::c_dPi = 3.14159265358979323846;
unsigned char ScannerSickS300::m_iScanId = 7;

const unsigned short crc_LookUpTable[256]
	   = {
		0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
		0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
		0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
		0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
		0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
		0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
		0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
		0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
		0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
		0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
		0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
		0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
		0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
		0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
		0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
		0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
		0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
		0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
		0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
		0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
		0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
		0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
		0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
		0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
		0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
		0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
		0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
		0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
		0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
		0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
		0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
	 };

unsigned int TelegramParser::createCRC(uint8_t *ptrData, int Size)
{
	int CounterWord;
	unsigned short CrcValue=0xFFFF;

	for (CounterWord = 0; CounterWord < Size; CounterWord++)
	{
		CrcValue = (CrcValue << 8) ^ crc_LookUpTable[ (((uint8_t)(CrcValue >> 8)) ^ *ptrData) ];
		ptrData++;
	}

	return (CrcValue);
}

//-----------------------------------------------
ScannerSickS300::ScannerSickS300()
{
	// allows to set different Baud-Multipliers depending on used SerialIO-Card
	m_dBaudMult = 1.0;

	// init scan with zeros
	m_iPosReadBuf2 = 0;

	m_actualBufferSize = 0;

	m_bInStandby = true;

}


//-------------------------------------------
ScannerSickS300::~ScannerSickS300()
{
	m_SerialIO.closeIO();
}


// ---------------------------------------------------------------------------
bool ScannerSickS300::open(const char* pcPort, int iBaudRate, int iScanId=7)
{
    int bRetSerial;

	// update scan id (id=8 for slave scanner, else 7)
	m_iScanId = iScanId;

	// initialize Serial Interface
	m_SerialIO.setBaudRate(iBaudRate);
	m_SerialIO.setDeviceName(pcPort);
	m_SerialIO.setBufferSize(READ_BUF_SIZE - 10 , WRITE_BUF_SIZE -10 );
	m_SerialIO.setHandshake(SerialIO::HS_NONE);
	m_SerialIO.setMultiplier(m_dBaudMult);
	bRetSerial = m_SerialIO.openIO();
	m_SerialIO.setTimeout(0.0);
	m_SerialIO.SetFormat(8, SerialIO::PA_NONE, SerialIO::SB_ONE);

    if(bRetSerial == 0)
    {
	    // Clears the read and transmit buffer.
	    m_iPosReadBuf2 = 0;
	    m_SerialIO.purge();
	    return true;
    }
    else
    {
        return false;
    }
}


//-------------------------------------------
void ScannerSickS300::purgeScanBuf()
{
	m_iPosReadBuf2 = 0;
	m_SerialIO.purge();
}


//-------------------------------------------
void ScannerSickS300::resetStartup()
{
}


//-------------------------------------------
void ScannerSickS300::startScanner()
{
}


//-------------------------------------------
void ScannerSickS300::stopScanner()
{
}

//-----------------------------------------------
bool ScannerSickS300::getScan(std::vector<double> &vdDistanceM, std::vector<double> &vdAngleRAD, std::vector<double> &vdIntensityAU, unsigned int &iTimestamp, unsigned int &iTimeNow, const bool debug)
{
	bool bRet = false;
	int i;
	int iNumRead2 = 0;
	std::vector<ScanPolarType> vecScanPolar;
	//printf("***************\n");
	iTimeNow=0;

	if(SCANNER_S300_READ_BUF_SIZE-2-m_actualBufferSize<=0)
		m_actualBufferSize=0;
//	printf("SCANNER_S300_READ_BUF_SIZE-2-m_actualBufferSize : %d\n", SCANNER_S300_READ_BUF_SIZE-2-m_actualBufferSize);
	iNumRead2 = m_SerialIO.readBlocking((char*)m_ReadBuf+m_actualBufferSize, SCANNER_S300_READ_BUF_SIZE-2-m_actualBufferSize);
//	printf("iNUMREAD2 : %d\n",iNumRead2);
	if(iNumRead2<=0) return false;

	m_actualBufferSize = m_actualBufferSize + iNumRead2;

	// Try to find scan. Searching backwards in the receive queue.
//	printf("m_actualBufferSize     :   %d\n", m_actualBufferSize);
	for(i=m_actualBufferSize; i>=0; i--)
	{
		// parse through the telegram until header with correct scan id is found
		if(tp_.parseHeader(m_ReadBuf+i, m_actualBufferSize-i, m_iScanId, debug))
		{
			tp_.readDistRaw(m_ReadBuf+i, m_viScanRaw, debug);
	//		printf("m_viScanRaw : %d",m_viScanRaw);
			if(m_viScanRaw.size()>0) {
				// Scan was succesfully read from buffer
				bRet = true;
				int old = m_actualBufferSize;
				m_actualBufferSize -= tp_.getCompletePacketSize()+i;
				for(int i=0; i<old-m_actualBufferSize; i++)
				    m_ReadBuf[i] = m_ReadBuf[i+old-m_actualBufferSize];
				break;
			}
		//	printf("+++++++++++++++++\n");
		}
	}

	PARAM_MAP::const_iterator param = m_Params.find(tp_.getField());
	if(bRet && param!=m_Params.end())
	{
		// convert data into range and intensity information
		convertScanToPolar(param, m_viScanRaw, vecScanPolar);
	//	printf("--------------------");
		// resize vectors to size of Scan
		vdDistanceM.resize(vecScanPolar.size());
		vdAngleRAD.resize(vecScanPolar.size());
		vdIntensityAU.resize(vecScanPolar.size());
		// assign outputs
		for(unsigned int i=0; i < vecScanPolar.size(); i++)
		{
			vdDistanceM[i] = vecScanPolar[i].dr;
			vdAngleRAD[i] = vecScanPolar[i].da;
			vdIntensityAU[i] = vecScanPolar[i].di;
		}
	}
	//printf("////////////////bRet : %d\n",bRet);
	return bRet;
}

//-------------------------------------------
void ScannerSickS300::convertScanToPolar(const PARAM_MAP::const_iterator param, std::vector<int> viScanRaw,
							std::vector<ScanPolarType>& vecScanPolar )
{
	double dDist;
	double dAngle, dAngleStep;
	double dIntens;
	bool bInStandby = true;

	vecScanPolar.resize(viScanRaw.size());
	dAngleStep = fabs(param->second.dStopAngle - param->second.dStartAngle) / double(viScanRaw.size() - 1) ;


	for(size_t i=0; i<viScanRaw.size(); i++)
	{
		dDist = double ((viScanRaw[i] & 0x1FFF) * param->second.dScale);

		// if not all values are 0x4004 , we are not in standby
		if ( !(viScanRaw[i] == 0x4004) )
			bInStandby = false;

		dAngle = param->second.dStartAngle + i*dAngleStep;
		dIntens = double(viScanRaw[i] & 0x2000);

		vecScanPolar[i].dr = dDist;
		vecScanPolar[i].da = dAngle;
		vecScanPolar[i].di = dIntens;
	}

	m_bInStandby = bInStandby;
}
