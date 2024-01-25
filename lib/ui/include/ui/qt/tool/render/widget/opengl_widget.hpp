#ifndef __VTX_UI_QT_WIDGET_OPENGL__
#define __VTX_UI_QT_WIDGET_OPENGL__

// Before QOpenGLWidget to use glad instead of QOpenGLFunctions.
#include <renderer/renderer.hpp>
//
#include "ui/qt/widget/base_manual_widget.hpp"
#include <QOpenGLWidget>
#include <QWidget>

namespace VTX
{
	namespace UI::QT::Tool::Render::Widget
	{
		class OpenGLWidget : public QT::Widget::BaseManualWidget<QOpenGLWidget>
		{
			NEW_ARCHI_VTX_WIDGET

		  public:
			// using CallbackInitGL   = std::function<void()>;
			// using CallbackResizeGL = std::function<void( int, int )>;

			~OpenGLWidget();

			void initializeGL() override;
			void paintGL() override;
			void resizeGL( int, int ) override;
			void localize() override {}

			// inline void setCallbackInitGL( const CallbackInitGL & p_cb ) { _cbInitGL = p_cb; }

		  protected:
			OpenGLWidget( QWidget * p_parent );

			void _setupUi( const QString & p_name ) override {}
			void _setupSlots() override {}

		  private:
			// CallbackInitGL	 _cbInitGL;
			// CallbackResizeGL _cbResizeGL;
		};
	} // namespace UI::QT::Tool::Render::Widget

} // namespace VTX

#endif
