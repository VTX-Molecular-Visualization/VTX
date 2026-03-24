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
#include <renderer/camera.hpp>
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

		connect(
			&SELECTION(),
			&SelectionManager::selectionChanged,
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
					const auto [ ent, _ ] = App::ECS::getFirstEntityWithComponents<Renderer::Camera>();
					auto * cameraWidget	  = new Widget::Camera( ent, this );
					_layout->insertWidget( _layout->indexOf( _filler ), cameraWidget );
					break;
				}
			}
		);
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
