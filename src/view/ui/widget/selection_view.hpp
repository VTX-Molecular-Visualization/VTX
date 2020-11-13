#ifndef __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__
#define __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/selection.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "view/base_view.hpp"
#include <QTreeWidgetItem>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class SelectionView : public View::BaseView<Model::Selection>, public VTX::UI::Widget::BaseManualWidget<QTreeWidget>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void		 localize() override;
					virtual void notify( const Event::VTXEvent * const p_event ) override;

				  protected:
					SelectionView( Model::Selection * const p_model, QWidget * const p_parent ) : View::BaseView<Model::Selection>( p_model ), BaseManualWidget( p_parent ) {}
					void _setupUi( const QString & ) override;
					void _setupSlots() override;
					void _refreshView() override;

				  private:
					void _clear();

					void _onItemClicked( QTreeWidgetItem *, int );

					inline Model::ID _getModelID( const QTreeWidgetItem & p_item ) const
					{
						const QVariant & dataID = p_item.data( 0, Qt::UserRole );
						return dataID.value<VTX::Model::ID>();
					}
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
