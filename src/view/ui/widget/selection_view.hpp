#ifndef __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__
#define __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/selection.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "view/base_view.hpp"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
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
					class SelectionStyleItemDelegate : public QStyledItemDelegate
					{
					  public:
						void  paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const override;
						QSize sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const override;

					  protected:
						int _getSize() const;
					};

					VTX_WIDGET
					VTX_VIEW

				  public:
					void localize() override;

				  protected:
					void _setupUi( const QString & ) override;
					void _setupSlots() override;
					void _refreshView() override; // Debug only.

					static const int NAME_COLUMN_INDEX	 = 0;
					static const int REMOVE_COLUMN_INDEX = 1;

				  private:
					SelectionView( Model::Selection * const p_model, QWidget * const p_parent ) : View::BaseView<Model::Selection>( p_model ), BaseManualWidget( p_parent ) {}
					void _onItemClicked( QTreeWidgetItem *, int );

					bool			 _createTopLevelChildren( const uint p_count );
					bool			 _createChildren( QTreeWidgetItem & p_item, const uint p_count );
					inline Model::ID _getModelID( const QTreeWidgetItem & p_item ) const
					{
						const QVariant & dataID = p_item.data( NAME_COLUMN_INDEX, Qt::UserRole );
						return dataID.value<VTX::Model::ID>();
					}
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
