#include "base_camera_controller.hpp"
#include "object3d/camera.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/render/opengl_widget.hpp"
#include "vtx_app.hpp"
#include <QCursor>
#include <QPoint>
#include <QRect>
#include <QWidget>

namespace VTX::Controller
{
	void BaseCameraController::update( const float & p_deltaTime )
	{
		BaseMouseController::update( p_deltaTime );

		if ( _isOrienting )
		{
			_orientTime += p_deltaTime;

			if ( _orientTime >= ORIENT_DURATION )
			{
				_updateOrient( 1.f );
				_isOrienting = false;
			}
			else
			{
				_updateOrient( _orientTime / ORIENT_DURATION );
			}
		}
		else
		{
			_updateInputs( p_deltaTime );
		}
	}

	bool BaseCameraController::_mouseHoveringRenderWidget() const
	{
		const QWidget & renderWidget  = VTXApp::get().getMainWindow().getOpenGLWidget();
		const QRect &	renderRect	  = renderWidget.contentsRect();
		const QPoint	localMousePos = renderWidget.mapFromGlobal( QCursor::pos() );

		return renderRect.contains( localMousePos );
	}
} // namespace VTX::Controller
