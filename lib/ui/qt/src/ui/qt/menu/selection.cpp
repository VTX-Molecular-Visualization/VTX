#include "ui/qt/menu/selection.hpp"
#include "ui/qt/services.hpp"
#include <app/action/scene.hpp>
#include <app/helper/system.hpp>
#include <app/system/metadata.hpp>
#include <app/system/selection.hpp>
#include <core/struct/topology.hpp>
#include <string>
#include <util/types.hpp>

namespace VTX::UI::QT::Menu
{
	namespace
	{
		ActionRegistry::ActionParams colorSchemeParams( const ColorScheme::Selected & p_selected )
		{
			ActionRegistry::ActionParams params {
				{ std::string( Action::Selection::PARAM_COLOR_SCHEME ),
				  static_cast<int>( toUnderlying( p_selected.scheme ) ) }
			};

			if ( p_selected.index )
			{
				params.emplace(
					std::string( Action::Selection::PARAM_COLOR_INDEX ),
					static_cast<int>( *p_selected.index )
				);
			}

			return params;
		}

		ActionRegistry::ActionParams representationParams( const App::ECS::Entity p_representation )
		{
			return ActionRegistry::ActionParams {
				{ std::string( Action::Selection::PARAM_REPRESENTATION ),
				  static_cast<int>( toUnderlying( p_representation ) ) }
			};
		}
	} // namespace

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
		delete _colorSchemeMenu;
		delete _representationMenu;

		addAction( Action::Selection::SHOW );
		addAction( Action::Selection::HIDE );
		// addAction( Action::Selection::SOLO );
		addSeparator();

		_colorSchemeMenu = new ColorScheme( this );
		connect(
			_colorSchemeMenu,
			&ColorScheme::selected,
			this,
			[]( const ColorScheme::Selected & p_selected )
			{
				UI_ACTIONS().trigger( Action::Selection::SET_COLOR_SCHEME, colorSchemeParams( p_selected ) );
			}
		);
		addMenu( _colorSchemeMenu );

		_representationMenu = new Representation( this );
		connect(
			_representationMenu,
			&Representation::selected,
			this,
			[]( const App::ECS::Entity p_representation )
			{
				UI_ACTIONS().trigger(
					Action::Selection::SET_REPRESENTATION, representationParams( p_representation )
				);
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
				auto * const action = addAction( Action::Selection::DELETE );
				continue;
			}
		}
	}

} // namespace VTX::UI::QT::Menu
