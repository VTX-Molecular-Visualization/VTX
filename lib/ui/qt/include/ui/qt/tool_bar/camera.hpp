#ifndef VTX_UI_QT_TOOL_BAR_CAMERA_HPP
#define VTX_UI_QT_TOOL_BAR_CAMERA_HPP

#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QToolBar>

namespace VTX::UI::QT::ToolBar
{

	class Camera : public Widget::BaseWidget<Camera, QToolBar>
	{
	  public:
		Camera( QWidget * const p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "Camera" );

			UI_ACTIONS().addTo( *this, Action::Camera::PERSPECTIVE );
			UI_ACTIONS().addTo( *this, Action::Camera::ORTHOGRAPHIC );
			addSeparator();
			UI_ACTIONS().addTo( *this, Action::Camera::TRACKBALL );
			UI_ACTIONS().addTo( *this, Action::Camera::FREEFLY );
			addSeparator();
			UI_ACTIONS().addTo( *this, Action::Camera::ORIENT );
			UI_ACTIONS().addTo( *this, Action::Camera::RESET );
		}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
