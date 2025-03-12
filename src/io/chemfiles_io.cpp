#include "chemfiles_io.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "worker/base_thread.hpp"
#include "worker/base_worker.hpp"
#include <chemfiles.hpp>

namespace VTX::IO
{
	void ChemfilesIO::_prepareChemfiles() const
	{
#ifdef VTX_PRODUCTION
		chemfiles::warning_callback_t callback = [ & ]( const std::string & p_log ) { _logFile( p_log ); };
#else
		chemfiles::warning_callback_t callback = [ & ]( const std::string & p_log ) { _logWarning( p_log ); };
#endif
		chemfiles::set_warning_callback( [ & ]( const std::string & ) {} );
		// chemfiles::set_warning_callback( callback ); // Nuke perfs in debug. temporary commented
	}

	void ChemfilesIO::_logError( const std::string & p_log ) const
	{
		if ( _thread != nullptr )
			emit _thread->logError( p_log );
		else
			VTX_ERROR( p_log );
	}

	void ChemfilesIO::_logWarning( const std::string & p_log ) const
	{
		if ( _thread != nullptr )
			emit _thread->logWarning( p_log );
		else
			VTX_WARNING( p_log );
	}
	void ChemfilesIO::_logInfo( const std::string & p_log ) const
	{
		if ( _thread != nullptr )
			emit _thread->logInfo( p_log );
		else
			VTX_INFO( p_log );
	}
	void ChemfilesIO::_logDebug( const std::string & p_log ) const
	{
		if ( _thread != nullptr )
			emit _thread->logDebug( p_log );
		else
			VTX_DEBUG( p_log );
	}
	void ChemfilesIO::_logFile( const std::string & p_log ) const
	{
		if ( _thread != nullptr )
			emit _thread->logFile( p_log );
		else
			VTX_LOG_FILE( p_log );
	}
	bool ChemfilesIO::_isThreadInterrupted() const
	{
		if ( _thread != nullptr )
			return _thread->isInterruptionRequested();
		else
			return false;
	}

} // namespace VTX::IO
