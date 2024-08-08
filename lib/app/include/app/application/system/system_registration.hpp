#ifndef __VTX_APP_APPLICATION_SYSTEM_SYSTEM_REGISTRATION__
#define __VTX_APP_APPLICATION_SYSTEM_SYSTEM_REGISTRATION__

#include "app/core/system/base_system.hpp"
#include "app/core/system/system_handler.hpp"
#include "app/vtx_app.hpp"
#include <typeinfo>
#include <util/hashing.hpp>

namespace VTX::App::Application::System
{
	// Automating system registration to VTXApp systemHandler at launch when declared as static member of a BaseSystem
	template<typename T>
	class SystemRegistration
	{
	  public:
		SystemRegistration() : SystemRegistration( typeid( T ).hash_code() ) {}
		T & get() const { return APP().getSystemHandler().get<T>( _hash ); }

	  private:
		SystemRegistration( const Hash & p_hash ) : _hash( p_hash )
		{
			APP().getSystemHandler().template store<T>( _hash );
		}
		// SystemRegistration( const size_t & p_hash ) : SystemRegistration( Util::hash( p_hash ) ) {}

		const Hash _hash;
	};

	// VTX systems must inherits from AutoRegistrateSystem<T> in order to registrate them directly in the SystemHandler
	template<typename T>
	class AutoRegistrateSystem : public Core::System::BaseSystem
	{
	  public:
		inline static const System::SystemRegistration<T> SYSTEM = System::SystemRegistration<T>();
	};

} // namespace VTX::App::Application::System

#endif
