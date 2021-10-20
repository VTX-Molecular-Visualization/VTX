#ifndef __VTX_WRITER_SERIALIZED_OBJECT__
#define __VTX_WRITER_SERIALIZED_OBJECT__

#include "base_writer.hpp"
#include "define.hpp"
#include "io/serializer.hpp"
#include "worker/base_thread.hpp"
#include "worker/base_worker.hpp"
#include <nlohmann/json.hpp>
#include <QTextStream>

namespace VTX::IO::Writer
{
	template<typename T>
	class SerializedObject : public BaseWriter<T>
	{
	  public:
		SerializedObject() : _thread( nullptr ) {}
		SerializedObject( const Worker::BaseThread * const p_thread ) : _thread( p_thread ) {}
		SerializedObject( const Worker::BaseWorker * const p_worker ) : _thread( nullptr ) {}

		void writeFile( const IO::FilePath & p_path, const T & p_data ) override
		{
			IO::Serializer serializer = IO::Serializer( _thread );

			nlohmann::json json = { { "_VERSION",
									  { { "MAJOR", VTX_VERSION_MAJOR },
										{ "MINOR", VTX_VERSION_MINOR },
										{ "REVISION", VTX_VERSION_REVISION } } },
									{ "DATA", serializer.serialize( p_data ) } };

			QFile file( p_path.qpath() );
			if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
			{
				throw Exception::IOException( "Can not write file: " + p_path.path() );
			}

			QTextStream out( &file );
			out << QString::fromStdString( json.dump( 4 ) ) << "\n";
			file.close();
		}

	  protected:
		void _logError( const std::string & p_log ) const
		{
			if ( _thread != nullptr )
				emit _thread->logError( p_log );
			else
				VTX_ERROR( p_log );
		}
		void _logWarning( const std::string & p_log ) const
		{
			if ( _thread != nullptr )
				emit _thread->logWarning( p_log );
			else
				VTX_WARNING( p_log );
		}
		void _logInfo( const std::string & p_log ) const
		{
			if ( _thread != nullptr )
				emit _thread->logInfo( p_log );
			else
				VTX_INFO( p_log );
		}
		void _logDebug( const std::string & p_log ) const
		{
			if ( _thread != nullptr )
				emit _thread->logDebug( p_log );
			else
				VTX_DEBUG( p_log );
		}

	  private:
		const Worker::BaseThread * const _thread;
	};
} // namespace VTX::IO::Writer
#endif
