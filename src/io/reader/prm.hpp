#ifndef __VTX_READER_PRM__
#define __VTX_READER_PRM__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader_line.hpp"
#include "define.hpp"
#include <vector>

namespace VTX
{
	namespace Model::Configuration
	{
		struct Molecule;
	}
	namespace IO
	{
		namespace Reader
		{
			class PRM : public BaseReaderLine<Model::Configuration::Molecule>
			{
			  protected:
				void _readLine( const std::string &, Model::Configuration::Molecule & ) override;

			  private:
				uint _readId( const std::string & ) const;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
