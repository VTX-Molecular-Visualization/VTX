#ifndef __VTX_READER_PSF__
#define __VTX_READER_PSF__

#include "app/component/io/_fwd.hpp"
#include "base_reader_line.hpp"
#include <set>
#include <string>
#include <util/types.hpp>

namespace VTX::IO::Reader
{
	class PSF : public BaseReaderLine<App::Component::IO::MoleculeConfiguration>
	{
	  protected:
		void _readLine( const std::string &, App::Component::IO::MoleculeConfiguration & ) override;

	  private:
		std::string _readSymbol( const std::string & ) const;
	};
} // namespace VTX::IO::Reader
#endif
