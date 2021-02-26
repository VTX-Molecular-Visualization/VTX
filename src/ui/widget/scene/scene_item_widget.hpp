#ifndef __VTX_UI_WIDGET_SCENE_ITEM__
#define __VTX_UI_WIDGET_SCENE_ITEM__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/base_model.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QTreeWidget>

namespace VTX::UI::Widget::Scene
{
	class SceneItemWidget : public BaseManualWidget<QTreeWidget>
	{
		VTX_WIDGET

	  public:
		void		 localize() override;
		virtual void updatePosInSceneHierarchy( const int p_position );

		virtual const Model::ID & getModelID() const = 0;

	  protected:
		SceneItemWidget( QWidget * p_parent );

		virtual bool _canDragObjectAtPos( const QPoint & p_position ) { return true; }

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		virtual void mousePressEvent( QMouseEvent * p_event ) override;
		virtual void mouseMoveEvent( QMouseEvent * p_event ) override;
		void		 dragEnterEvent( QDragEnterEvent * p_event ) override;

		virtual void _onItemExpanded( QTreeWidgetItem * const );
		virtual void _onItemCollapsed( QTreeWidgetItem * const );

		virtual void _refreshSize();
		virtual int	 _getMinimumHeight() const;

		virtual QMimeData * _getDataForDrag() = 0;

		const Qt::CheckState _getCheckState( const bool p_enable ) const
		{
			return p_enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
		};

	  private:
		QPoint _dragStartPosition;
	};

} // namespace VTX::UI::Widget::Scene

#endif
