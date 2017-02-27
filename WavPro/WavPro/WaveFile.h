#ifndef _WAVE_FILE_H_
#define _WAVE_FILE_H_
#pragma once

#include<stdio.h>
#include<stdlib.h>
#define MAX_PATH 260

//enum WaveFormat
//{
//	WF_NONE = 0,
//	WF_PCM_S16LE
//};

//enum SeekOpt
//{
//	WF_FILE_SET=0,
//	WF_FILE_CUR,
//	WF_FILE_END,
//	WF_DATA_SET,
//	WF_DATA_END
//};

#define WF_NONE 0
#define WF_PCM_S16LE 1

#define WF_FILE_SET 0
#define WF_FILE_CUR 1
#define WF_FILE_END 2
#define WF_DATA_SET 3
#define WF_DATA_END 4

class WaveFile
{
private:
	FILE * fptr;
	char fileName[MAX_PATH];
	bool isRead;
	int waveFormat;
	fpos_t dataPos;
	unsigned int fileSize;
	unsigned int fmtSize;
	unsigned short fmtTag;
	unsigned short nChannel;
	unsigned int nSamplesPerSec;
	unsigned int nAvgBytesPerSec;
	unsigned short nBlockAlign;
	unsigned short wBitsPerSample;
	unsigned short cbSize;
	unsigned int dataSize;

	void SwapMemory(void * buffer, int size);
public:
	WaveFile(void);
	~WaveFile(void);
	//File operation
	int OpenFile(char* sFileName, int _waveFormat=WF_NONE, bool _isRead=true, unsigned int _nSamplesPerSec=0, unsigned short _nChannel=0);
	void CloseFile(void);
	void FlushFile(void);
	void SeekFile(int offset,int seekOpt);
	//Data retrive
	bool FindChuck(const char* chuckName, int chuckSize=4, bool restart=false);
	bool CreateData(short** &data, int size);
	bool DestoryData(short** &data);
	int GetData(short ** data, int size);
	int PutData(short ** data, int dataChannelCnt, int size, int startNum=0);
	int GetDataBySampleIndex(short ** data, int size, unsigned int startSampleIndex, unsigned int endSampleIndex=-1);
	int GetDataByTime(short ** data, int size, double startTime, double endTime=-1.0);
	//Get Basic Informations.
	unsigned int GetSamplesPerSec(void);
	unsigned short GetChannelNum(void);
	unsigned short GetBitsPerSample(void);
	unsigned short GetBytesPerSample(void);
	unsigned int GetAvgBytesPerSec(void);
	unsigned short GetBlockAlign(void);	
	unsigned int GetTotalSample(void);
	unsigned int GetDataSize(void);
};
#endif //_WAVE_FILE_H_