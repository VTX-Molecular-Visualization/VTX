#ifndef __VTX_READER_PRM__
#define __VTX_READER_PRM__

#include "app/component/io/_fwd.hpp"
#include "base_reader_line.hpp"
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::IO::Reader
{
	class PRM : public BaseReaderLine<App::Component::IO::MoleculeConfiguration>
	{
	  protected:
		void _readLine( const std::string &, App::Component::IO::MoleculeConfiguration & ) override;

	  private:
		uint _readId( const std::string & ) const;
	};
} // namespace VTX::IO::Reader
#endif
