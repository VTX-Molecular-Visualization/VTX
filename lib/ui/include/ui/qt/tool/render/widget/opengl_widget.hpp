#ifndef __VTX_UI_QT_WIDGET_OPENGL__
#define __VTX_UI_QT_WIDGET_OPENGL__

#include "ui/qt/widget/base_manual_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI::QT::Tool::Render::Widget
	{
		class OpenGLWidget : public QT::Widget::BaseManualWidget<QWidget>
		{
			NEW_ARCHI_VTX_WIDGET

		  public:
			~OpenGLWidget();

			void localize() override {}

		  protected:
			OpenGLWidget( QWidget * p_parent );

			void _setupUi( const QString & p_name ) override {}
			void _setupSlots() override {}
		};
	} // namespace UI::QT::Tool::Render::Widget

} // namespace VTX

#endif
