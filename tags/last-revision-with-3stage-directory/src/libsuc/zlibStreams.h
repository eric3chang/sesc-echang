#ifndef ZLIBSTREAMS_H
#define ZLIBSTREAMS_H

#include <zlib.h>
#include <zconf.h>
#include <fstream>
#include <stdint.h>

class ZlibOut
{
	static const size_t chunkSize = 1048576;
	uint8_t* inBuffer;
	size_t currentMarker;

	std::ofstream outStream;

	bool isOpen;

	void Flush();

public:
	ZlibOut(const char* fname);
	~ZlibOut();

	void Write(void* buffer, size_t size);
	void Close();
};

class ZlibIn
{
	static const int chunkSize = 1048576;
	uint8_t* outBuffer;
	size_t currentMarker;
	size_t currentQuantity;

	std::ifstream inStream;

	bool isOpen;

	void Refill();

public:
	ZlibIn(const char* fname);
	~ZlibIn();

	void Read(void* buffer, size_t size);
	void Close();
};

#endif
