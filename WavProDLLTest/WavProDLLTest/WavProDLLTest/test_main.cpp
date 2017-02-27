
#include <iostream>
#include <stdio.h>
#include "WaveFile.h"
using namespace std;

#pragma comment(lib, "WavProDLL.lib")

int main(int argc, char *argv[])
{
	int ret = 0;
	WaveFile inWav;
	
	ret = inWav.OpenFile(argv[1], 0);
	if (ret <0)
	{
		cout << "open wav err!" << endl;
		return 0;
	}
	int datalength = (int)inWav.GetTotalSample();
	int sampleRate = (int)inWav.GetSamplesPerSec();
	float data_time = float(datalength) / float(sampleRate);
	int num_chanel = inWav.GetChannelNum();

	// µ¥ÉùµÀ 
	short **data = NULL;
	data = new short*[num_chanel];
	for (int ii = 0; ii < num_chanel; ii++)
	{
		data[ii] = new short[datalength];
	}
	//short *data = new short[datalength];

	datalength = inWav.GetData(data, datalength);


	printf("data_time=%.4f\tsampleRate=%d\n", data_time, sampleRate);

	// write wav 
	//int WaveFile::OpenFile(char _fileName[260], int _waveFormat, bool _isRead, unsigned int _nSamplesPerSec, unsigned short _nChannel)
	WaveFile outWav;
	outWav.OpenFile(argv[2], 1, false, sampleRate, 1);

	//outWav.OpenFile(outWavName, inWav.WaveFileFormat, false, (uint)sampleRate, (ushort)data.GetLength(0));
	//int WaveFile::PutData(short ** data, int dataChannelCnt, int size, int startNum)
	outWav.PutData(data, num_chanel, datalength / 2, 0);
	outWav.FlushFile();
	outWav.CloseFile();
	inWav.DestoryData(data);
	inWav.CloseFile();

	printf("completed!\n");
	getchar();
	return 0;

}