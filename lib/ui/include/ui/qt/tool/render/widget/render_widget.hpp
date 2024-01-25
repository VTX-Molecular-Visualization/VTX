#ifndef __VTX_UI_QT_TOOL_RENDER_WIDGET_RENDER__
#define __VTX_UI_QT_TOOL_RENDER_WIDGET_RENDER__

#include "ui/core/define.hpp"
#include "ui/qt/qt_panel.hpp"
#include "ui/qt/tool/render/widget/opengl_widget.hpp"
#include "ui/qt/widget/base_manual_widget.hpp"
#include <QFocusEvent>
#include <QVBoxLayout>
#include <QWidget>
#include <map>

namespace VTX::UI::QT::Tool::Render::Widget
{
	class RenderWidget : public QtPanelTemplate<QWidget>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		~RenderWidget();

		PANEL_TYPE getPanelType() const override { return PANEL_TYPE::CENTRAL_WIDGET; };

	  protected:
		RenderWidget( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		OpenGLWidget * _openGLWidget = nullptr;

		inline OpenGLWidget & getOpenGLWidget()
		{
			assert( _openGLWidget != nullptr );
			return *_openGLWidget;
		}
	};
} // namespace VTX::UI::QT::Tool::Render::Widget
#endif
