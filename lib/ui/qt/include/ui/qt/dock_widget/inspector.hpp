#ifndef __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__
#define __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include <QTextEdit>

namespace VTX::UI::QT::DockWidget
{

	class Inspector : public BaseDockWidget<Inspector>
	{
	  public:
		Inspector( QWidget * p_parent ) : BaseDockWidget( "Selection", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

			_view = new QTextEdit( this );
			_view->setReadOnly( true );
			_view->setLineWrapMode( QTextEdit::NoWrap );
			_layout->addWidget( _view );

			_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

			auto & selectionModel = SELECTION();
			connect( &selectionModel, &QItemSelectionModel::selectionChanged, this, &Inspector::_rebuild );
		}

	  private:
		/**
		 * @brief Current selection text view.
		 */
		QPointer<QTextEdit> _view;

		void _rebuild()
		{
			auto &				  selectionModel = SELECTION();
			const QModelIndexList rows			 = selectionModel.selectedRows();
			QString				  out;

			for ( const QModelIndex & index : rows )
			{
				if ( not index.isValid() )
				{
					out += "<invalid>";
					continue;
				}

				out += index.data( Qt::DisplayRole ).toString();
				out += "\n";
			}
			_view->setPlainText( out );
		}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
