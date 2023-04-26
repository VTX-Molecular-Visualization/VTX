#ifndef __VTX_IO_CHEMFILES__
#define __VTX_IO_CHEMFILES__

#include <string>

namespace VTX
{
	namespace App::Core::Worker
	{
		class BaseThread;
		class BaseWorker;
	} // namespace App::Core::Worker

	namespace IO
	{
		class ChemfilesIO
		{
		  public:
			ChemfilesIO() : _thread( nullptr ) {}
			ChemfilesIO( const VTX::App::Core::Worker::BaseThread * const p_thread ) : _thread( p_thread ) {}
			ChemfilesIO( const VTX::App::Core::Worker::BaseWorker * const p_worker ) : _thread( nullptr ) {}

		  protected:
			void _prepareChemfiles() const;

			void _logError( const std::string & p_log ) const;
			void _logWarning( const std::string & p_log ) const;
			void _logInfo( const std::string & p_log ) const;
			void _logDebug( const std::string & p_log ) const;
			void _logFile( const std::string & p_log ) const;

		  private:
			const VTX::App::Core::Worker::BaseThread * const _thread;
		};
	} // namespace IO
} // namespace VTX
#endif
