#ifndef __VTX_READER_PSF__
#define __VTX_READER_PSF__

#include "base_reader_line.hpp"
#include <util/types.hpp>
#include <set>

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
