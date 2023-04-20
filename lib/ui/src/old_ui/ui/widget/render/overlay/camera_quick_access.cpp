#include "ui/old_ui/ui/widget/render/overlay/camera_quick_access.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/renderer/render_effect_library_combo_box.hpp"
#include "ui/old_ui/ui/widget/settings/setting_widget_enum.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QAction>
#include <QIcon>
#include <app/core/action/action_manager.hpp>
#include <app/action/renderer.hpp>
#include <app/action/setting.hpp>
#include <app/event/vtx_event.hpp>
#include <app/old_app/id.hpp>
#include <app/core/mvc/mvc_manager.hpp>
#include <app/old_app/setting.hpp>
#include <set>

namespace VTX::UI::Widget::Render::Overlay
{
	CameraQuickAccess::CameraQuickAccess( QWidget * p_parent ) : BaseOverlay( p_parent )
	{
		_registerEvent( VTX::Event::Global::SETTINGS_CHANGE );
		_registerEvent( VTX::Event::Global::APPLIED_RENDER_EFFECT_CHANGE );
	};
	CameraQuickAccess ::~CameraQuickAccess() {}

	void CameraQuickAccess::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::Global::SETTINGS_CHANGE )
		{
			const VTX::Event::VTXEventRef<std::set<Setting::PARAMETER>> & castedEvent
				= dynamic_cast<const VTX::Event::VTXEventRef<std::set<Setting::PARAMETER>> &>( p_event );

			if ( castedEvent.ref.find( Setting::PARAMETER::CAMERA_PROJECTION ) != castedEvent.ref.cend() )
				_refreshCameraProjectionButton();
		}
		else if ( p_event.name == VTX::Event::Global::APPLIED_RENDER_EFFECT_CHANGE )
		{
			_refreshIconColors();

			VTX::Core::MVC::MvcManager::get().deleteView( &VTX_RENDER_EFFECT(),
											   ID::View::UI_CAMERA_QUICK_ACCESS_ON_RENDER_EFFECT );

			_attachViewOnAppliedRenderEffect();

			_refreshRenderEffectMenu();
		}
	}

	void CameraQuickAccess::_setupUi( const QString & p_name )
	{
		BaseOverlay::_setupUi( p_name );

		_projectionOrthographicIcon.generateIcons( QImage( ":/sprite/camera_projection_ortho_icon.png" ) );
		_projectionPerspectiveIcon.generateIcons( QImage( ":/sprite/camera_projection_perspective_icon.png" ) );

		QAction * const toggleCameraProjectionAction = addAction( _projectionPerspectiveIcon.getIcon(), "" );
		_cameraProjectionButton = dynamic_cast<QToolButton *>( widgetForAction( toggleCameraProjectionAction ) );
		_cameraProjectionButton->setToolTip( "Camera projection" );

		_renderEffectLibraryMenu = new QMenu( this );
		QToolButton * const renderEffectButton
			= dynamic_cast<QToolButton *>( widgetForAction( addMenu( _renderEffectLibraryMenu ) ) );
		renderEffectButton->setIcon( QIcon( ":/sprite/render_effect_preset_icon.png" ) );
		renderEffectButton->setToolTip( "Render effect preset" );

		_exportImageIcon.generateIcons( QImage( ":/sprite/screenshot_icon.png" ) );
		QAction * const exportImageAction = addAction( _exportImageIcon.getIcon(), "" );
		_exportImageButton				  = dynamic_cast<QToolButton *>( widgetForAction( exportImageAction ) );
		_exportImageButton->setToolTip( "Export Image..." );

		_refreshRenderEffectMenu();
		_attachViewOnAppliedRenderEffect();
		_refreshCameraProjectionButton();
		_refreshIconColors();
	}

	void CameraQuickAccess::_setupSlots()
	{
		BaseOverlay::_setupSlots();

		connect( _renderEffectLibraryMenu,
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &CameraQuickAccess::_applyRenderEffectPresetAction );
		connect( _cameraProjectionButton, &QToolButton::clicked, this, &CameraQuickAccess::_toggleCameraProjection );
		connect( _exportImageButton, &QToolButton::clicked, this, &CameraQuickAccess::_onExportImageClickedAction );
	}

	void CameraQuickAccess::localize() {};

	void CameraQuickAccess::_refreshIconColors() const
	{
		const UI::Object::BackgroundDependentIcon & cameraProjectionIcon
			= VTX_SETTING().getCameraPerspective() ? _projectionPerspectiveIcon : _projectionOrthographicIcon;

		_cameraProjectionButton->setIcon( cameraProjectionIcon.getIcon( VTX_RENDER_EFFECT().getBackgroundColor() ) );
		_exportImageButton->setIcon( _exportImageIcon.getIcon( VTX_RENDER_EFFECT().getBackgroundColor() ) );
	}

	void CameraQuickAccess::_refreshRenderEffectMenu() const
	{
		_renderEffectLibraryMenu->clear();

		const int appliedPresetIndex = Model::Renderer::RenderEffectPresetLibrary::get().getAppliedPresetIndex();

		for ( int i = 0; i < Model::Renderer::RenderEffectPresetLibrary::get().getPresetCount(); i++ )
		{
			const Model::Renderer::RenderEffectPreset * const preset
				= Model::Renderer::RenderEffectPresetLibrary::get().getPreset( i );

			QAction * const action = _renderEffectLibraryMenu->addAction( QString::fromStdString( preset->getName() ) );

			action->setCheckable( true );
			action->setChecked( i == appliedPresetIndex );

			action->setData( QVariant( i ) );
		}

		QAction * const action = _renderEffectLibraryMenu->addAction( "Settings..." );
		action->setData( QVariant( -1 ) );
	}

	void CameraQuickAccess::_refreshCameraProjectionButton()
	{
		const bool isPerspective = VTX_SETTING().getCameraPerspective();

		const UI::Object::BackgroundDependentIcon & bgDependantIcon
			= isPerspective ? _projectionPerspectiveIcon : _projectionOrthographicIcon;

		_cameraProjectionButton->setIcon( bgDependantIcon.getIcon( VTX_RENDER_EFFECT().getBackgroundColor() ) );
	}

	void CameraQuickAccess::_toggleCameraProjection()
	{
		const bool changeToPerspective = !VTX_SETTING().getCameraPerspective();
		VTX_ACTION( new Action::Setting::ChangeCameraProjectionToPerspective( changeToPerspective ) );
	}

	void CameraQuickAccess::_applyRenderEffectPresetAction( const QAction * const p_action )
	{
		const int renderEffectPreset = p_action->data().toInt();

		// data == -1 => Settings else preset
		if ( renderEffectPreset == -1 )
		{
			VTXApp::get().getMainWindow().openSettingWindow( UI::Widget::Settings::SETTING_MENU::RENDER_EFFECTS );
		}
		else
		{
			Model::Renderer::RenderEffectPreset * const preset
				= Model::Renderer::RenderEffectPresetLibrary::get().getPreset( renderEffectPreset );

			VTX_ACTION( new Action::Renderer::ApplyRenderEffectPreset( *preset ) );
		}
	}
	void CameraQuickAccess::_onExportImageClickedAction() const { UI::Dialog::openAdvancedSettingImageExportDialog(); }

	void CameraQuickAccess::_onRenderEffectChange( const VTX::Event::VTXEvent * const p_event )
	{
		_refreshIconColors();
	}

	void CameraQuickAccess::_attachViewOnAppliedRenderEffect()
	{
		RenderEffectView * const view = VTX::Core::MVC::MvcManager::get().instantiateView<RenderEffectView>(
			&VTX_RENDER_EFFECT(), ID::View::UI_CAMERA_QUICK_ACCESS_ON_RENDER_EFFECT );

		view->setCallback( this, &CameraQuickAccess::_onRenderEffectChange );
	}
} // namespace VTX::UI::Widget::Render::Overlay
