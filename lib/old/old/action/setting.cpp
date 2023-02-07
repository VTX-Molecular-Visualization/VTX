#include "setting.hpp"
#include "action/action_manager.hpp"
#include "io/reader/serialized_object.hpp"
#include "io/writer/serialized_object.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "model/representation/representation_library.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "renderer/base_renderer.hpp"
#include "representation/representation_manager.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include <exception>
#include <string>
#include <util/math.hpp>

namespace VTX::Action::Setting
{
	void RestoreLayout::execute()
	{
		// Delete previous save file in case of corruption
		VTXApp::get().getMainWindow().deleteLayoutSaveFile();
		VTXApp::get().getMainWindow().restoreDefaultLayout();

		VTX_INFO( "Default layout restored " );
	}

	void Load::execute()
	{
		const Util::FilePath & path = Util::Filesystem::getSettingJsonFile();
		if ( path.exists() == false )
		{
			VTX_INFO( "No settings file found" );
			return;
		}

		IO::Reader::SerializedObject<VTX::Setting> reader = IO::Reader::SerializedObject<VTX::Setting>();
		try
		{
			reader.readFile( path, VTX_SETTING() );
			VTX_INFO( "Settings loaded " );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Cannot load settings: " + std::string( p_e.what() ) );
		}
	}

	void Save::execute()
	{
		IO::Writer::SerializedObject<VTX::Setting> writer = IO::Writer::SerializedObject<VTX::Setting>();
		try
		{
			writer.writeFile( Util::Filesystem::getSettingJsonFile(), VTX_SETTING() );
			VTX_INFO( "Settings saved " );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Cannot save settings: " + std::string( p_e.what() ) );
		}
	}

	void WindowMode::execute()
	{
		VTX_SETTING().setWindowFullscreen( _windowMode == UI::WindowMode::Fullscreen );
		VTXApp::get().getMainWindow().setWindowMode( _windowMode );
	}

	void ActiveRenderer::execute() { VTX_SETTING().setActivateRenderer( _active ); }

	void ForceRenderer::execute() { VTX_SETTING().setForceRenderer( _force ); }

	void ChangeBackgroundColor::execute()
	{
		VTX_RENDER_EFFECT().setBackgroundColor( _color );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ChangeSnapshotFormat::execute() { VTX_SETTING().setSnapshotFormat( _format ); };

	void ChangeBackgroundOpacity::execute()
	{
		VTX_SETTING().setSnapshotBackgroundOpacity( _opacity );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ChangeSnapshotQuality::execute()
	{
		VTX_SETTING().setSnapshotQuality( _quality );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ChangeSnapshotResolution::execute() { VTX_SETTING().setSnapshotResolution( _resolution ); };

	void ChangeDefaultRepresentation::execute()
	{
		const VTX::Model::Representation::Representation * const previousDefaultRepresentation
			= VTX::Model::Representation::RepresentationLibrary::get().getDefaultRepresentation();

		VTX::Model::Representation::RepresentationLibrary::get().setDefaultRepresentation( _representationIndex );

		VTX::Model::Representation::Representation * const newDefaultRepresentation
			= VTX::Model::Representation::RepresentationLibrary::get().getDefaultRepresentation();

		for ( Model::Representation::InstantiatedRepresentation * const instantiatedRepresentation :
			  Representation::RepresentationManager::get().getAllInstantiatedRepresentations(
				  previousDefaultRepresentation ) )
		{
			if ( instantiatedRepresentation->getOverridedMembersFlag() == Model::Representation::MEMBER_FLAG::NONE )
			{
				Representation::RepresentationManager::get().instantiateRepresentation(
					newDefaultRepresentation, *instantiatedRepresentation->getTarget() );
			}
		}

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};

	void ChangeDefaultRenderEffectPreset::execute()
	{
		VTX_SETTING().setDefaultRenderEffectPresetIndex( Util::Math::clamp(
			_renderEffectPresetIndex, 0, VTX::Model::Renderer::RenderEffectPresetLibrary::get().getPresetCount() ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};

	void ChangeColorMode::execute()
	{
		for ( Model::Representation::Representation * const representation :
			  Model::Representation::RepresentationLibrary::get().getRepresentations() )
		{
			for ( Model::Representation::InstantiatedRepresentation * const instantiatedRepresentation :
				  Representation::RepresentationManager::get().getAllInstantiatedRepresentations( representation ) )
			{
				instantiatedRepresentation->setColorMode( _mode );
			}
		}

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};

	void ChangeShading::execute()
	{
		VTX_RENDER_EFFECT().setShading( _shading );
		VTXApp::get().getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::SHADING );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};

	void ActiveVerticalSync::execute()
	{
		VTX_SETTING().setVSync( _active );
		// TODO: find a way to update VSync at runtime.
	};

	void ActiveAO::execute()
	{
		VTX_RENDER_EFFECT().enableSSAO( _active );
		VTXApp::get().getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::SSAO );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};

	void ChangeAOIntensity::execute()
	{
		VTX_RENDER_EFFECT().setSSAOIntensity( _intensity );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ChangeAOBlurSize::execute()
	{
		VTX_RENDER_EFFECT().setSSAOBlurSize( _blurSize );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ActiveOutline::execute()
	{
		VTX_RENDER_EFFECT().enableOutline( _active );
		VTXApp::get().getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::OUTLINE );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};

	void ChangeOutlineColor::execute()
	{
		VTX_RENDER_EFFECT().setOutlineColor( _color );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ChangeOutlineThickness::execute()
	{
		VTX_RENDER_EFFECT().setOutlineThickness( _thickness );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ChangeOutlineSensivity::execute()
	{
		VTX_RENDER_EFFECT().setOutlineSensivity( _sensivity );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ActiveFog::execute()
	{
		VTX_RENDER_EFFECT().enableFog( _active );
		VTXApp::get().getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::FOG );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};

	void ChangeFogNear::execute()
	{
		VTX_RENDER_EFFECT().setFogNear( _near );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ChangeFogFar::execute()
	{
		VTX_RENDER_EFFECT().setFogFar( _far );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ChangeFogDensity::execute()
	{
		VTX_RENDER_EFFECT().setFogDensity( _density );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ChangeFogColor::execute()
	{
		VTX_RENDER_EFFECT().setFogColor( _color );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ActiveAA::execute()
	{
		VTX_RENDER_EFFECT().setAA( _active );
		VTXApp::get().getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::AA );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};

	void ChangeLightColor::execute()
	{
		VTX_RENDER_EFFECT().setCameraLightColor( _color );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	};

	void ChangeCameraClip::execute()
	{
		VTX_RENDER_EFFECT().setCameraNearClip( Util::Math::min( _near, _far ) );
		VTX_RENDER_EFFECT().setCameraFarClip( Util::Math::max( _near, _far ) );

		VTXApp::get().getScene().getCamera().setNear( VTX_RENDER_EFFECT().getCameraNearClip() );
		VTXApp::get().getScene().getCamera().setFar( VTX_RENDER_EFFECT().getCameraFarClip() );
	};

	void ChangeCameraFov::execute()
	{
		VTX_RENDER_EFFECT().setCameraFOV( _fov );
		VTXApp::get().getScene().getCamera().setFov( VTX_RENDER_EFFECT().getCameraFOV() );
	};

	void ChangeCameraProjection::execute()
	{
		VTX_RENDER_EFFECT().setPerspectiveProjection( _perspective );
		VTXApp::get().getScene().getCamera().setPerspective( VTX_RENDER_EFFECT().isPerspectiveProjection() );
	};

	void ChangeTranslationSpeed::execute() { VTX_SETTING().setTranslationSpeed( _speed ); };

	void ChangeAccelerationFactorSpeed::execute() { VTX_SETTING().setAccelerationSpeedFactor( _factor ); };

	void ChangeDecelerationFactorSpeed::execute() { VTX_SETTING().setDecelerationSpeedFactor( _factor ); };

	void ChangeRotationSpeed::execute() { VTX_SETTING().setRotationSpeed( _speed ); };

	void ActiveYAxisInversion::execute() { VTX_SETTING().setYAxisInverted( _active ); };

	void ActiveControllerElasticity::execute() { VTX_SETTING().setControllerElasticityActive( _active ); };

	void ChangeControllerElasticity::execute() { VTX_SETTING().setControllerElasticityFactor( _elasticity ); };

	void ChangeDefaultTrajectorySpeed::execute() { VTX_SETTING().setDefaultTrajectorySpeed( _speed ); };

	void ChangeDefaultTrajectoryPlayMode::execute() { VTX_SETTING().setDefaultTrajectoryPlayMode( _playMode ); };

	void ChangeSymbolDisplayMode::execute() { VTX_SETTING().setSymbolDisplayMode( _displayMode ); };

	void ChangeCheckVTXUpdateAtLaunch::execute()
	{
		VTX_SETTING().setCheckVTXUpdateAtLaunch( _checkVTXUpdateAtLaunch );
	};

	void ActivatePortableSave::execute() { VTX_SETTING().activatePortableSave( _activate ); };

	void ChangeRenderMode::execute()
	{
		VTX_SETTING().mode = _mode;
		// TODO
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	};

	void ChangeSelectionGranularity::execute() { VTX_SETTING().setSelectionGranularity( _granularity ); };

	void ChangeDefaultRepresentationPerCategory::execute()
	{
		VTX_SETTING().setDefaultRepresentationIndexPerCategory( _categoryEnum, _representationIndex );
	};

	void ApplyAllSettings::execute()
	{
		VTX_ACTION( new Action::Setting::ChangeSymbolDisplayMode( _setting.getSymbolDisplayMode() ) );
		VTX_ACTION( new Action::Setting::ChangeCheckVTXUpdateAtLaunch( _setting.getCheckVTXUpdateAtLaunch() ) );
		VTX_ACTION( new Action::Setting::ActivatePortableSave( _setting.isPortableSaveActivated() ) );

		VTX_ACTION( new Action::Setting::WindowMode( _setting.getWindowFullscreen() ? VTX::UI::WindowMode::Fullscreen
																					: VTX::UI::WindowMode::Windowed ) );

		VTX_ACTION( new Action::Setting::ActiveRenderer( _setting.getActivateRenderer() ) );
		VTX_ACTION( new Action::Setting::ForceRenderer( _setting.getForceRenderer() ) );
		VTX_ACTION( new Action::Setting::ChangeDefaultRepresentation( _setting.getDefaultRepresentationIndex() ) );
		VTX_ACTION(
			new Action::Setting::ChangeDefaultRenderEffectPreset( _setting.getDefaultRenderEffectPresetIndex() ) );

		VTX_ACTION( new Action::Setting::ActiveVerticalSync( _setting.getVSync() ) );

		VTX_ACTION( new Action::Setting::ChangeSnapshotFormat( _setting.getSnapshotFormat() ) );
		VTX_ACTION( new Action::Setting::ChangeBackgroundOpacity( _setting.getSnapshotBackgroundOpacity() ) );
		VTX_ACTION( new Action::Setting::ChangeSnapshotQuality( _setting.getSnapshotQuality() ) );
		VTX_ACTION( new Action::Setting::ChangeSnapshotResolution( _setting.getSnapshotResolution() ) );

		VTX_ACTION( new Action::Setting::ChangeTranslationSpeed( _setting.getTranslationSpeed() ) );
		VTX_ACTION( new Action::Setting::ChangeAccelerationFactorSpeed( _setting.getAccelerationSpeedFactor() ) );
		VTX_ACTION( new Action::Setting::ChangeDecelerationFactorSpeed( _setting.getDecelerationSpeedFactor() ) );
		VTX_ACTION( new Action::Setting::ChangeRotationSpeed( _setting.getRotationSpeed() ) );
		VTX_ACTION( new Action::Setting::ActiveYAxisInversion( _setting.getYAxisInverted() ) );

		VTX_ACTION( new Action::Setting::ActiveControllerElasticity( _setting.getControllerElasticityActive() ) );
		VTX_ACTION( new Action::Setting::ChangeControllerElasticity( _setting.getControllerElasticityFactor() ) );

		VTX_ACTION( new Action::Setting::ChangeDefaultTrajectorySpeed( _setting.getDefaultTrajectorySpeed() ) );
		VTX_ACTION( new Action::Setting::ChangeDefaultTrajectoryPlayMode( _setting.getDefaultTrajectoryPlayMode() ) );
	}

	void ReloadSetting::execute()
	{
		VTX_ACTION( new Action::Setting::Load() );
		VTX_ACTION( new Action::Setting::ApplyAllSettings( VTX_SETTING() ) );
	}

	void RestoreSetting::execute()
	{
		VTX_SETTING().restore();
		VTX_ACTION( new Action::Setting::ApplyAllSettings( VTX_SETTING() ) );
	}

	void RestoreDefaultRepresentationPerCategory::execute()
	{
		VTX_SETTING().restoreDefaultRepresentationPerCategory();
	};
} // namespace VTX::Action::Setting
