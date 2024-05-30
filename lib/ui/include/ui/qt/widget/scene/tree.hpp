#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_TREE_WIDGET__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_TREE_WIDGET__

#include "ui/qt/core/base_manual_widget.hpp"
#include <QTreeWidget>

namespace VTX::UI::QT::Widget::Scene
{
	class Tree : public QT::Core::BaseManualWidget<QTreeWidget>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		void localize() override;

	  protected:
		Tree( QWidget * p_parent = nullptr );
		~Tree() = default;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
	};
} // namespace VTX::UI::QT::Widget::Scene

#endif
