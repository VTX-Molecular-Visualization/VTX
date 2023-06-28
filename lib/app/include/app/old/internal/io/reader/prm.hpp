#ifndef __VTX_APP_OLD_INTERNAL_IO_READER_PRM__
#define __VTX_APP_OLD_INTERNAL_IO_READER_PRM__

#include "app/old/component/io/_fwd.hpp"
#include "app/old/core/io/reader/base_reader_line.hpp"
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Internal::IO::Reader
{
	class PRM : public Core::IO::Reader::BaseReaderLine<App::Old::Component::IO::MoleculeConfiguration>
	{
	  protected:
		void _readLine( const std::string &, App::Old::Component::IO::MoleculeConfiguration & ) override;

	  private:
		uint _readId( const std::string & ) const;
	};
} // namespace VTX::App::Old::Internal::IO::Reader
#endif
