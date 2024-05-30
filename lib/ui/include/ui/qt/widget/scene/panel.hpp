#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_WIDGET__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_WIDGET__

#include "tree.hpp"
#include "ui/qt/qt_panel.hpp"

namespace VTX::UI::QT::Widget::Scene
{
	class Panel : public QtDockablePanel
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		void localize() override;

		inline const Tree * const getTreeWidget() const { return _treeWidget; }
		inline Tree * const		  getTreeWidget() { return _treeWidget; }

	  protected:
		Panel( QWidget * p_parent = nullptr );
		~Panel() = default;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		Tree * _treeWidget;
	};
} // namespace VTX::UI::QT::Widget::Scene

#endif
