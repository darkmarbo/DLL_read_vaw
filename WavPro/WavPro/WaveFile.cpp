#include "WaveFile.h"
#include<string.h>

WaveFile::WaveFile(void)
{
	fptr=NULL;
	fileName[0]='\0';
	isRead=true;
}

WaveFile::~WaveFile(void)
{
	if(fptr) fclose(fptr);
}

unsigned int WaveFile::GetSamplesPerSec(void)
{
	if(fptr==NULL) return 0;
	else return nSamplesPerSec;
}

unsigned short WaveFile::GetChannelNum(void)
{
	if(fptr==NULL) return 0;
	else return nChannel;
}

unsigned short WaveFile::GetBitsPerSample(void)
{
	if(fptr==NULL) return 0;
	else return wBitsPerSample;
}

unsigned short WaveFile::GetBytesPerSample(void)
{
	if(fptr==NULL) return 0;
	else return wBitsPerSample/8;
}

unsigned int WaveFile::GetAvgBytesPerSec(void)
{
	if(fptr==NULL) return 0;
	else return nAvgBytesPerSec;
}

unsigned short WaveFile::GetBlockAlign(void)
{
	if(fptr==NULL) return 0;
	else return nBlockAlign;
}

//Open an wav file for read or write (Create if possible)
//char _fileName[260]: the full path of the file, could be relative to the exe file.
//int _waveFormat: the WaveFormat enum value of an waveFormat.[Default WF_NONE] 
//				   For reading: If _waveFormat is set to WF_NONE then the detected format is used.(Little Endian By Default)
//								If _waveFormat is not WF_NONE and the detected format is different then -2 is returned, else the file will be load. 
//								If the file is raw and _waveFormat is not WF_NONE, then use the _waveFormat as the file format.
//								If the file is raw and _waveFormat is WF_NONE, then -3 is returned.
//				   For writing: Write the file in _waveFormat, WF_NONE means a raw type.
//bool _isRead: Set to true if the file is for read, or false for write.[Default true]
//unsigned int _nSamplesPerSec: For Reading set 0 to use the auto detect, or nonezero to check the autoselect, if they are not same, -2 is returned.
//								For Writing can not be zero, set the sampple rate.
//unsigend short _nChannel: For Reading set 0 to use the auto detect, or nonezero to check the autoselect, if they are not same, -2 is returned.
//							For Writing can not be zero, set the channel number.
//Retruns: 0---Success
//		   -1---File could not be open.
//		   -2---Format not match(Read mode).
//		   -3---Format not detected(Read mode).
//         -4---File format not support.
//		   -5---No data found.
//		   -6---Unsupport samplerate.(Write Mode).
//		   -7---Unsupport channel number;
int WaveFile::OpenFile(char _fileName[260], int _waveFormat, bool _isRead, unsigned int _nSamplesPerSec, unsigned short _nChannel)
{
	isRead = _isRead;
	bool isLB = true;
	if(isRead) fopen_s(&fptr,_fileName, "rb");
	else fopen_s(&fptr,_fileName, "wb");
	if(fptr==NULL) return -1;	//File could not open.
	else strcpy_s(fileName,_fileName);
	if(isRead)
	{
		int detectFormat;

		if(!FindChuck("RIFF"))
		{
			fclose(fptr);
			return -4; //File format not support.
		}

		fread(&fileSize,4,1,fptr);
		if(!isLB)SwapMemory(&fileSize,4);

		if(!FindChuck("WAVE"))
		{
			fclose(fptr);
			return -4; //File format not support.
		}

		if(!FindChuck("fmt "))
		{
			fclose(fptr);
			return -4; //File format not support.
		}

		fread(&fmtSize,4,1,fptr);
		if(!isLB)SwapMemory(&fmtSize,4);
		fread(&fmtTag,2,1,fptr);
		if(!isLB)SwapMemory(&fmtTag,2);
		fread(&nChannel,2,1,fptr);
		if(!isLB)SwapMemory(&nChannel,2);
		fread(&nSamplesPerSec,4,1,fptr);
		if(!isLB)SwapMemory(&nSamplesPerSec,4);
		fread(&nAvgBytesPerSec,4,1,fptr);
		if(!isLB)SwapMemory(&nAvgBytesPerSec,4);
		fread(&nBlockAlign,2,1,fptr);
		if(!isLB)SwapMemory(&nBlockAlign,2);
		fread(&wBitsPerSample,2,1,fptr);
		if(!isLB)SwapMemory(&wBitsPerSample,2);

		if(_nSamplesPerSec!=0 && nSamplesPerSec!=_nSamplesPerSec)
		{
			fclose(fptr);
			return -2;	//Format not match(Read mode).
		}
		if(_nChannel!=0 && nChannel!=_nChannel)
		{
			fclose(fptr);
			return -2;	//Format not match(Read mode).
		}

		if(fmtSize==18U || fmtSize==16U)
		{
			if(fmtTag==1U)	//PCM
			{
				switch(wBitsPerSample)
				{
					case 16:
						if(isLB) detectFormat=WF_PCM_S16LE;
						else detectFormat = WF_NONE;
						break;
					default:
						detectFormat = WF_NONE;
				}
			}
			else
			{
				fclose(fptr);
				return -4;	//File format not support.
			}
			if(fmtSize==18U) 
			{
				fread(&cbSize,2,1,fptr);
				if(!isLB) SwapMemory(&cbSize,2);
			}
		}
		else
		{
			fclose(fptr);
			return -4;	//File format not support.
		}

		if(_waveFormat==WF_NONE)
		{
			if(detectFormat!=WF_NONE) waveFormat = detectFormat;
			else
			{
				fclose(fptr);
				return -3;	//Format not detected(Read mode).
			}
		}
		else
		{
			if(_waveFormat==detectFormat) waveFormat = detectFormat;
			else
			{
				fclose(fptr);
				return -2;	//Format not match(Read mode).
			}
		}

		if(!FindChuck("data"))
		{
			fclose(fptr);
			return -5; //No data found.
		}

		fread(&dataSize,4,1,fptr);
		if(!isLB)SwapMemory(&dataSize,4);
	}//ReadMode if finish successful, fptr points the first data, and waveFormat is not WF_NONE
	else
	{
		if(_nSamplesPerSec==0)
		{
			fclose(fptr);
			return -6;//Unsupport samplerate.(Write Mode).
		}
		if(_nChannel==0)
		{
			fclose(fptr);
			return -7;//Unsupport channel number;
		}

		char chuckName[5]={0};
		unsigned int num=0;
		unsigned short snum=0;
		switch(_waveFormat)
		{
			case WF_PCM_S16LE:
				fmtSize = 16U;
				fmtTag = 1U;
				nChannel = _nChannel;
				nSamplesPerSec = _nSamplesPerSec;
				wBitsPerSample = 16;
				nBlockAlign = nChannel*wBitsPerSample/8;
				nAvgBytesPerSec = nSamplesPerSec*nBlockAlign;
				fseek(fptr,0,SEEK_SET);
				strcpy_s(chuckName,5,"RIFF");
				fwrite(chuckName,1,4,fptr);
				num=36;
				fwrite(&num,4,1,fptr);
				strcpy_s(chuckName,5,"WAVE");
				fwrite(chuckName,1,4,fptr);
				strcpy_s(chuckName,5,"fmt ");
				fwrite(chuckName,1,4,fptr);
				fwrite(&fmtSize,4,1,fptr);
				fwrite(&fmtTag,2,1,fptr);
				fwrite(&nChannel,2,1,fptr);
				fwrite(&nSamplesPerSec,4,1,fptr);
				fwrite(&nAvgBytesPerSec,4,1,fptr);
				fwrite(&nBlockAlign,2,1,fptr);
				fwrite(&wBitsPerSample,2,1,fptr);
				strcpy_s(chuckName,5,"data");
				fwrite(chuckName,1,4,fptr);
				num=0;
				fwrite(&num,4,1,fptr);
				fflush(fptr);//finish header
				fileSize=36;
				dataSize=0;
				waveFormat = WF_PCM_S16LE;
				break;
			case WF_NONE:
				nSamplesPerSec = _nSamplesPerSec;
				nChannel = _nChannel;
				wBitsPerSample = 8;
				nBlockAlign = nChannel*wBitsPerSample/8;
				nAvgBytesPerSec = nSamplesPerSec*nBlockAlign;
				fileSize=0;
				dataSize=0;
				waveFormat = WF_NONE;
				break;
			default:
				fclose(fptr);
				return -4;//File format not support.
		}
	}//WriteMode if finish successful, fptr points the end of header, ant waveFormat is not WF_NONE

	fgetpos(fptr,&dataPos);//remember the start pos of data
	return 0;
}
//Flush the opened file. Only use in write mode
void WaveFile::FlushFile(void)
{
	if(fptr==NULL) return;
	fflush(fptr);
	switch(waveFormat)
	{
		case WF_PCM_S16LE:
			fseek(fptr,4,SEEK_SET);
			fwrite(&fileSize,4,1,fptr);
			fseek(fptr,40,SEEK_SET);
			fwrite(&dataSize,4,1,fptr);
			fseek(fptr,0,SEEK_END);
			fflush(fptr);
			break;
		default:
			;//do nothing.
	}
}


//Close the opened file. REMEMBER to free all memory allocated by CreateData by using DestoryData().
void WaveFile::CloseFile(void)
{
	if(fptr==NULL) return;
	if(!isRead) FlushFile();
	fclose(fptr);
	fptr=NULL;
}

//Swap the bype order in a memory.
void WaveFile::SwapMemory(void * buffer, int size)
{
	if(size<=1) return;
	char * p = (char*)buffer;
	for(int i=0; i<size/2; i++)
	{
		char temp = p[i];
		p[i] = p[size-i-1];
		p[size-i-1] = temp;
	}
	return;
}

//Get waveData in short format.
//short ** data: The buffer to recieve data, you shoud Use CreateData() to initialze data before passing it,
//				 and DestoryData() to free the memory. The format is data[CHANNEL_NUM][BUFFER_LENGTH].
//				 Self memory management is not recommand, use GreateData() and DestoryData() instead.
//int size: The maxium number of data element that you  want to read.
//return the number of element readed in data
int WaveFile::GetData(short** data, int size)
{
	if(fptr==NULL || size==0) return 0;
	int readNum=0;
	char * buffer = (char *)malloc(nBlockAlign);
	switch(waveFormat)
	{
		case WF_PCM_S16LE:
			while(readNum<size)
			{
				short temp;
				if(fread_s(buffer,nBlockAlign,1,nBlockAlign,fptr)!=nBlockAlign) break;
				for(int i=0; i<nChannel; i++)
				{
					memcpy(&temp,buffer+i*wBitsPerSample/8,2);
					data[i][readNum]=temp;
				}
				readNum++;
			}
			break;
		default:
			;	//do nothing
	}
	free(buffer);
	return readNum;;
}
//Put waveData in short format into waveFile.
//short ** data: The buffer containing data, you shoud Use CreateData() to initialze data before passing it,
//				 and DestoryData() to free the memory. The format is data[CHANNEL_NUM][BUFFER_LENGTH].
//				 Self memory management is not recommand, use CreateData() and DestoryData() instead.
//int size: The maxium number of data element that you  want to put into file.
//int maxNum: Use data[startNum] istead of data[0] as the begining to fill every track of the output wave. 
//            If data has not enougth channels, 0 data will be filled.
//return the number of element put from data
int WaveFile::PutData(short ** data, int dataChannelCnt, int size, int startNum)
{
	char zeroData[2] = {0x00, 0x00};

	if(fptr==NULL || isRead) return 0;
	int putNum=0;

	if(startNum<0) startNum=0;

	switch(waveFormat)
	{	
		case WF_PCM_S16LE:
			while(putNum<size)
			{
				for(int i=0; i<nChannel; i++) 
				{
					if(i+startNum < dataChannelCnt)
						fwrite(&data[i+startNum][putNum],2,1,fptr);
					else fwrite(zeroData,1,2,fptr);
				}
				putNum++;
			}
			break;
		case WF_NONE:
			while(putNum<size)
			{
				for(int i=0; i<nChannel; i++) 
				{
					if(i+startNum < dataChannelCnt)
						fwrite(&data[i+startNum][putNum],2,1,fptr);
					else fwrite(zeroData,1,2,fptr);
				}
				putNum++;
			}
			break;
		default:
			;//do nothing
	}
	fileSize += putNum * nBlockAlign;
	dataSize += putNum * nBlockAlign;
	return putNum;
}
//Create a data for functions like GetDataInShort();
//short ** &data: A reference to a 2 degree short array to hold the data.
//int size: The length of data in the last degree.
//return: True if data is created succesfully, or false is anything going wrong, but previews data might be freed.
bool WaveFile::CreateData(short** &data, int size)
{
	if(fptr==NULL) return false;
	if(data!=NULL)
	{
		if(DestoryData(data)) data=NULL;
		else return false;
	}

	data = new short*[nChannel];
	for(int i=0; i<nChannel; i++) data[i] = new short[size];
	return true;
}
//Destroty a data for functions like GetDataInShort();
//short** data
bool WaveFile::DestoryData(short** &data)
{
	if(fptr==NULL) return false;
	if(data!=NULL)
	{
		for(int i=0; i<nChannel; i++)
			if(data[i]!=NULL) delete[] data[i];
		delete[] data;
	}
	data=NULL;
	return true;
}

//Find the chuck that chuckName specified.
//const char * chuckName: The name of the chuck, case sensitive. e.g. "RIFF" "fmt "
//int chuckSize: The length of chuckName.[Default: 4]
//bool restart: If the search should restart. [Default: false]
//              True if you want to search the chuck from the beginning of the file.
//				False if you want to search from the current pos of the file.
//return: True if the chuck is found, and fptr locates after the chuck string.
//		  False if the chuck is not found, and fptr locates when you call the function.
bool WaveFile::FindChuck(const char* chuckName, int chuckSize, bool restart)
{
	if(fptr==NULL) return false;
	fpos_t filePos;
	fgetpos(fptr,&filePos);
	if(restart) fseek(fptr,0,SEEK_SET);

	int * KMPList = (int *)malloc(chuckSize*sizeof(int));

	//Calculate KMP next array.
	KMPList[0]=-1;
	int i=0, j=-1;
	while(i<chuckSize-1)
	{
		if(j==-1 || chuckName[i]==chuckName[j])
		{
			i++;j++;
			if(chuckName[i]!=chuckName[j]) KMPList[i]=j;
			else KMPList[i]=KMPList[j];
		}
		else j=KMPList[j];
	}

	char c;
	if(!fread(&c,1,1,fptr))
	{
		free(KMPList);
		fsetpos(fptr,&filePos);
		return false;
	}

	bool isFound=false;
	i=0;
	while(!isFound && !feof(fptr))
	{
		if(c==chuckName[i])
		{
			i++;
			if(i==chuckSize) isFound=true;
			else if(!fread(&c,1,1,fptr)) break;
		}
		else
		{
			if(KMPList[i]==-1)
			{
				if(!fread(&c,1,1,fptr)) break;
				i=0;
			}
			else i=KMPList[i];
		}
	}

	free(KMPList);

	if(!isFound) fsetpos(fptr,&filePos);
	return isFound;
}

unsigned int WaveFile::GetTotalSample(void)
{
	if(fptr==NULL) return 0;
	else return dataSize/nBlockAlign;
}

unsigned int WaveFile::GetDataSize(void)
{
	if(fptr==NULL) return 0;
	else return dataSize;
}



//Change the curret stream positon(might be very slow, use as least as you can)
//int offset: the relative offset of the given seekOpt.
//int seekOpt: use One of the SeekOpe enum.
//			   FILE_SET: The begining of the file.
//			   FILE_CUR: The current place of the stream.
//			   FILE_END: The end of the file.
//			   WF_DATA_SET: The begining of the data part.
//			   DATA_END: The end of the data part.
void WaveFile::SeekFile(int offset,int seekOpt)
{
	if(fptr==NULL) return;
	switch(seekOpt)
	{
		case WF_FILE_SET:
			fseek(fptr,offset,SEEK_SET);
			break;
		case WF_FILE_CUR:
			fseek(fptr,offset,SEEK_CUR);
			break;
		case WF_FILE_END:
			fseek(fptr,offset,SEEK_END);
			break;
		case WF_DATA_SET:
			fsetpos(fptr,&dataPos);
			fseek(fptr,offset,SEEK_CUR);
			break;
		case WF_DATA_END:
			fsetpos(fptr,&dataPos);
			fseek(fptr,dataSize-offset,SEEK_CUR);
			break;
		default:
			;	//Do nothing.
	}
}

//Get the data form startSampleIndex till endSampleIndex, endSampleIndex is not included.
//short ** data: The buffer containing data, you shoud Use CreateData() to initialze data before passing it,
//				 and DestoryData() to free the memory. The format is data[CHANNEL_NUM][BUFFER_LENGTH].
//				 Self memory management is not recommand, use GreateData() and DestoryData() instead.
//int size: The maxium number of data element that you  want to put into file.
//unsigned int startSampleIndex: The sample index that start form the data. 0 started
//unsigned int endSampleIndex: The sample index that end to the data, not includes. -1 for no limit.
//return the number of element put from data.
int WaveFile::GetDataBySampleIndex(short ** data, int size, unsigned int startSampleIndex, unsigned int endSampleIndex)
{
	if(fptr==NULL || startSampleIndex<0) return 0;
	int readNum=0;
	int readLength;
	if(endSampleIndex<0) readLength=size;
	else
	{
		if(startSampleIndex>endSampleIndex)
		{
			readLength = startSampleIndex;
			startSampleIndex = endSampleIndex;
			endSampleIndex = readLength;
		}
		readLength=endSampleIndex-startSampleIndex;
		if(readLength>size) readLength=size;
	}
	SeekFile(startSampleIndex*nBlockAlign/8,WF_DATA_SET);
	char * buffer = (char *)malloc(nBlockAlign);
	switch(waveFormat)
	{
		case WF_PCM_S16LE:
			while(readNum<readLength)
			{
				short temp;
				if(fread_s(buffer,nBlockAlign,1,nBlockAlign,fptr)!=nBlockAlign) break;
				for(int i=0; i<nChannel; i++)
				{
					memcpy(&temp,buffer+i*wBitsPerSample/8,2);
					data[i][readNum]=temp;
				}
				readNum++;
			}
			break;
		default:
			;	//do nothing
	}
	free(buffer);
	return readNum;
}
//Get the data form startSampleIndex till endSampleIndex, endSampleIndex is not included.
//short ** data: The buffer containing data, you shoud Use CreateData() to initialze data before passing it,
//				 and DestoryData() to free the memory. The format is data[CHANNEL_NUM][BUFFER_LENGTH].
//				 Self memory management is not recommand, use GreateData() and DestoryData() instead.
//int size: The maxium number of data element that you  want to put into file.
//double startTime: The time that start form the begining of the wave in second, to begin data retriving.
//double endTime:  The time that start form the begining of the wave in second, to finish data retriving.
//return the number of element put from data.
int WaveFile::GetDataByTime(short ** data, int size, double startTime, double endTime)
{
	if(fptr==NULL || startTime<0|| endTime<0) return 0;
	int readNum=0;
	int readLength;
	int startSampleIndex = (int)(startTime * nSamplesPerSec) * nBlockAlign;
	int endSampleIndex = (int)(endTime * nSamplesPerSec) * nBlockAlign;
	if(endTime<0) readLength=size;
	else
	{
		if(startSampleIndex>endSampleIndex)
		{
			readLength = startSampleIndex;
			startSampleIndex = endSampleIndex;
			endSampleIndex = readLength;
		}
		readLength=endSampleIndex-startSampleIndex;
		if(readLength>size) readLength=size;
	}
	SeekFile(startSampleIndex*nBlockAlign/8,WF_DATA_SET);
	char * buffer = (char *)malloc(nBlockAlign);
	switch(waveFormat)
	{
		case WF_PCM_S16LE:
			while(readNum<readLength)
			{
				short temp;
				if(fread_s(buffer,nBlockAlign,1,nBlockAlign,fptr)!=nBlockAlign) break;
				for(int i=0; i<nChannel; i++)
				{
					memcpy(&temp,buffer+i*wBitsPerSample/8,2);
					data[i][readNum]=temp;
				}
				readNum++;
			}
			break;
		default:
			;	//do nothing
	}
	free(buffer);
	return readNum;
}