#include "ui/old_ui/ui/widget/main_menu/camera/camera_projection_block.hpp"
#include "ui/old_ui/ui/widget/settings/setting_widget_enum.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <app/action/main.hpp>
#include <app/action/setting.hpp>
#include <app/event/global.hpp>
#include <app/old_app/id.hpp>

namespace VTX::UI::Widget::MainMenu::Camera
{
	CameraProjectionBlock::CameraProjectionBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( VTX::App::Event::Global::SETTINGS_CHANGE );
	};

	CameraProjectionBlock::~CameraProjectionBlock() {}

	void CameraProjectionBlock::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::SETTINGS_CHANGE )
		{
			const VTX::App::Core::Event::VTXEventArg<const std::set<VTX::App::Application::Setting::PARAMETER> &> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<const std::set<VTX::App::Application::Setting::PARAMETER> &> &>(
					p_event );

			if ( castedEvent.get().find( VTX::App::Application::Setting::PARAMETER::CAMERA_PROJECTION ) != castedEvent.get().end() )
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
		VTX_ACTION( new App::Action::Setting::ChangeCameraProjectionToPerspective( changeToPerspective ) );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera
