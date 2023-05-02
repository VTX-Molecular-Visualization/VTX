#ifndef __VTX_APP_ACTION_REPRESENTABLE__
#define __VTX_APP_ACTION_REPRESENTABLE__

#include "app/action.hpp"
#include "app/application/representation/base_representable.hpp"
#include "app/application/representation/representation_preset.hpp"
#include "app/core/action/base_action.hpp"
#include "app/application/selection/selection.hpp"

namespace VTX::App::Action::Representable
{
	class SetRepresentation : public App::Core::Action::BaseAction
	{
	  public:
		explicit SetRepresentation( App::Application::Representation::BaseRepresentable &		   p_representable,
									App::Application::Representation::RepresentationPreset * const p_representation ) :
			_representable( &p_representable ),
			_representation( p_representation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit SetRepresentation( const App::Application::Selection::SelectionModel * const							 p_selection,
									App::Application::Representation::RepresentationPreset * p_representation ) :
			_selection( p_selection ),
			_representation( p_representation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		void execute();

	  private:
		const App::Application::Selection::SelectionModel * const								_selection	   = nullptr;
		App::Application::Representation::BaseRepresentable * const _representable = nullptr;

		App::Application::Representation::RepresentationPreset * const _representation;
	};
	class RemoveRepresentation : public App::Core::Action::BaseAction
	{
	  public:
		explicit RemoveRepresentation( App::Application::Representation::BaseRepresentable & p_representable ) :
			_representable( &p_representable )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		void execute();

	  private:
		App::Application::Representation::BaseRepresentable * const _representable;
	};
} // namespace VTX::App::Action::Representable
#endif
