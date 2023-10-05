#ifndef __VTX_APP_APPLICATION_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM__

#include <memory>
#include <util/variant.hpp>

namespace VTX::App::Application
{
	class System
	{
	  public:
		System();
		~System();

		template<typename T>
		void referenceSystem( const std::string & p_key, T * const p_system )
		{
			assert( _systems.find( p_key ) == _systems.end() );
			_systems[ p_key ] = p_system;
		}

		template<typename T>
		T & getSystem( const std::string & p_key ) const
		{
			return *( _systems.at( p_key ).getPtr<T>() );
		}

	  private:
		Util::VariantMap _systems = Util::VariantMap();
	};
} // namespace VTX::App::Application

#endif
