#include "app/internal/io/chemfiles_io.hpp"
#include "app/core/worker/base_thread.hpp"
#include "app/core/worker/base_worker.hpp"
#include <chemfiles.hpp>
#include <util/logger.hpp>

namespace VTX::App::Internal::IO
{
	void ChemfilesIO::_prepareChemfiles() const
	{
#ifdef VTX_PRODUCTION
		chemfiles::warning_callback_t callback = [ & ]( const std::string & p_log ) { _logFile( p_log ); };
#else
		chemfiles::warning_callback_t callback = [ & ]( const std::string & p_log ) { _logWarning( p_log ); };
#endif
		chemfiles::set_warning_callback( callback );
	}

	// TODO reimplement this without Qt
	void ChemfilesIO::_logError( const std::string & p_log ) const
	{
		// if ( _thread != nullptr )
		//	emit _thread->logError( p_log );
		// else
		//	VTX_ERROR( p_log );
	}

	// TODO reimplement this without Qt
	void ChemfilesIO::_logWarning( const std::string & p_log ) const
	{
		// if ( _thread != nullptr )
		//	emit _thread->logWarning( p_log );
		// else
		//	VTX_WARNING( p_log );
	}

	// TODO reimplement this without Qt
	void ChemfilesIO::_logInfo( const std::string & p_log ) const
	{
		// if ( _thread != nullptr )
		//	emit _thread->logInfo( p_log );
		// else
		//	VTX_INFO( p_log );
	}

	// TODO reimplement this without Qt
	void ChemfilesIO::_logDebug( const std::string & p_log ) const
	{
		// if ( _thread != nullptr )
		//	emit _thread->logDebug( p_log );
		// else
		//	VTX_DEBUG( p_log );
	}

	// TODO reimplement this without Qt
	void ChemfilesIO::_logFile( const std::string & p_log ) const
	{
		// if ( _thread != nullptr )
		//	emit _thread->logFile( p_log );
		//// else
		//// VTX_LOG_FILE( p_log );
	}
} // namespace VTX::App::Internal::IO
