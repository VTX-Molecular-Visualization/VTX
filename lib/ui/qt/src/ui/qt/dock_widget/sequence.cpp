#include "ui/qt/dock_widget/sequence.hpp"
#include "ui/qt/helper.hpp"
#include <QHeaderView>
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
	Sequence::Sequence( QWidget * p_parent ) : Core::BaseDockWidget<Sequence, 1, 1>( "Sequence", p_parent )
	{
		setAllowedAreas( Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );

		// onSceneItemAdded.
		using namespace App::Component;
		using namespace App::Component::Scene;
		using namespace App::Component::Chemistry;

		App::SCENE().onSceneItemAdded += [ this ]( const SceneItemComponent & p_system )
		{
			return;

			setUpdatesEnabled( false );

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

				// Size.
				const size_t chainCount	  = system.getChains().size();
				const size_t residueCount = system.getResidues().size();
				const size_t columnCount  = 1 + chainCount + residueCount;

				// Table.
				auto * table = new QTableWidget( 2, int( columnCount ), this );
				table->setContentsMargins( 0, 0, 0, 0 );
				table->setEditTriggers( QAbstractItemView::NoEditTriggers );
				table->setSelectionMode( QAbstractItemView::NoSelection );
				table->setShowGrid( false );
				table->verticalHeader()->hide();
				table->horizontalHeader()->hide();
				// table->setUpdatesEnabled( false );
				table->setSortingEnabled( false );

				// Default item.
				auto * const defaultItem = new QTableWidgetItem();
				defaultItem->setTextAlignment( Qt::AlignCenter );
				QFont fontBold = defaultItem->font();
				fontBold.setBold( true );

				// System.
				auto * item = defaultItem->clone();
				item->setText( system.getName().c_str() );
				item->setFont( fontBold );
				table->setItem( 0, 0, item );
				int	   index		= 1;
				size_t currentChain = 0;
				for ( const auto & chain : system.getChains() )
				{
					// Chain.
					item = defaultItem->clone();
					item->setText( chain->getName().c_str() );
					item->setFont( fontBold );
					const QColor colorChain
						= Helper::toQColor( colorlayout.getChainColor( ( currentChain % 26 ) + 1 ) );
					item->setForeground( colorChain );
					table->setItem( 0, index++, item );
					for ( size_t i = 0; i < chain->getResidueCount(); ++i )
					{
						// Residue.
						const size_t	   r	   = chain->getIndexFirstResidue() + i;
						const auto * const residue = system.getResidue( r );
						const QColor	   colorResidue
							= Helper::toQColor( colorlayout.getResidueColor( size_t( residue->getSymbol() ) ) );

						// First row.
						if ( i == 0 || ( i + 1 ) % 5 == 0 )
						{
							item = defaultItem->clone();
							item->setText( std::to_string( i + 1 ).c_str() );
							item->setForeground( colorChain );
							table->setItem( 0, index, item );
						}

						// Second row.
						item = defaultItem->clone();
						item->setText( residue->getShortName().data() );
						item->setForeground( colorResidue );
						item->setData( Qt::UserRole, QVariant::fromValue( r ) );
						table->setItem( 1, index, item );
						index++;
					}

					currentChain++;
					table->setItem( 0, index, new QTableWidgetItem( chain->getName().c_str() ) );
				}

				table->resizeColumnsToContents();
				table->resizeRowsToContents();
				table->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );

				_systemComponents.emplace( table, &system );

				_layout->addWidget( table );
			}

			setUpdatesEnabled( true );

			float time = timer.elapsedTime();
			VTX_ERROR( "Sequence loading duration: {}", Util::String::durationToStr( time ) );
		};
	}
} // namespace VTX::UI::QT::DockWidget
