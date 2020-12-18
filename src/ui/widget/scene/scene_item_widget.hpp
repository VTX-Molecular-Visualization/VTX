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

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Scene
			{
				class SceneItemWidget : public BaseManualWidget<QTreeWidget>
				{
					VTX_WIDGET

				  public:
					void		 localize() override;
					virtual void updatePosInSceneHierarchy( const int p_position )
					{
						int minimumHeight = rowHeight( model()->index( 0, 0 ) );
						setMinimumHeight( minimumHeight );
						setMinimumWidth( sizeHintForColumn( 0 ) );
					};

					virtual const Model::ID & getModelID() const = 0;

				  protected:
					SceneItemWidget( QWidget * p_parent );

					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

					void _onCustomContextMenuCalled( const QPoint & p_clicPos );

					void mousePressEvent( QMouseEvent * p_event ) override;
					void mouseMoveEvent( QMouseEvent * p_event ) override;
					void dragEnterEvent( QDragEnterEvent * p_event ) override;

					QMenu * _contextMenu = nullptr;

					virtual void		_deleteAction()	  = 0;
					virtual QMimeData * _getDataForDrag() = 0;

					const Qt::CheckState _getCheckState( const bool p_enable ) const { return p_enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked; };

				  private:
					QPoint _dragStartPosition;
				};

			} // namespace Scene
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX

#endif
