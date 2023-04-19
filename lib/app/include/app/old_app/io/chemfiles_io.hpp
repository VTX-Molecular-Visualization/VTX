#ifndef __VTX_IO_CHEMFILES__
#define __VTX_IO_CHEMFILES__

#include "app/core/worker/base_thread.hpp"
#include "app/core/worker/base_worker.hpp"
#include <string>

namespace VTX
{
	namespace Worker
	{
		class BaseThread;
		class BaseWorker;
	} // namespace Worker

	namespace IO
	{
		class ChemfilesIO
		{
		  public:
			ChemfilesIO() : _thread( nullptr ) {}
			ChemfilesIO( const VTX::Core::Worker::BaseThread * const p_thread ) : _thread( p_thread ) {}
			ChemfilesIO( const VTX::Core::Worker::BaseWorker * const p_worker ) : _thread( nullptr ) {}

		  protected:
			void _prepareChemfiles() const;

			void _logError( const std::string & p_log ) const;
			void _logWarning( const std::string & p_log ) const;
			void _logInfo( const std::string & p_log ) const;
			void _logDebug( const std::string & p_log ) const;
			void _logFile( const std::string & p_log ) const;

		  private:
			const VTX::Core::Worker::BaseThread * const _thread;
		};
	} // namespace IO
} // namespace VTX
#endif
