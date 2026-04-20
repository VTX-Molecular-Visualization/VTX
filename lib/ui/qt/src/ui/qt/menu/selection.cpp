#include "ui/qt/menu/selection.hpp"
#include "ui/qt/menu/color_scheme.hpp"
#include "ui/qt/menu/representation.hpp"
#include <app/action/scene.hpp>
#include <app/helper/system.hpp>
#include <app/system/metadata.hpp>
#include <app/system/selection.hpp>
#include <core/struct/topology.hpp>

namespace VTX::UI::QT::Menu
{
	Selection::Selection( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setTitle( "Selection" );

		// Refresh menu when opened.
		connect( this, &QMenu::aboutToShow, this, &Selection::_refresh );
	}

	void Selection::_refresh()
	{
		using namespace App;
		using namespace Core::Struct;

		// Clear previous actions.
		clear();

		auto &	   reg		= REG();
		const auto entities = reg.view<App::System::Selection>();

		std::set<App::ECS::Entity> _fullSelected;

		for ( auto entity : entities )
		{
			// const auto & topology = reg.get<Core::Struct::Topology>( entity );
			//  const auto & selection = reg.get<App::System::Selection>( entity );
			const auto & metadata = reg.get<App::System::Metadata>( entity );

			QString name = QString::fromStdString( metadata.name );

			const auto systemState = App::Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::SYSTEM } );
			if ( systemState == App::System::E_SELECTION_STATE::FULL )
			{
				_fullSelected.emplace( entity );
			}
		}

		if ( not _fullSelected.empty() )
		{
			addSection( "Danger zone" );
			auto * const action = addAction<Action::System::Delete>();
		}
	}
} // namespace VTX::UI::QT::Menu
