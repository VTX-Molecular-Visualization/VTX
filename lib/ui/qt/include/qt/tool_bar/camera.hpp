#ifndef __VTX_UI_QT_TOOL_BAR_CAMERA__
#define __VTX_UI_QT_TOOL_BAR_CAMERA__

#include "qt/base_widget.hpp"
#include "qt/helper.hpp"
#include "qt/resources.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Camera : public BaseWidget<Camera, QToolBar>
	{
	  public:
		Camera( QWidget * p_parent ) : BaseWidget<Camera, QToolBar>( "Camera", p_parent )
		{
			using namespace Helper;

			addQAction(
				this,
				{ "Perspective",
				  "Change projection mode (perspective/orthographic)",
				  Resources::SPRITE_CAMERA_PERSPECTIVE.toStdString() }
			);
			addSeparator();
			addQAction(
				this, { "Trackball", "Use Trackball controller", Resources::SPRITE_CAMERA_TRACKBALL.toStdString() }
			);
			addQAction( this, { "Freefly", "Use Freefly controller", Resources::SPRITE_CAMERA_FREEFLY.toStdString() } );
			addSeparator();
			addQAction(
				this, { "Orient", "Orient camera on selection", Resources::SPRITE_CAMERA_ORIENT.toStdString() }
			);
			addQAction( this, { "Reset", "Reset camera", Resources::SPRITE_CAMERA_RESET.toStdString() } );
		}

		virtual ~Camera() {}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
