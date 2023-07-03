#ifndef __VTX_APP_OLD_ACTION_REPRESENTABLE__
#define __VTX_APP_OLD_ACTION_REPRESENTABLE__

#include "app/old/action.hpp"
#include "app/old/application/representation/base_representable.hpp"
#include "app/old/application/representation/representation_preset.hpp"
#include "app/old/core/action/base_action.hpp"
#include "app/old/application/selection/selection.hpp"

namespace VTX::App::Old::Action::Representable
{
	class SetRepresentation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit SetRepresentation( App::Old::Application::Representation::BaseRepresentable &		   p_representable,
									App::Old::Application::Representation::RepresentationPreset * const p_representation ) :
			_representable( &p_representable ),
			_representation( p_representation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit SetRepresentation( const App::Old::Application::Selection::SelectionModel * const							 p_selection,
									App::Old::Application::Representation::RepresentationPreset * p_representation ) :
			_selection( p_selection ),
			_representation( p_representation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		void execute();

	  private:
		const App::Old::Application::Selection::SelectionModel * const								_selection	   = nullptr;
		App::Old::Application::Representation::BaseRepresentable * const _representable = nullptr;

		App::Old::Application::Representation::RepresentationPreset * const _representation;
	};
	class RemoveRepresentation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit RemoveRepresentation( App::Old::Application::Representation::BaseRepresentable & p_representable ) :
			_representable( &p_representable )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		void execute();

	  private:
		App::Old::Application::Representation::BaseRepresentable * const _representable;
	};
} // namespace VTX::App::Old::Action::Representable
#endif
