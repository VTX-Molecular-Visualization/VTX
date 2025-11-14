#include "ui/qt/dock_widget/inspector.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/main_window.hpp"
#include "ui/qt/widget/tree.hpp"
#include <QFontDatabase>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>

namespace VTX::UI::QT::DockWidget
{
	Inspector::Inspector( QWidget * p_parent ) : BaseDockWidget( p_parent )
	{
		setWindowTitle( "Selection" );
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		// Selection toolbar.
		auto * toolbar = new QToolBar( this );
		toolbar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
		// toolbar->setIconSize( QSize( 18, 18 ) );
		auto * lockAction = Application::getAction<Action::Selection::Lock>();
		lockAction->setCheckable( true );
		toolbar->addAction( lockAction );
		toolbar->addAction( Application::getAction<Action::Selection::Save>() );
		toolbar->addAction( Application::getAction<Action::Selection::Clear>() );
		_layout->addWidget( toolbar );

		connect(
			lockAction,
			&QAction::toggled,
			this,
			[]( const bool p_locked )
			{
				MAIN_WINDOW()
					.findChild<Widget::Tree *>( Util::typeName<Widget::Tree>() )
					->setSelectionMode(
						p_locked ? QAbstractItemView::NoSelection : QAbstractItemView::ExtendedSelection
					);
			}
		);

		// Selection list widget.
		_selectionListWidget = new Widget::Selection( this );
		_layout->addWidget( _selectionListWidget );

		//_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

		// TODO: connect to App selection change, not QT selection model.
		auto & selectionModel = SELECTION();
		connect(
			&selectionModel,
			&QItemSelectionModel::selectionChanged,
			_selectionListWidget,
			&Widget::Selection::refreshSelection
		);
	}

} // namespace VTX::UI::QT::DockWidget
