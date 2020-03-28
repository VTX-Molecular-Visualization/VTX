#ifndef __VTX_READER_PRM__
#define __VTX_READER_PRM__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#include "define.hpp"
#include <vector>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			struct PRMFile
			{
				std::vector<uint> solvantIds = std::vector<uint>();
			};

			class PRM : public BaseReader<PRMFile>
			{
			  public:
				virtual bool readFile( const Path &, PRMFile & ) override;
				virtual bool readBuffer( const std::string &, PRMFile & ) override;

			  private:
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
