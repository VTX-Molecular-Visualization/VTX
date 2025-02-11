#ifndef __VTX_APP_ACTION_REPRESENTATION__
#define __VTX_APP_ACTION_REPRESENTATION__

#include "app/core/action/base_action.hpp"
#include <app/application/scene.hpp>
#include <app/component/representation/representation.hpp>

namespace VTX::App::Action::Representation
{
	template<Renderer::Proxy::E_REPRESENTATION_SETTINGS S, typename T>
	class ChangeRepresentation final : public App::Core::Action::BaseAction
	{
	  public:
		ChangeRepresentation( const T p_value ) : _value( p_value ) {}

		void execute() override
		{
			const auto & scene	   = App::SCENE();
			auto &		 component = App::ECS_REGISTRY().getComponent<App::Component::Representation::Representation>(
				  App::ECS_REGISTRY().getEntity( scene )
			  );

			component.set<S>( _value );
		}

	  private:
		const T _value;
	};

} // namespace VTX::App::Action::Representation
#endif
