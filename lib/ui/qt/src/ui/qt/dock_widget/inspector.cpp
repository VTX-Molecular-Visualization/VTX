#include "ui/qt/dock_widget/inspector.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/camera.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QFontDatabase>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <app/action/selection.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{
	Inspector::Inspector( QWidget * p_parent ) : BaseDockWidget( p_parent, "Selection" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::SELECTION ) );

		_filler = new QWidget( this );
		_filler->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
		_layout->addWidget( _filler );

		connect( &SELECTION(), &SelectionManager::cleared, this, &Inspector::_clear );

		connect(
			&SELECTION(),
			&SelectionManager::selected,
			this,
			[ this ]( const E_SELECTION_GROUP p_group, const QItemSelection & p_selection )
			{
				// Clear all.
				_clear();

				if ( p_selection.isEmpty() )
				{
					return;
				}

				// Insert widget.
				switch ( p_group )
				{
				case E_SELECTION_GROUP::CAMERA:
					const auto [ ent, _, __ ]
						= App::ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
					auto * cameraWidget = new Widget::Camera( ent, this );
					_layout->insertWidget( _layout->indexOf( _filler ), cameraWidget );
					break;
				}
			}
		);

		/*
		// Selection toolbar.
		auto * toolbar = new QToolBar( this );
		toolbar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
		// toolbar->setIconSize( QSize( 18, 18 ) );
		auto * lockAction = Application::getAction<Action::Selection::Lock>();
		lockAction->setCheckable( true );
		toolbar->addAction( lockAction );
		toolbar->addAction( Application::getAction<Action::Selection::Save>() );
		toolbar->addAction( Application::getAction<Action::Selection::Clear>() );
		//_layout->addWidget( toolbar );

		// Picking granularity combobox.
		auto * cbPickingGranularity = new QComboBox( this );
		cbPickingGranularity->addItem(
			"Atom", QVariant( toUnderlying( App::Action::Selection::E_GRANULARITY::ATOM ) )
		);
		cbPickingGranularity->addItem(
			"Residue", QVariant( toUnderlying( App::Action::Selection::E_GRANULARITY::RESIDUE ) )
		);
		cbPickingGranularity->addItem(
			"Chain", QVariant( toUnderlying( App::Action::Selection::E_GRANULARITY::CHAIN ) )
		);
		cbPickingGranularity->addItem(
			"System", QVariant( toUnderlying( App::Action::Selection::E_GRANULARITY::SYSTEM ) )
		);
		_layout->addWidget( cbPickingGranularity );

		// Selection list widget.
		_selectionListWidget = new Widget::Selection( this );
		_layout->addWidget( _selectionListWidget );

		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

		lockAction->setChecked( SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool() );

		connect(
			lockAction,
			&QAction::toggled,
			this,
			[]( const bool p_locked )
			{
				SETTINGS().setValue( SETTING_KEY_LOCK_SELECTION, p_locked );
				App::HUB().trigger<Events::SelectionLocked>( p_locked );
			}
		);

		const int index = cbPickingGranularity->findData( SETTINGS().value( SETTING_KEY_GRANULARITY, 0 ).toInt() );
		if ( index != -1 )
		{
			cbPickingGranularity->setCurrentIndex( index );
		}

		connect(
			cbPickingGranularity,
			&QComboBox::currentIndexChanged,
			this,
			[ cbPickingGranularity ]( const int )
			{ SETTINGS().setValue( SETTING_KEY_GRANULARITY, cbPickingGranularity->currentData() ); }
		);
		*/
	}

	void Inspector::_clear()
	{
		for ( int i = _layout->count() - 1; i >= 0; --i )
		{
			if ( _layout->itemAt( i )->widget() != _filler )
			{
				QWidget * w = _layout->itemAt( i )->widget();
				_layout->removeWidget( w );
				w->deleteLater();
			}
		}
	}

} // namespace VTX::UI::QT::DockWidget
