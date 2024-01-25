#ifndef __VTX_APP_APPLICATION_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM__

#include <concepts>
#include <memory>
#include <util/variant.hpp>

namespace VTX::App::Application
{
	// template<typename T>
	// concept SystemConcept = std::is_class<T>::value;

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

		bool exists( const std::string & p_key ) const { return _systems.find( p_key ) != _systems.end(); }

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
