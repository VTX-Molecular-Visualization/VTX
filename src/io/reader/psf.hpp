#ifndef __VTX_READER_PSF__
#define __VTX_READER_PSF__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#include "define.hpp"
#include <set>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			struct PSFFile
			{
				std::set<std::string> solventResidueSymbols = std::set<std::string>();
				std::set<std::string> ionResidueSymbols		= std::set<std::string>();
			};

			class PSF : public BaseReader<PSFFile>
			{
			  public:
				virtual void readFile( const Path &, PSFFile & ) override;
				virtual void readBuffer( const std::string &, const std::string &, PSFFile & ) override
				{
					throw Exception::NotImplementedException();
				};

			  private:
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
