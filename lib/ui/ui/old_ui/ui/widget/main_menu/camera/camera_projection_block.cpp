#include "camera_projection_block.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "id.hpp"
#include "ui/widget/settings/setting_widget_enum.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	CameraProjectionBlock::CameraProjectionBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::SETTINGS_CHANGE );
	};

	CameraProjectionBlock::~CameraProjectionBlock() {}

	void CameraProjectionBlock::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SETTINGS_CHANGE )
		{
			const Event::VTXEventRef<std::set<Setting::PARAMETER>> & castedEvent
				= dynamic_cast<const Event::VTXEventRef<std::set<Setting::PARAMETER>> &>( p_event );

			if ( castedEvent.ref.find( Setting::PARAMETER::CAMERA_PROJECTION ) != castedEvent.ref.end() )
			{
				_refreshCameraProjectionButton();
			}
		}
	}

	void CameraProjectionBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_cameraProjectionButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "cameraProjectionButton" );
		_cameraProjectionButton->setData( "", "", Qt::Orientation::Vertical );
		pushButton( *_cameraProjectionButton, 0 );

		validate();
		_refreshCameraProjectionButton();
	}

	void CameraProjectionBlock::localize() { setTitle( "Projection" ); }

	void CameraProjectionBlock::_setupSlots()
	{
		_cameraProjectionButton->setTriggerAction( this, &CameraProjectionBlock::_toggleCameraProjection );
	}

	void CameraProjectionBlock::_refreshCameraProjectionButton() const
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

	void CameraProjectionBlock::_toggleCameraProjection() const
	{
		const bool changeToPerspective = !VTX_SETTING().getCameraPerspective();
		VTX_ACTION( new Action::Setting::ChangeCameraProjectionToPerspective( changeToPerspective ) );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera
