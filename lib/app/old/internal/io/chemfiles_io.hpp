#ifndef __VTX_APP_OLD_INTERNAL_IO_CHEMFILES_IO__
#define __VTX_APP_OLD_INTERNAL_IO_CHEMFILES_IO__

#include "app/old/core/worker/base_thread.hpp"
#include "app/old/core/worker/base_worker.hpp"
#include <string>

namespace VTX::App::Old::Internal::IO
{
	class ChemfilesIO
	{
	  public:
		ChemfilesIO() : _thread( nullptr ) {}
		ChemfilesIO( const Core::Worker::BaseThread * const p_thread ) : _thread( p_thread ) {}
		ChemfilesIO( const Core::Worker::BaseWorker * const p_worker ) : _thread( nullptr ) {}

	  protected:
		void _prepareChemfiles() const;

		void _logError( const std::string & p_log ) const;
		void _logWarning( const std::string & p_log ) const;
		void _logInfo( const std::string & p_log ) const;
		void _logDebug( const std::string & p_log ) const;
		void _logFile( const std::string & p_log ) const;

	  private:
		const Core::Worker::BaseThread * const _thread;
	};
} // namespace VTX::App::Old::Internal::IO
#endif
