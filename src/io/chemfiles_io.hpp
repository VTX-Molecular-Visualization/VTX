#ifndef __VTX_IO_CHEMFILES__
#define __VTX_IO_CHEMFILES__

#ifdef _MSC_VER
#pragma once
#endif

#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
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
			ChemfilesIO( Worker::BaseThread * const p_thread ) : _thread( p_thread ) {}
			ChemfilesIO( Worker::BaseWorker * const p_worker ) : _thread( nullptr ) {}

		  protected:
			void _prepareChemfiles() const;

			void _logError( const std::string & p_log ) const;
			void _logWarning( const std::string & p_log ) const;
			void _logInfo( const std::string & p_log ) const;
			void _logDebug( const std::string & p_log ) const;

		  private:
			Worker::BaseThread * const _thread;
		};
	} // namespace IO
} // namespace VTX
#endif
