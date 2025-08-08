#ifndef __VTX_APP_ACTION_REPRESENTATION__
#define __VTX_APP_ACTION_REPRESENTATION__

#include "app/core/action/base_action.hpp"
#include "app/core/library/library_system.hpp"
#include "app/library/preset/representation.hpp"
#include <renderer/proxy/representation.hpp>

namespace VTX::App::Action::Representation
{
	template<Renderer::Proxy::E_REPRESENTATION_SETTINGS S, typename T>
	class ChangeRepresentation final : public App::Core::Action::BaseAction
	{
	  public:
		ChangeRepresentation( const std::string_view p_name, const T p_value ) : _name( p_name ), _value( p_value ) {}

		void execute() override
		{
			using namespace Renderer::Proxy;

			auto * const library = LIBRARY_SYSTEM().getLibrary<App::Library::Preset::Representation>();
			auto * const preset	 = library->getPreset( _name );

			preset->setValue<S>( _value );
		}

	  private:
		const std::string _name;
		const T			  _value;
	};

} // namespace VTX::App::Action::Representation
#endif
