#include "ui/qt/menu/selection.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include <app/helper/system.hpp>
#include <app/system/selection.hpp>
#include <core/struct/topology.hpp>
#include <io/metadata.hpp>
#include <string>
#include <util/types.hpp>

namespace VTX::UI::QT::Menu
{
	namespace
	{
		ActionRegistry::ActionParams colorSchemeParams( const ColorScheme::Selected & p_selected )
		{
			ActionRegistry::ActionParams params { { std::string( Action::Selection::PARAM_COLOR_SCHEME ),
													static_cast<int>( toUnderlying( p_selected.scheme ) ) } };

			if ( p_selected.index )
			{
				params.emplace( std::string( Action::Selection::PARAM_COLOR_INDEX ),
								static_cast<int>( *p_selected.index ) );
			}

			return params;
		}

		ActionRegistry::ActionParams representationParams( const Entity p_representation )
		{
			return ActionRegistry::ActionParams { { std::string( Action::Selection::PARAM_REPRESENTATION ),
													static_cast<int>( toUnderlying( p_representation ) ) } };
		}
	} // namespace

	Selection::Selection( QWidget * const p_parent ) : BaseWidget( p_parent )
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
		delete _extendMenu;

		UI_ACTIONS().addTo( *this, Action::Selection::SHOW );
		UI_ACTIONS().addTo( *this, Action::Selection::HIDE );
		// UI_ACTIONS().addTo( *this, Action::Selection::SOLO );
		addSeparator();

		_colorSchemeMenu = Helper::makeQObject<ColorScheme>( this );
		connect( _colorSchemeMenu,
				 &ColorScheme::selected,
				 this,
				 []( const ColorScheme::Selected & p_selected )
				 { UI_ACTIONS().trigger( Action::Selection::SET_COLOR_SCHEME, colorSchemeParams( p_selected ) ); } );
		addMenu( _colorSchemeMenu );

		_representationMenu = Helper::makeQObject<Representation>( this );
		connect( _representationMenu,
				 &Representation::selected,
				 this,
				 []( const Entity p_representation )
				 {
					 UI_ACTIONS().trigger( Action::Selection::SET_REPRESENTATION,
										   representationParams( p_representation ) );
				 } );
		addMenu( _representationMenu );
		addSeparator();

		UI_ACTIONS().addTo( *this, Action::Selection::SELECT_ALL );

		_extendMenu = Helper::makeQObject<QMenu>( this );
		_extendMenu->setTitle( "Extend" );
		UI_ACTIONS().addTo( *_extendMenu, Action::Selection::EXTEND_TO_RESIDUE );
		_extendMenu->addSeparator();
		for ( const double distance : { 3.0, 5.0, 8.0 } )
		{
			QAction * const action = _extendMenu->addAction( QString( "Within %1 Å" ).arg( distance ) );
			connect(
				action,
				&QAction::triggered,
				[ distance ]()
				{
					UI_ACTIONS().trigger(
						Action::Selection::EXTEND,
						ActionRegistry::ActionParams { { std::string( Action::Selection::PARAM_DISTANCE ), distance } }
					);
				}
			);
		}
		UI_ACTIONS().addTo( *_extendMenu, Action::Selection::EXTEND );
		_extendMenu->setEnabled( not UI_ACTIONS().isChecked( Action::Selection::LOCK ) );
		addMenu( _extendMenu );
		UI_ACTIONS().addTo( *this, Action::Selection::INVERT );

		UI_ACTIONS().addTo( *this, Action::Selection::CLEAR );

		auto &	   reg		= REG();
		const auto entities = reg.view<App::System::Selection>();
		for ( auto entity : entities )
		{
			const auto & metadata = reg.get<IO::Metadata>( entity );

			const QString name = QString::fromStdString( metadata.name );

			const auto systemState = App::Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::SYSTEM } );
			if ( systemState == App::System::E_SELECTION_STATE::FULL )
			{
				addSeparator();
				UI_ACTIONS().addTo( *this, Action::Selection::DELETE );
				continue;
			}
		}
	}

} // namespace VTX::UI::QT::Menu
