#ifndef __VTX_UI_QT_TOOL_RENDER_WIDGET_RENDER__
#define __VTX_UI_QT_TOOL_RENDER_WIDGET_RENDER__

#include "qt/core/base_manual_widget.hpp"
#include "qt/qt_panel.hpp"
#include "qt/widget/renderer/opengl.hpp"
#include <QFocusEvent>
#include <QVBoxLayout>
#include <QWidget>
#include <map>
#include <ui/core/define.hpp>

namespace VTX::UI::QT::Widget::Renderer
{
	class Panel : public QtPanelTemplate<QWidget>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		~Panel();

		PANEL_TYPE getPanelType() const override { return PANEL_TYPE::CENTRAL_WIDGET; };

		void makeCurrentContext();
		void doneCurrentContext();

	  protected:
		Panel( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		OpenGL * _openGLWidget = nullptr;

		inline OpenGL & getOpenGLWidget()
		{
			assert( _openGLWidget != nullptr );
			return *_openGLWidget;
		}
	};
} // namespace VTX::UI::QT::Widget::Renderer
#endif
