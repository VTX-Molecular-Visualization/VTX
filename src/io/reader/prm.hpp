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
				std::vector<uint> solventIds = std::vector<uint>();
				std::vector<uint> ionIds	 = std::vector<uint>();
			};

			class PRM : public BaseReader<PRMFile>
			{
			  public:
				virtual void readFile( const Path &, PRMFile & ) override;
				virtual void readBuffer( const std::string &, PRMFile & ) override
				{
					throw Exception::NotImplementedException();
				};

			  private:
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
