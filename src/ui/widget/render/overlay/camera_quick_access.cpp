#include "camera_quick_access.hpp"
#include "action/action_manager.hpp"
#include "action/setting.hpp"
#include "event/event.hpp"
#include "mvc/mvc_manager.hpp"
#include "setting.hpp"
#include "ui/widget/settings/setting_widget_enum.hpp"
#include "vtx_app.hpp"
#include <QAction>
#include <QIcon>
#include <set>

namespace VTX::UI::Widget::Render::Overlay
{
	CameraQuickAccess::CameraQuickAccess( QWidget * p_parent ) : BaseOverlay( p_parent )
	{
		_registerEvent( Event::Global::SETTINGS_CHANGE );
		_registerEvent( Event::Global::APPLIED_RENDER_EFFECT_CHANGE );
	};
	CameraQuickAccess ::~CameraQuickAccess() {}

	void CameraQuickAccess::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SETTINGS_CHANGE )
		{
			const Event::VTXEventRef<std::set<Setting::PARAMETER>> & castedEvent
				= dynamic_cast<const Event::VTXEventRef<std::set<Setting::PARAMETER>> &>( p_event );

			if ( castedEvent.ref.find( Setting::PARAMETER::CAMERA_PROJECTION ) != castedEvent.ref.cend() )
				_refreshCameraProjectionButton();
		}
		else if ( p_event.name == Event::Global::APPLIED_RENDER_EFFECT_CHANGE )
		{
			_refreshCameraProjectionIconColor();

			MVC::MvcManager::get().deleteView( &VTX_RENDER_EFFECT(),
											   ID::View::UI_CAMERA_QUICK_ACCESS_ON_RENDER_EFFECT );

			_attachViewOnAppliedRenderEffect();
		}
	}

	void CameraQuickAccess::_attachViewOnAppliedRenderEffect()
	{
		RenderEffectView * const view = MVC::MvcManager::get().instantiateView<RenderEffectView>(
			&VTX_RENDER_EFFECT(), ID::View::UI_CAMERA_QUICK_ACCESS_ON_RENDER_EFFECT );

		view->setCallback( this, &CameraQuickAccess::_onRenderEffectChange );
	}

	void CameraQuickAccess::_setupUi( const QString & p_name )
	{
		BaseOverlay::_setupUi( p_name );

		_projectionOrthographicIcon.generateIcons( QImage( ":/sprite/camera_projection_ortho_icon.png" ) );
		_projectionPerspectiveIcon.generateIcons( QImage( ":/sprite/camera_projection_perspective_icon.png" ) );

		QAction * const toggleCameraProjectionAction = addAction( _projectionPerspectiveIcon.getIcon(), "" );
		_cameraProjectionButton = dynamic_cast<QToolButton *>( widgetForAction( toggleCameraProjectionAction ) );

		_attachViewOnAppliedRenderEffect();
		_refreshCameraProjectionButton();

		_refreshSize();
	}

	void CameraQuickAccess::_setupSlots()
	{
		BaseOverlay::_setupSlots();

		connect( _cameraProjectionButton, &QToolButton::clicked, this, &CameraQuickAccess::_toggleCameraProjection );
	}

	void CameraQuickAccess::localize() {};

	void CameraQuickAccess::_refreshCameraProjectionIconColor() const
	{
		const bool isPerspective = VTX_SETTING().getCameraPerspective();

		const UI::Object::BackgroundDependentIcon & bgDependantIcon
			= isPerspective ? _projectionPerspectiveIcon : _projectionOrthographicIcon;

		_cameraProjectionButton->setIcon( bgDependantIcon.getIcon( VTX_RENDER_EFFECT().getBackgroundColor() ) );
	}

	void CameraQuickAccess::_refreshCameraProjectionButton()
	{
		const bool											  isPerspective = VTX_SETTING().getCameraPerspective();
		const Widget::Settings::VTXSettings::CameraProjection settingCameraProj
			= isPerspective ? Widget::Settings::VTXSettings::CameraProjection::PERSPECTIVE
							: Widget::Settings::VTXSettings::CameraProjection::ORTHOGRAPHIC;

		const QString text = QString::fromStdString(
			Widget::Settings::VTXSettings::CAMERA_PROJECTION_STR[ int( settingCameraProj ) ] );
		const UI::Object::BackgroundDependentIcon & bgDependantIcon
			= isPerspective ? _projectionPerspectiveIcon : _projectionOrthographicIcon;

		_cameraProjectionButton->setText( text );
		_cameraProjectionButton->setIcon( bgDependantIcon.getIcon( VTX_RENDER_EFFECT().getBackgroundColor() ) );
	}

	void CameraQuickAccess::_toggleCameraProjection()
	{
		const bool changeToPerspective = !VTX_SETTING().getCameraPerspective();
		VTX_ACTION( new Action::Setting::ChangeCameraProjectionToPerspective( changeToPerspective ) );
	}

	void CameraQuickAccess::_onRenderEffectChange( const Event::VTXEvent * const p_event )
	{
		_refreshCameraProjectionIconColor();
	}
} // namespace VTX::UI::Widget::Render::Overlay
