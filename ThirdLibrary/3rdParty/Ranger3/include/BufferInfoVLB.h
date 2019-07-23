#pragma once
#pragma warning(disable:4251)


#include "GenIRanger.h"
#include "SampleUtils.h"
#include <vector>


// From GenICam Pixel Format Naming Convention we get the
// pixel format values, imported from "pfnch.h" available emva.org
#define PFNC_Mono8 0x01080001 // Monochrome 8-bit
#define PFNC_Mono12p 0x010C0047 // Monochrome 12-bit packed
#define PFNC_Mono16 0x01100007 // Monochrome 16-bit
#define PFNC_Coord3D_C12p 0x010C00DA // 3D coordinate C 12-bit packed
#define PFNC_Coord3D_C16 0x011000B8 // 3D coordinate C 16-bit

// ----------------------------------------------------------------------------
// Class declarations

class BufferInfo
{
public:
	BufferInfo(GenTLApi* tl,
		GenTL::DS_HANDLE dataStreamHandle,
		GenTL::BUFFER_HANDLE bufferHandle);
	void log(std::ostream& logFile) const;
public:
	bool8_t mBufferIncomplete;
	size_t mBufferFilledSize;

	uint64_t mBufferFrameID;
	uint8_t* mDataPointer;
	uint64_t mDataFormat;
	size_t mBufferDataSize;
	size_t mDataWidth; // error
	size_t mDataHeight; // error

};

class BufferPartInfo
{
public:
	BufferPartInfo(GenTLApi* tl,
		GenTL::DS_HANDLE dataStreamHandle,
		GenTL::BUFFER_HANDLE bufferHandle,
		uint32_t partNumber);
	void log(std::ostream& logFile) const;

public:
	uint32_t mPartNumber;		
	uint8_t* mPartDataPointer;
	size_t mPartDataType;		
	uint64_t mPartDataFormat;
	uint64_t mRegionID;		  
	uint64_t mPurposeID;		
	size_t mPartDataSize;		
	size_t mPartDataWidth;	  
	size_t mPartDataHeight;	  

};




