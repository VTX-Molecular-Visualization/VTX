#include "camera_quick_access.hpp"
#include "action/action_manager.hpp"
#include "action/setting.hpp"
#include "event/event.hpp"
#include "setting.hpp"
#include "ui/widget/settings/setting_widget_enum.hpp"
#include <QAction>
#include <QIcon>
#include <set>

namespace VTX::UI::Widget::Render::Overlay
{
	CameraQuickAccess::CameraQuickAccess( QWidget * p_parent ) : BaseOverlay( p_parent )
	{
		_registerEvent( Event::Global::SETTINGS_CHANGE );
	};

	void CameraQuickAccess::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SETTINGS_CHANGE )
		{
			const Event::VTXEventRef<std::set<Setting::PARAMETER>> & castedEvent
				= dynamic_cast<const Event::VTXEventRef<std::set<Setting::PARAMETER>> &>( p_event );

			if ( castedEvent.ref.find( Setting::PARAMETER::CAMERA_PROJECTION ) != castedEvent.ref.cend() )
				_refreshCameraProjectionButton();
		}
	}

	void CameraQuickAccess::_setupUi( const QString & p_name )
	{
		BaseOverlay::_setupUi( p_name );

		QAction * const toggleCameraProjectionAction = addAction( QIcon( ":/sprite/camera_orient_icon.png" ), "" );
		_cameraProjectionButton = dynamic_cast<QToolButton *>( widgetForAction( toggleCameraProjectionAction ) );

		_refreshCameraProjectionButton();
		_refreshSize();
	}

	void CameraQuickAccess::_setupSlots()
	{
		BaseOverlay::_setupSlots();

		connect( _cameraProjectionButton, &QToolButton::clicked, this, &CameraQuickAccess::_toggleCameraProjection );
	}

	void CameraQuickAccess::localize() {};

	void CameraQuickAccess::_refreshCameraProjectionButton()
	{
		const bool											  isPerspective = VTX_SETTING().getCameraPerspective();
		const Widget::Settings::VTXSettings::CameraProjection settingCameraProj
			= isPerspective ? Widget::Settings::VTXSettings::CameraProjection::PERSPECTIVE
							: Widget::Settings::VTXSettings::CameraProjection::ORTHOGRAPHIC;

		const QString text = QString::fromStdString(
			Widget::Settings::VTXSettings::CAMERA_PROJECTION_STR[ int( settingCameraProj ) ] );

		const QString iconPath = isPerspective ? ":/sprite/camera_projection_perspective_icon.png"
											   : ":/sprite/camera_projection_ortho_icon.png";

		_cameraProjectionButton->setText( text );
		_cameraProjectionButton->setIcon( QIcon::fromTheme( iconPath ) );
	}

	void CameraQuickAccess::_toggleCameraProjection()
	{
		const bool changeToPerspective = !VTX_SETTING().getCameraPerspective();
		VTX_ACTION( new Action::Setting::ChangeCameraProjectionToPerspective( changeToPerspective ) );
	}
} // namespace VTX::UI::Widget::Render::Overlay
