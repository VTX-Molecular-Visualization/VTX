#include "ui/qt/dock_widget/inspector.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"

namespace VTX::UI::QT::DockWidget
{
	Inspector::Inspector( QWidget * p_parent ) : BaseDockWidget( "Selection", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		// Selection list widget.
		_selectionListWidget = new Widget::Selection( this );
		_layout->addWidget( _selectionListWidget );

		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

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
