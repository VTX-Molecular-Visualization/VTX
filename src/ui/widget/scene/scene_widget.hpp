#ifndef __VTX_UI_WIDGET_SCENE__
#define __VTX_UI_WIDGET_SCENE__

#include "generic/base_scene_item.hpp"
#include "model/base_model.hpp"
#include "scene_item_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/dock_window_main_widget.hpp"
#include "ui/widget_factory.hpp"
#include "view/base_view.hpp"
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QScrollArea>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QVariant>
#include <vector>

namespace VTX::UI::Widget::Scene
{
	class SceneWidget : public BaseManualWidget<QDockWidget>
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void localize() override;

		const std::vector<SceneItemWidget *> & getSceneItemWidgets() const { return _sceneWidgets; }
		SceneItemWidget *					   getPreviousSceneItemWidgets( SceneItemWidget * p_item ) const;
		SceneItemWidget *					   getNextSceneItemWidgets( SceneItemWidget * p_item ) const;

		SceneItemWidget * getSceneItemWidgetFromModel( const Model::BaseModel & p_model ) const;

		void openRenameEditor( const Model::ID & p_itemID ) const;
		void scrollToItem( const QTreeWidgetItem & p_item );

	  protected:
		SceneWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void mousePressEvent( QMouseEvent * p_event ) override;
		void dragEnterEvent( QDragEnterEvent * p_event ) override;
		void mouseMoveEvent( QMouseEvent * p_event ) override;
		void dropEvent( QDropEvent * p_event ) override;

	  private:
		CustomWidget::DockWindowMainWidget<QScrollArea> * _scrollArea		 = nullptr;
		QWidget *										  _scrollAreaContent = nullptr;
		QVBoxLayout *									  _layout			 = nullptr;
		std::vector<SceneItemWidget *>					  _sceneWidgets		 = std::vector<SceneItemWidget *>();

		int	 _getDefaultIndex( const ID::VTX_ID & p_itemTypeID ) const;
		void _addWidgetInLayout( QWidget * const p_sceneItemWidget, const int p_index );

		void _removeWidgetInLayout( SceneItemWidget * const p_sceneItemWidget );
		void _refreshItemIndex();

		int	 _findItemIndex( const Model::ID & p_modelID, const int p_startIndex = 0 ) const;
		void _swapItems( const int p_lhs, const int p_rhs );

		template<typename V,
				 typename M,
				 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<Generic::BaseSceneItem, M>::value>,
				 typename = std::enable_if<std::is_base_of<SceneItemWidget, V>::value>,
				 typename = std::enable_if<std::is_base_of<View::BaseView<M>, V>::value>>
		void instantiateSceneItem( M * const		   p_model,
								   const ID::VTX_ID &  p_viewID,
								   const std::string & p_widgetName = "" )
		{
			// Set no parent to not trigger ItemChange event during init
			V * const sceneItemWidget
				= WidgetFactory::get().instantiateViewWidget<V>( p_model, p_viewID, _scrollAreaContent, p_widgetName );

			const int index = int( _sceneWidgets.size() );
			_addWidgetInLayout( sceneItemWidget, index );
			_sceneWidgets.emplace_back( sceneItemWidget );
			sceneItemWidget->updatePosInSceneHierarchy( index );
		}

		template<typename V,
				 typename M,
				 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<SceneItemWidget, V>::value>,
				 typename = std::enable_if<std::is_base_of<View::BaseView<M>, V>::value>>
		void deleteSceneItem( M * const p_model, const ID::VTX_ID & p_viewID )
		{
			V * const sceneItemWidget = MVC::MvcManager::get().getView<V>( p_model, p_viewID );

			_removeWidgetInLayout( sceneItemWidget );

			MVC::MvcManager::get().deleteView<V>( p_model, p_viewID );
		}
	};

} // namespace VTX::UI::Widget::Scene

#endif
