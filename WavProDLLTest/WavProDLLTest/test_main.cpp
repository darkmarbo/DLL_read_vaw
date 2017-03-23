
#include <iostream>
#include <stdio.h>
#include "WaveFile.h"
using namespace std;

#pragma comment(lib, "WavProDLL.lib")

int main(int argc, char *argv[])
{
	int ret = 0;
	int sampleRate = 0; // ����Ƶ�� 
	int datalength = 0; // ���������
	int num_chanel = 1; // ͨ������
	float data_time = 0.0; // ����ʱ�� �� 
	WaveFile inWav;
	
	ret = inWav.OpenFile(argv[1], 0);
	if (ret <0)
	{
		cout << "open wav err!" << endl;
		return 0;
	}
	
	datalength = (int)inWav.GetTotalSample();
	sampleRate = (int)inWav.GetSamplesPerSec();
	num_chanel = inWav.GetChannelNum();
	data_time = float(datalength) / float(sampleRate);
	

	// ��ͨ�� 
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
	outWav.OpenFile(argv[2], 1, false, sampleRate, num_chanel);

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