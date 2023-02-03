#ifndef __VTX_UI_QT_QT_PANEL__
#define __VTX_UI_QT_QT_PANEL__

#include "core/base_panel.hpp"
#include "qt/widget/base_manual_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT
{
	class QtPanel : public Core::BasePanel
	{
	  public:
		enum class PANEL_TYPE : int
		{
			DOCK_WIDGET,
			FLOATING_WINDOW,
			CENTRAL_WIDGET,
		};

	  public:
		QtPanel();
		~QtPanel();

		virtual PANEL_TYPE getPanelType() const = 0;
	};

	class QtDockablePanel : public QtPanel, public Widget::BaseManualWidget<QDockWidget>
	{
	  public:
		class LayoutData
		{
		  public:
			bool floating;
			bool visible;

			// For tabified
			Qt::DockWidgetArea widgetArea;
			Qt::Orientation	   orientation;

			// For floating
			QSize size;
		};

	  public:
		QtDockablePanel();
		~QtDockablePanel();

		virtual PANEL_TYPE getPanelType() const override { return PANEL_TYPE::DOCK_WIDGET; };
	};

	class QtFloatingWindowPanel : public QtPanel
	{
	  public:
		QtFloatingWindowPanel();
		~QtFloatingWindowPanel();

		virtual PANEL_TYPE getPanelType() const override { return PANEL_TYPE::FLOATING_WINDOW; };
	};

} // namespace VTX::UI::QT

#endif
