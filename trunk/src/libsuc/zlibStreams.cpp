#include "zlibStreams.h"
#include <cstdlib>
#include <iostream>
#include <zlib.h>

void ZlibOut::Flush()
{
	uLongf length = chunkSize * 10;
	uint8_t* outBuffer = new uint8_t[chunkSize * 10];

	int result = compress2((Bytef*)outBuffer,&length,(Bytef*) inBuffer, currentMarker,9);
	if(result != Z_OK)
	{
		if(result == Z_MEM_ERROR)
		{
			std::cout << "chunkSize" << chunkSize << std::endl;
			std::cout << "compression error : mem error" << std::endl;
			inBuffer = NULL;
		}
		if(result == Z_STREAM_ERROR)
		{
			std::cout << "chunkSize" << chunkSize << std::endl;
			std::cout << "compression error : stream error" << std::endl;
			inBuffer = NULL;
		}
		if(result == Z_BUF_ERROR)
		{
			std::cout << "chunkSize" << chunkSize << std::endl;
			std::cout << "compression error : buf error" << std::endl;
			inBuffer = NULL;
		}
	}
	uint32_t augLength = length;
	outStream.write((char*)&augLength,sizeof(augLength));
	outStream.write((char*)outBuffer,augLength);
	currentMarker = 0;

	delete [] outBuffer;
}
ZlibOut::ZlibOut(const char* fname) : outStream(fname,std::ios::out | std::ios::binary)
{
	isOpen = true;
	currentMarker = 0;
	inBuffer = new uint8_t[chunkSize];
}
ZlibOut::~ZlibOut()
{
	Close();
}
void ZlibOut::Write(void* buffer, size_t size)
{
	if(!isOpen)
	{
		return;
	}
	uint8_t* b = (uint8_t*)buffer;
	for(size_t i = 0; i < size; i++)
	{
		inBuffer[currentMarker] = b[i];
		currentMarker++;
		if(currentMarker == chunkSize)
		{
			Flush();
		}
	}
}
void ZlibOut::Close()
{
	if(!isOpen)
	{
		return;
	}
	Flush();
	outStream.close();
	delete [] inBuffer;
	isOpen = false;
}
void ZlibIn::Refill()
{
	uint32_t augSize;
	inStream.read((char*)&augSize,sizeof(augSize));
	uint8_t* inBuffer = new uint8_t[augSize];
	inStream.read((char*)inBuffer,augSize);
	if(inStream.bad())
	{
		inBuffer = NULL;
	}
	uLongf bufSize = augSize;
	uLongf outBuffSize = chunkSize;
	int result = uncompress(outBuffer,&outBuffSize,inBuffer,bufSize);
	if(Z_OK != result)
	{
		if(result == Z_DATA_ERROR)
		{
			outBuffer = 0;
		}
	}
	delete [] inBuffer;
	currentQuantity = outBuffSize;
	currentMarker = 0;
}
ZlibIn::ZlibIn(const char* fname) : inStream(fname,std::ios::in | std::ios::binary)
{
	isOpen = true;
	outBuffer = NULL;
	outBuffer = new uint8_t[chunkSize * 2];
	Refill();
}
ZlibIn::~ZlibIn()
{
	Close();
}
void ZlibIn::Read(void* buffer, size_t size)
{
	if(!isOpen)
	{
		return;
	}
	uint8_t* b = (uint8_t*)buffer;
	for(size_t i = 0; i < size; i++)
	{
		if(currentMarker == currentQuantity)
		{
			Refill();
		}
		b[i] = outBuffer[currentMarker];
		currentMarker++;
	}
}
void ZlibIn::Close()
{
	if(!isOpen)
	{
		return;
	}
	inStream.close();
	delete [] outBuffer;
	isOpen = false;
}
