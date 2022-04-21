#ifndef PNM_JPEG_H
#define PNM_JPEG_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <cvd/byte.h>
#include <cvd/image.h>
#include <cvd/internal/convert_pixel_types.h>
#include <cvd/internal/load_and_save.h>

namespace CVD
{
namespace JPEG
{

	using CVD::Internal::Head;
	using CVD::Internal::TypeList;

	class ReadPimpl;
	class Reader
	{
		public:
		Reader(std::istream&);
		~Reader();

		ImageRef size();
		bool top_row_first();

		void get_raw_pixel_line(unsigned char*);
		void get_raw_pixel_line(Rgb<unsigned char>*);

		std::string datatype();
		std::string name();

		typedef TypeList<byte,
		    TypeList<Rgb<byte>,
		        Head>>
		    Types;

		static bool first_byte_matches(const int b)
		{
			return b == 0xff;
		}

		private:
		std::unique_ptr<ReadPimpl> t;
	};

	class WritePimpl;

	class writer
	{
		public:
		writer(std::ostream&, ImageRef size, const std::string& type, const std::map<std::string, Parameter<>>& p);
		~writer();

		void write_raw_pixel_line(const byte*);
		void write_raw_pixel_line(const Rgb<byte>*);

		template <class Incoming>
		struct Outgoing
		{
			typedef byte type;
		};

		static const int top_row_first = 1;

		protected:
		std::unique_ptr<WritePimpl> t;
	};

	template <class C>
	struct writer::Outgoing<Rgb<C>>
	{
		typedef Rgb<byte> type;
	};

	template <class C>
	struct writer::Outgoing<Rgba<C>>
	{
		typedef Rgb<byte> type;
	};

	template <>
	struct writer::Outgoing<Rgb8>
	{
		typedef Rgb<byte> type;
	};

}
}
#endif
