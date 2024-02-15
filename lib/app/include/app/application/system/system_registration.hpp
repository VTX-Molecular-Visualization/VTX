#ifndef __VTX_APP_APPLICATION_SYSTEM_SYSTEM_REGISTRATION__
#define __VTX_APP_APPLICATION_SYSTEM_SYSTEM_REGISTRATION__

#include "app/core/system/system_handler.hpp"
#include "app/vtx_app.hpp"
#include <typeinfo>
#include <util/hashing.hpp>

namespace VTX::App::Application::System
{
	template<typename T>
	class SystemRegistration
	{
	  public:
		SystemRegistration() : SystemRegistration( typeid( T ).hash_code() ) {}
		T & get() const { return VTXApp::get().getSystemHandler().get<T>( _hash ); }

	  private:
		SystemRegistration( const Util::Hashing::Hash & p_hash ) : _hash( p_hash )
		{
			VTXApp::get().getSystemHandler().template store<T>( _hash );
		}
		// SystemRegistration( const size_t & p_hash ) : SystemRegistration( Util::Hashing::hash( p_hash ) ) {}

		const Util::Hashing::Hash _hash;
	};
} // namespace VTX::App::Application::System

#endif
