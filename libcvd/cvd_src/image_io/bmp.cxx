#include <cvd/internal/io/bmp.h>

using namespace std;
#include <iomanip>
#include <iostream>
#include <setjmp.h>
#include <sstream>

namespace CVD
{
namespace BMP
{

	struct BMPHeader
	{
		uint32_t size; //!< header size
		uint16_t reserved[2]; //!< reserved
		uint32_t dataOffset; //!< offset to beginning of data
		uint32_t infoSize; //!< size of info
		uint32_t width; //!< width of image
		uint32_t height; //!< height of image
		uint16_t planes; //!< planes
		uint16_t bpp; //!< colour depth
		uint32_t compression; //!< compression type
		uint32_t dataSize; //!< size of data
		uint32_t xPelsPerMeter; //!< x resolution in pixels/meter
		uint32_t yPelsPerMeter; //!< y resolution in pixels/meter
		uint32_t colors; //!< number of specified colors in the map
		uint32_t importantColors; //!< number of important colors
	};

	void write_u4(ostream& o, unsigned long l)
	{
		o << (unsigned char)(0xff & (l))
		  << (unsigned char)(0xff & (l >> 8))
		  << (unsigned char)(0xff & (l >> 16))
		  << (unsigned char)(0xff & (l >> 24));
	}

	void write_u1(ostream& o, unsigned long l)
	{
		o << (unsigned char)(0xff & (l));
	}

	void write_u2(ostream& o, unsigned long l)
	{
		o << (unsigned char)(0xff & (l))
		  << (unsigned char)(0xff & (l >> 8));
	}

	inline uint32_t read_u4(istream& i)
	{
		//Data is little endian:
		//unsigned long r =  i.get() | (i.get() << 8) | (i.get() << 16) | (i.get() << 24 );
		int a = i.get();
		int b = i.get();
		int c = i.get();
		int d = i.get();
		uint32_t r = a | (b << 8) | (c << 16) | (d << 24);
		if(i.eof())
			throw(Exceptions::Image_IO::MalformedImage("EOF in header."));

		return r;
	}

	inline uint16_t read_u2(istream& i)
	{
		//Data is little endian:
		//unsigned long r =  i.get() | (i.get() << 8);
		int a = i.get();
		int b = i.get();
		if(i.eof())
			throw(Exceptions::Image_IO::MalformedImage("EOF in header."));

		return static_cast<uint16_t>(a | (b << 8));
	}

	BMPHeader read_header(istream& i)
	{
		//Read the header
		BMPHeader h;
		h.size = read_u4(i);
		h.reserved[0] = read_u2(i);
		h.reserved[1] = read_u2(i);
		h.dataOffset = read_u4(i);

		//Read the info block
		h.infoSize = read_u4(i);

		if(h.infoSize != 40)
		{

			string type;

			switch(h.infoSize)
			{
				case 40:
					type = "Windows 3.x style bitmap unsupported.";
					break;
				case 12:
					type = "OS/2 1.x style bitmap unsupported.";
					break;
				case 64:
					type = "OS/2 2.x style bitmap unsupported.";
					break;
				case 108:
					type = "Windows V4 (95/NT) style bitmap unsupported.";
					break;
				case 124:
					type = "Windows V5 (98/2000) style bitmap unsupported.";
					break;
				default:
				{
					ostringstream ts;
					ts << "unknown bitmap type (info size = " << h.infoSize << ")";
					type = ts.str();
				}
			}

			throw(Exceptions::Image_IO::UnsupportedImageSubType("Windows BMP/DIB", type));
		}
		h.width = read_u4(i);
		h.height = read_u4(i);
		h.planes = read_u2(i);
		h.bpp = read_u2(i);
		h.compression = read_u4(i);
		h.dataSize = read_u4(i);
		h.xPelsPerMeter = read_u4(i);
		h.yPelsPerMeter = read_u4(i);
		h.colors = read_u4(i);
		h.importantColors = read_u4(i);

		return h;
	}

	void writeBMPHeader(unsigned int width, unsigned int height, unsigned int channels, ostream& out)
	{
		unsigned int rowSize = channels * width;
		if(rowSize % 4)
			rowSize += 4 - (rowSize % 4);

		unsigned long dataSize = rowSize * height;
		unsigned long offset = 14 + 40 + (channels == 1 ? 1024 : 0);

		//Output header
		/*   0  */ out << "BM";
		/*   2  */ write_u4(out, offset + dataSize); //File size
		/*   6  */ write_u2(out, 0); //Reserved
		/*   8  */ write_u2(out, 0); //Reserved
		/*  10  */ write_u4(out, offset); //Data offset

		//Output info
		/*  14  */ write_u4(out, 40); //3.x style header size
		/*  18  */ write_u4(out, width);
		/*  22  */ write_u4(out, height);
		/*  26  */ write_u2(out, 1); //Planes (always 1);
		/*  28  */ write_u2(out, channels * 8); //Bpp
		/*  30  */ write_u4(out, 0); //Compression method
		/*  34  */ write_u4(out, dataSize);
		/*  38  */ write_u4(out, 0); //Pixels per meter (x)
		/*  42  */ write_u4(out, 0); //Pixels per meter (y)
		/*  46  */ write_u4(out, 0); //Number of colours (0 == 2^N)
		/*  50  */ write_u4(out, 0); //Number of important colora

		//Write the palette
		if(channels == 1)
		{
			for(unsigned int i = 0; i < 256; i++)
			{
				write_u1(out, i); //Blue
				write_u1(out, i); //Green
				write_u1(out, i); //Red
				write_u1(out, 0); //Reserved
			}
		}
	}

	void readBMPHeader(unsigned int& width, unsigned int& height, unsigned int& channels, unsigned int& compression, istream& in)
	{
		char tag[3] = { 0 };
		in.read(tag, 2);
		if(strcmp(tag, "BM") != 0)
			throw CVD::Exceptions::Image_IO::MalformedImage("BMP files must start with 'BM'");
		struct BMPHeader header = read_header(in);

		if(header.bpp == 8)
			channels = 1;
		else if(header.bpp == 24)
			channels = 3;
		else
			throw(Exceptions::Image_IO::UnsupportedImageSubType("Windows BMP/DIB", "image is not 8 or 24 bits per pixel."));

		compression = header.compression;
		if(compression != 0)
			throw(Exceptions::Image_IO::UnsupportedImageSubType("Windows BMP/DIB", "compressed images not supported."));

		width = header.width;
		height = header.height;
	}
}
}
