#ifndef __VTX_UI_WIDGET_SCENE__
#define __VTX_UI_WIDGET_SCENE__

#include "model/base_model.hpp"
#include "scene_item_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
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

		void openRenameEditor( const Model::ID & p_itemID ) const;

	  protected:
		SceneWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void mousePressEvent( QMouseEvent * p_event ) override;
		void dragEnterEvent( QDragEnterEvent * p_event ) override;
		void mouseMoveEvent( QMouseEvent * p_event ) override;
		void dropEvent( QDropEvent * p_event ) override;

	  private:
		QWidget *					   _scrollAreaContent = nullptr;
		QVBoxLayout *				   _layout			  = nullptr;
		std::vector<SceneItemWidget *> _sceneWidgets	  = std::vector<SceneItemWidget *>();

		int _getPositionInHierarchy( SceneItemWidget * const p_sceneItemWidget );
		void _addWidgetInLayout( SceneItemWidget * const p_sceneItemWidget );
		void _removeWidgetInLayout( SceneItemWidget * const p_sceneItemWidget );
	};

} // namespace VTX::UI::Widget::Scene

#endif
