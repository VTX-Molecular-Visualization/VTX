#include "io/writer/chemfiles.hpp"

namespace VTX::IO::Writer
{
	void Chemfiles::setWriteDestination( FilePath p_ ) noexcept { _dest = std::move( p_ ); }

	void Chemfiles::setWriteFormat( Util::Chemfiles::E_STRUCT_FILE_FORMAT p_ ) noexcept
	{
		_fileFormat = std::move( p_ );
	}


} // namespace VTX::IO::Writer
