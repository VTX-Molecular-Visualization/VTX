#include "ui/qt/menu/selection.hpp"
#include "ui/qt/application.hpp"
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
		_clearDynamicSubMenus();

		addAction<Action::Selection::Show>();
		addAction<Action::Selection::Hide>();
		addAction<Action::Selection::Solo>();
		addSeparator();

		QAction * const colorSchemeAction = Application::getAction<Action::Selection::SetColorScheme>();
		_colorSchemeMenu				  = new ColorScheme( this );
		connect(
			_colorSchemeMenu,
			&ColorScheme::selected,
			this,
			[ colorSchemeAction ]( const ColorScheme::Selected & p_selected )
			{
				colorSchemeAction->setData( QVariant::fromValue( p_selected ) );
				colorSchemeAction->trigger();
			}
		);
		addMenu( _colorSchemeMenu );

		QAction * const representationAction = Application::getAction<Action::Selection::SetRepresentation>();
		_representationMenu					 = new Representation( this );
		connect(
			_representationMenu,
			&Representation::selected,
			this,
			[ representationAction ]( const App::ECS::Entity p_representation )
			{
				representationAction->setData( QVariant::fromValue( p_representation ) );
				representationAction->trigger();
			}
		);
		addMenu( _representationMenu );

		auto &	   reg		= REG();
		const auto entities = reg.view<App::System::Selection>();
		for ( auto entity : entities )
		{
			const auto & metadata = reg.get<App::System::Metadata>( entity );

			QString name = QString::fromStdString( metadata.name );

			const auto systemState = App::Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::SYSTEM } );
			if ( systemState == App::System::E_SELECTION_STATE::FULL )
			{
				addSeparator();
				auto * const action = addAction<Action::Selection::Delete>();
				continue;
			}
		}
	}

	void Selection::_clearDynamicSubMenus()
	{
		delete _colorSchemeMenu;
		_colorSchemeMenu = nullptr;

		delete _representationMenu;
		_representationMenu = nullptr;
	}
} // namespace VTX::UI::QT::Menu
