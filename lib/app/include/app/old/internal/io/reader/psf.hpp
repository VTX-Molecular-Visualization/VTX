#ifndef __VTX_APP_OLD_INTERNAL_IO_READER_PSF__
#define __VTX_APP_OLD_INTERNAL_IO_READER_PSF__

#include "app/old/component/io/_fwd.hpp"
#include "app/old/core/io/reader/base_reader_line.hpp"
#include <set>
#include <string>
#include <util/types.hpp>

namespace VTX::App::Old::Internal::IO::Reader
{
	class PSF : public Core::IO::Reader::BaseReaderLine<App::Old::Component::IO::MoleculeConfiguration>
	{
	  protected:
		void _readLine( const std::string &, App::Old::Component::IO::MoleculeConfiguration & ) override;

	  private:
		std::string _readSymbol( const std::string & ) const;
	};
} // namespace VTX::App::Old::Internal::IO::Reader
#endif
