//#include "Surface.h"
//#include <d3d11.h>
//#include <stdio.h>
//#include "GraphicsThrowMacros.h"
//
//Surface::~Surface()
//{
//}
//
//Surface::Surface(unsigned int width, unsigned int height, unsigned int pitch) noexcept : height(height), width(width), pitch(pitch)
//{}
//
//Surface::Surface(unsigned int width, unsigned int height) noexcept : width(width), height(height)
//{}
//
//Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<unsigned char[]> pBufferParam) noexcept
//	: width(width), height(height), pBuffer(std::move(pBufferParam))
//{
//}
//
//Surface& Surface::operator=(Surface&& donor) noexcept
//{
//	width = donor.width;
//	height = donor.height;
//	pBuffer = std::move(donor.pBuffer);
//	donor.pBuffer = nullptr;
//	return *this;
//}
//
//Surface::Surface(Surface && source) noexcept : pBuffer(std::move(source.pBuffer)), width(source.width), height(source.height)
//{}
//
//Surface Surface::FromFile(const std::string & filename)
//{
//	FILE* file;
//	int error;
//	int count;
//	TargaHeader targaHeader;
//	unsigned char* targaImage;
//
//	//Open target file
//	error = fopen_s(&file, filename.data(), "rb");
//	if (error != 0)
//	{
//		
//	}
//	//Read in file header
//	count = (unsigned int)fread(&targaHeader, sizeof(TargaHeader), 1, file);
//	if (count != 1)
//	{
//
//	}
//
//	int height = (int)targaHeader.height;
//	int width = (int)targaHeader.width;
//
//	int imageSize = (width * height) * 4;
//	targaImage = new unsigned char[imageSize];
//	if (!targaImage)
//	{
//
//	}
//
//	count = (unsigned int)fread(targaImage, 1, imageSize, file);
//	if (count != imageSize)
//	{
//
//	}
//
//	error = fclose(file);
//
//
//	int index = 0;
//	int k = (width * height * 4) - (width * 4) + (9 * 4);
//
//	//Allocate memory for targa destination data
//	std::unique_ptr<unsigned char[]> tempBuffer = std::make_unique<unsigned char[]>(imageSize);
//
//	for (int j = 0; j < height; j++)
//	{
//		for (int i = 0; i < width; i++)
//		{
//			tempBuffer[index + 0] = targaImage[k + 2];
//			tempBuffer[index + 1] = targaImage[k + 1];
//			tempBuffer[index + 2] = targaImage[k + 0];
//			tempBuffer[index + 3] = targaImage[k + 3];
//
//			k += 4;
//			index += 4;
//		}
//
//		k -= (width * 8);
//	}
//
//	delete[] targaImage;
//	targaImage = 0;
//
//	return Surface(width, height, std::move(tempBuffer));
//}


#define EXCEPTT
#include "Surface.h"
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>
#include <sstream>

#pragma comment( lib,"gdiplus.lib" )

Surface::Surface(unsigned int width, unsigned int height) noexcept
	:
	pBuffer(std::make_unique<Color[]>(width* height)),
	width(width),
	height(height)
{}

Surface& Surface::operator=(Surface&& donor) noexcept
{
	width = donor.width;
	height = donor.height;
	pBuffer = std::move(donor.pBuffer);
	donor.pBuffer = nullptr;
	return *this;
}

Surface::Surface(Surface&& source) noexcept
	:
	pBuffer(std::move(source.pBuffer)),
	width(source.width),
	height(source.height)
{}

Surface::~Surface()
{}

void Surface::Clear(Color fillValue) noexcept
{
	memset(pBuffer.get(), fillValue.dword, width * height * sizeof(Color));
}

void Surface::PutPixel(unsigned int x, unsigned int y, Color c)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	pBuffer[y * width + x] = c;
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);
	return pBuffer[y * width + x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return height;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return pBuffer.get();
}

Surface Surface::FromFile(const std::string& name)
{
	unsigned int width = 0;
	unsigned int height = 0;
	std::unique_ptr<Color[]> pBuffer;

	{
		// convert filenam to wide string (for Gdiplus)
		wchar_t wideName[512];
		mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE);

		Gdiplus::Bitmap bitmap(wideName);
		if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
		{
			std::stringstream ss;
			ss << "Loading image [" << name << "]: failed to load.";
			throw Except(__LINE__, __FILE__, ss.str());
		}

		width = bitmap.GetWidth();
		height = bitmap.GetHeight();
		pBuffer = std::make_unique<Color[]>(width * height);

		for (unsigned int y = 0; y < height; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				Gdiplus::Color c;
				bitmap.GetPixel(x, y, &c);
				pBuffer[y * width + x] = c.GetValue();
			}
		}
	}

	return Surface(width, height, std::move(pBuffer));
}

void Surface::Save(const std::string& filename) const
{
	auto GetEncoderClsid = [&filename](const WCHAR* format, CLSID* pClsid) -> void
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
		{
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; size == 0.";
			throw Except(__LINE__, __FILE__, ss.str());
		}

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == nullptr)
		{
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
			throw Except(__LINE__, __FILE__, ss.str());
		}

		GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return;
			}
		}

		free(pImageCodecInfo);
		std::stringstream ss;
		ss << "Saving surface to [" << filename <<
			"]: failed to get encoder; failed to find matching encoder.";
		throw Except(__LINE__, __FILE__, ss.str());
	};

	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", &bmpID);


	// convert filenam to wide string (for Gdiplus)
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(width, height, width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)pBuffer.get());
	if (bitmap.Save(wideName, &bmpID, nullptr) != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to save.";
		throw Except(__LINE__, __FILE__, ss.str());
	}
}

void Surface::Copy(const Surface& src)
{
	assert(width == src.width);
	assert(height == src.height);
	memcpy(pBuffer.get(), src.pBuffer.get(), width * height * sizeof(Color));
}

Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept
	:
	width(width),
	height(height),
	pBuffer(std::move(pBufferParam))
{}


// surface exception stuff
Surface::Except::Except(int line, const char* file, std::string note) noexcept
	:
	Exception(line, file),
	note(std::move(note))
{}


const char* Surface::Except::what() const noexcept
{
	std::ostringstream oss;
	oss << Exception::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Surface::Except::GetType() const noexcept
{
	return "Graphics Exception";
}

const std::string& Surface::Except::GetNote() const noexcept
{
	return note;
}