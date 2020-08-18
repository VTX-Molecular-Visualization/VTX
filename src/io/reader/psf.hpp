#ifndef __VTX_READER_PSF__
#define __VTX_READER_PSF__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader_line.hpp"
#include "define.hpp"
#include "model/configuration/molecule.hpp"
#include <set>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			class PSF : public BaseReaderLine<Model::Configuration::Molecule>
			{
			  protected:
				void _readLine( const std::string &, Model::Configuration::Molecule & ) override;

			  private:
				std::string _readSymbol( const std::string & ) const;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
