#include "app/system/writer.hpp"
#include <io/writer/system.hpp>
#include <latch>

namespace VTX::App::System
{
	struct SelectionWriter::_impl
	{
		_impl( FilePath p_dest ) : _dest( std::move( p_dest ) ) {}

		FilePath   _dest;
		std::latch synchronizer { 1 };
	};

	SelectionWriter::SelectionWriter( FilePath p_dest ) : _ptr( new _impl( std::move( p_dest ) ) ) {}

	uint SelectionWriter::operator()( Util::StopToken p_token, Threading::OptionalThreadReference p_thread )
	{
		if ( p_token.stop_requested() )
			return 0;

		if ( p_thread )
			p_thread.value().get().setProgressText( std::format( "Writting file {} ...", _ptr->_dest.string() ) );

		VTX::IO::Writer::writeFile(
			VTX::IO::Writer::WriteArgs {
				.destination = _ptr->_dest,
				// TODO : selection not supported !!
			}
		);
	}

	void SelectionWriter::wait() noexcept { _ptr->synchronizer.wait(); }

} // namespace VTX::App::System
