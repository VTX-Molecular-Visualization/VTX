#ifndef __VTX_APP_ACTION_REPRESENTATION__
#define __VTX_APP_ACTION_REPRESENTATION__

#include "app/action/library.hpp"
#include "app/application/scene.hpp"
#include "app/component/representation/representation.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include "app/library/preset/representation.hpp"

namespace VTX::App::Action::Representation
{
	class SetCurrent final : public Library::BaseActionPreset<App::Library::Preset::Representation>
	{
	  public:
		SetCurrent( App::Library::Preset::Representation * const p_preset ) :
			Library::BaseActionPreset<App::Library::Preset::Representation>( p_preset )
		{
		}
		SetCurrent( const std::string_view p_preset ) :
			Library::BaseActionPreset<App::Library::Preset::Representation>( p_preset )
		{
		}
		void execute() override
		{
			ECS_REGISTRY().removeComponent<Component::Representation::Representation>(
				ECS_REGISTRY().getEntity( SCENE() )
			);
			auto & comp = ECS_REGISTRY().addComponent<Component::Representation::Representation>(
				ECS_REGISTRY().getEntity( SCENE() ), *_preset
			);
			comp.setupProxy();
		}
	};

	template<VTX::Core::Struct::E_REPRESENTATION_VALUES S, typename T>
	class Change final : public Library::BaseActionPreset<App::Library::Preset::Representation>
	{
	  public:
		Change( App::Library::Preset::Representation * const p_preset, const T p_value ) :
			Library::BaseActionPreset<App::Library::Preset::Representation>( p_preset ), _value( p_value )
		{
		}
		Change( const std::string_view p_preset, const T p_value ) :
			Library::BaseActionPreset<App::Library::Preset::Representation>( p_preset ), _value( p_value )
		{
		}

		void execute() override { _preset->setValue<S>( _value ); }

	  private:
		const T _value;
	};

} // namespace VTX::App::Action::Representation
#endif
