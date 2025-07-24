#include "ui/qt/dock_widget/sequences.hpp"
#include "ui/qt/widget/sequence.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/scene/scene_item_component.hpp>

namespace VTX::UI::QT::DockWidget
{
	Sequences::Sequences( QWidget * p_parent ) : Core::BaseDockWidget<Sequences, 1, 1>( "Sequence", p_parent )
	{
		setAllowedAreas( Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );

		// onSceneItemAdded.
		using namespace App::Component;
		using namespace App::Component::Scene;
		using namespace App::Component::Chemistry;

		App::SCENE().onSceneItemAdded += [ this ]( const SceneItemComponent & p_system )
		{
			if ( App::ECS_REGISTRY().hasComponent<App::Component::Chemistry::System>( p_system ) )
			{
				// Components.
				auto & system		  = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( p_system );
				auto * sequenceWidget = new Widget::Sequence( system, this );
				_layout->addWidget( sequenceWidget );
			}
		};
	}

} // namespace VTX::UI::QT::DockWidget
