#ifndef __VTX_APP_OLD_CORE_IO_WRITER_SERIALIZED_OBJECT__
#define __VTX_APP_OLD_CORE_IO_WRITER_SERIALIZED_OBJECT__

#include "app/old/application/define.hpp"
#include "app/old/core/io/writer/base_writer.hpp"
#include "app/old/core/worker/base_thread.hpp"
#include "app/old/core/worker/base_worker.hpp"
#include "app/old/internal/io/serializer.hpp"
// #include <QFile>
// #include <QTextStream>
#include <nlohmann/json.hpp>
#include <util/types.hpp>

namespace VTX::App::Old::Core::IO::Writer
{
	template<typename T>
	class SerializedObject : public BaseWriter<T>
	{
	  public:
		SerializedObject() : _thread( nullptr ) {}
		SerializedObject( const VTX::App::Old::Core::Worker::BaseThread * const p_thread ) : _thread( p_thread ) {}
		SerializedObject( const VTX::App::Old::Core::Worker::BaseWorker * const p_worker ) : _thread( nullptr ) {}

		void writeFile( const FilePath & p_path, const T & p_data ) override
		{
			Internal::IO::Serializer serializer = Internal::IO::Serializer( _thread );

			nlohmann::json json = { { "_VERSION",
									  { { "MAJOR", Application::VTX_VERSION_MAJOR },
										{ "MINOR", Application::VTX_VERSION_MINOR },
										{ "REVISION", Application::VTX_VERSION_REVISION } } },
									{ "DATA", serializer.serialize( p_data ) } };

			_writeJsonFile( p_path, json );
		}

	  protected:
		void _logError( const std::string & p_log ) const
		{
			// if ( _thread != nullptr )
			//	emit _thread->logError( p_log );
			// else
			VTX_ERROR( "{}", p_log );
		}
		void _logWarning( const std::string & p_log ) const
		{
			// if ( _thread != nullptr )
			//	emit _thread->logWarning( p_log );
			// else
			VTX_WARNING( "{}", p_log );
		}
		void _logInfo( const std::string & p_log ) const
		{
			// if ( _thread != nullptr )
			//	emit _thread->logInfo( p_log );
			// else
			VTX_INFO( "{}", p_log );
		}
		void _logDebug( const std::string & p_log ) const
		{
			// if ( _thread != nullptr )
			//	emit _thread->logDebug( p_log );
			// else
			VTX_DEBUG( "{}", p_log );
		}

		// TODO reimplement this without Qt
		void _writeJsonFile( const FilePath & p_path, const nlohmann::json & p_json )
		{
			// QFile file( QString::fromStdString( p_path.string() ) );
			// if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) == false )
			//{
			//	throw IOException( "Can not write file: " + p_path.string() );
			// }

			// QTextStream out( &file );
			// out << QString::fromStdString( p_json.dump( 4 ) ) << "\n";
			// file.close();
		}

	  private:
		const VTX::App::Old::Core::Worker::BaseThread * const _thread;
	};
} // namespace VTX::App::Old::Core::IO::Writer
#endif