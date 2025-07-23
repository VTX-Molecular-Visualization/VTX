#include "ui/qt/dock_widget/sequences.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/widget/sequence.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/representation/color_layout.hpp>
#include <app/component/scene/scene_item_component.hpp>
#include <core/struct/color_layout.hpp>
#include <util/string.hpp>

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
			Util::Chrono timer;
			timer.start();

			if ( App::ECS_REGISTRY().hasComponent<App::Component::Chemistry::System>( p_system ) )
			{
				// Components.
				auto &		 system = App::ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( p_system );
				const auto & scene	= App::SCENE();
				const auto & colorlayout
					= App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
						App::ECS_REGISTRY().getEntity( scene )
					);

				auto * sequenceWidget = new Widget::Sequence( this );

				QVector<QChar>	residues;
				QVector<QColor> colors;

				for ( const auto & chain : system.getChains() )
				{
					for ( size_t i = 0; i < chain->getResidueCount(); ++i )
					{
						const auto * residue = system.getResidue( chain->getIndexFirstResidue() + i );
						residues.append( residue->getShortName().at( 0 ) );
						colors.append(
							Helper::toQColor( colorlayout.getResidueColor( size_t( residue->getSymbol() ) ) )
						);
					}
				}

				sequenceWidget->setSequence( residues, colors );
				connect(
					sequenceWidget,
					&Widget::Sequence::residueClicked,
					this,
					[]( int index ) { qDebug() << "Residue clicked:" << index; }
				);

				_layout->addWidget( sequenceWidget );
			}

			float time = timer.elapsedTime();
			VTX_ERROR( "Sequence loading duration: {}", Util::String::durationToStr( time ) );
		};
	}

} // namespace VTX::UI::QT::DockWidget
