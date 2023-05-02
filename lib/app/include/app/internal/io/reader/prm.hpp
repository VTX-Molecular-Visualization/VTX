#ifndef __VTX_APP_INTERNAL_IO_READER_PRM__
#define __VTX_APP_INTERNAL_IO_READER_PRM__

#include "app/component/io/_fwd.hpp"
#include "app/core/io/reader/base_reader_line.hpp"
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Internal::IO::Reader
{
	class PRM : public Core::IO::Reader::BaseReaderLine<App::Component::IO::MoleculeConfiguration>
	{
	  protected:
		void _readLine( const std::string &, App::Component::IO::MoleculeConfiguration & ) override;

	  private:
		uint _readId( const std::string & ) const;
	};
} // namespace VTX::App::Internal::IO::Reader
#endif
