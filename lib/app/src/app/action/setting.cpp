#include "app/action/setting.hpp"
#include "app/application/render_effect/render_effect_library.hpp"
#include "app/application/render_effect/render_effect_preset.hpp"
#include "app/application/representation/representation_library.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include "app/core/io/reader/serialized_object.hpp"
#include "app/core/io/writer/serialized_object.hpp"
#include "app/internal/io/filesystem.hpp"
#include "app/internal/scene/camera_manager.hpp"
#include "app/manager/action_manager.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/render/renderer/base_renderer.hpp"
#include <exception>
#include <string>
#include <util/math.hpp>

namespace VTX::App::Action::Setting
{
	void Load::execute()
	{
		const FilePath & path = App::Internal::IO::Filesystem::getSettingJsonFile();
		if ( std::filesystem::exists( path ) == false )
		{
			VTX_INFO( "No settings file found" );
			return;
		}

		Core::IO::Reader::SerializedObject<VTX::App::Application::Setting> reader
			= Core::IO::Reader::SerializedObject<VTX::App::Application::Setting>();
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
		Core::IO::Writer::SerializedObject<VTX::App::Application::Setting> writer
			= Core::IO::Writer::SerializedObject<VTX::App::Application::Setting>();
		try
		{
			writer.writeFile( App::Internal::IO::Filesystem::getSettingJsonFile(), VTX_SETTING() );
			VTX_INFO( "Settings saved " );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Cannot save settings: " + std::string( p_e.what() ) );
		}
	}

	void ActiveRenderer::execute() { VTX_SETTING().setActivateRenderer( _active ); }
	void ForceRenderer::execute() { VTX_SETTING().setForceRenderer( _force ); }

	void ChangeBackgroundColor::execute()
	{
		VTX_RENDER_EFFECT().setBackgroundColor( _color );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeSnapshotFormat::execute() { VTX_SETTING().setSnapshotFormat( _format ); }

	void ChangeBackgroundOpacity::execute()
	{
		VTX_SETTING().setSnapshotBackgroundOpacity( _opacity );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeSnapshotQuality::execute()
	{
		VTX_SETTING().setSnapshotQuality( _quality );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeSnapshotResolution::execute() { VTX_SETTING().setSnapshotResolution( _resolution ); }

	void ChangeDefaultRepresentation::execute()
	{
		const VTX::App::Application::Representation::RepresentationPreset * const previousDefaultRepresentation
			= VTX::App::Application::Representation::RepresentationLibrary::get().getDefaultRepresentation();

		VTX::App::Application::Representation::RepresentationLibrary::get().setDefaultRepresentation(
			_representationIndex );

		VTX::App::Application::Representation::RepresentationPreset * const newDefaultRepresentation
			= VTX::App::Application::Representation::RepresentationLibrary::get().getDefaultRepresentation();

		if ( previousDefaultRepresentation != newDefaultRepresentation )
		{
			for ( App::Application::Representation::InstantiatedRepresentation * const instantiatedRepresentation :
				  App::Application::Representation::RepresentationManager::get().getAllInstantiatedRepresentations(
					  previousDefaultRepresentation ) )
			{
				if ( instantiatedRepresentation->getOverridedMembersFlag()
					 == Application::Representation::MEMBER_FLAG::ENUM::NONE )
				{
					App::Application::Representation::RepresentationManager::get().instantiateRepresentation(
						newDefaultRepresentation, *instantiatedRepresentation->getTarget() );
				}
			}
		}

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void ChangeDefaultRenderEffectPreset::execute()
	{
		const int clampedIndex
			= Util::Math::clamp( _renderEffectPresetIndex,
								 0,
								 VTX::App::Application::RenderEffect::RenderEffectLibrary::get().getPresetCount() );

		VTX_SETTING().setDefaultRenderEffectPresetIndex( clampedIndex );

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void ChangeColorMode::execute()
	{
		for ( App::Application::Representation::RepresentationPreset * const representation :
			  App::Application::Representation::RepresentationLibrary::get().getRepresentations() )
		{
			for ( App::Application::Representation::InstantiatedRepresentation * const instantiatedRepresentation :
				  App::Application::Representation::RepresentationManager::get().getAllInstantiatedRepresentations(
					  representation ) )
			{
				instantiatedRepresentation->setColorMode( _mode );
			}
		}

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void ChangeShading::execute()
	{
		VTX_RENDER_EFFECT().setShading( _shading );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}
	void ActiveVerticalSync::execute()
	{
		VTX_SETTING().setVSync( _active );
		// TODO: find a way to update VSync at runtime.
	}

	void ActiveAO::execute()
	{
		VTX_RENDER_EFFECT().enableSSAO( _active );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void ChangeAOIntensity::execute()
	{
		VTX_RENDER_EFFECT().setSSAOIntensity( _intensity );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeAOBlurSize::execute()
	{
		VTX_RENDER_EFFECT().setSSAOBlurSize( _blurSize );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ActiveOutline::execute()
	{
		VTX_RENDER_EFFECT().enableOutline( _active );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void ChangeOutlineColor::execute()
	{
		VTX_RENDER_EFFECT().setOutlineColor( _color );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeOutlineThickness::execute()
	{
		VTX_RENDER_EFFECT().setOutlineThickness( _thickness );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeOutlineSensivity::execute()
	{
		VTX_RENDER_EFFECT().setOutlineSensivity( _sensivity );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ActiveFog::execute()
	{
		VTX_RENDER_EFFECT().enableFog( _active );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void ChangeFogNear::execute()
	{
		VTX_RENDER_EFFECT().setFogNear( _near );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeFogFar::execute()
	{
		VTX_RENDER_EFFECT().setFogFar( _far );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeFogDensity::execute()
	{
		VTX_RENDER_EFFECT().setFogDensity( _density );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeFogColor::execute()
	{
		VTX_RENDER_EFFECT().setFogColor( _color );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeLightColor::execute()
	{
		VTX_RENDER_EFFECT().setCameraLightColor( _color );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCameraClip::execute()
	{
		VTX_SETTING().setCameraNearClip( Util::Math::min( _near, _far ) );
		VTX_SETTING().setCameraFarClip( Util::Math::max( _near, _far ) );

		VTXApp::get().getScene().getCamera().setNear( VTX_SETTING().getCameraNearClip() );
		VTXApp::get().getScene().getCamera().setFar( VTX_SETTING().getCameraFarClip() );
	}

	void ChangeCameraFov::execute()
	{
		VTX_SETTING().setCameraFOV( _fov );
		VTXApp::get().getScene().getCamera().setFov( _fov );

		VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
	}

	void ChangeCameraProjectionToPerspective::execute()
	{
		VTX_SETTING().setCameraPerspectiveProjection( _perspective );
		VTXApp::get().getScene().getCameraManager().refresh();

		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ActiveAA::execute()
	{
		VTX_SETTING().setAA( _active );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void ChangeTranslationSpeed::execute() { VTX_SETTING().setTranslationSpeed( _speed ); }

	void ChangeAccelerationFactorSpeed::execute() { VTX_SETTING().setAccelerationSpeedFactor( _factor ); }

	void ChangeDecelerationFactorSpeed::execute() { VTX_SETTING().setDecelerationSpeedFactor( _factor ); }

	void ChangeRotationSpeed::execute() { VTX_SETTING().setRotationSpeed( _speed ); }

	void ActiveYAxisInversion::execute() { VTX_SETTING().setYAxisInverted( _active ); }

	void ActiveControllerElasticity::execute() { VTX_SETTING().setControllerElasticityActive( _active ); }

	void ChangeControllerElasticity::execute() { VTX_SETTING().setControllerElasticityFactor( _elasticity ); }

	void ChangeDefaultTrajectorySpeed::execute() { VTX_SETTING().setDefaultTrajectorySpeed( _speed ); }

	void ChangeDefaultTrajectoryPlayMode::execute() { VTX_SETTING().setDefaultTrajectoryPlayMode( _playMode ); }

	void ChangeSymbolDisplayMode::execute() { VTX_SETTING().setSymbolDisplayMode( _displayMode ); }

	void ChangeCheckVTXUpdateAtLaunch::execute() { VTX_SETTING().setCheckVTXUpdateAtLaunch( _checkVTXUpdateAtLaunch ); }

	void ActivatePortableSave::execute() { VTX_SETTING().activatePortableSave( _activate ); }

	void ChangeRenderMode::execute()
	{
		VTX_SETTING().mode = _mode;
		// TODO
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void ChangeSelectionGranularity::execute() { VTX_SETTING().setSelectionGranularity( _granularity ); }

	void ChangeDefaultRepresentationPerCategory::execute()
	{
		VTX_SETTING().setDefaultRepresentationIndexPerCategory( _categoryEnum, _representationIndex );
	}

	void ApplyAllSettings::execute()
	{
		VTX_ACTION<ChangeSymbolDisplayMode>( _setting.getSymbolDisplayMode() );
		VTX_ACTION<ChangeCheckVTXUpdateAtLaunch>( _setting.getCheckVTXUpdateAtLaunch() );
		VTX_ACTION<ActivatePortableSave>( _setting.isPortableSaveActivated() );

		VTX_ACTION<ActiveRenderer>( _setting.getActivateRenderer() );
		VTX_ACTION<ForceRenderer>( _setting.getForceRenderer() );

		// Active AA before changing representation effet preset to prevent assert call
		VTX_ACTION<ActiveAA>( _setting.getAA() );

		// VTX_ACTION( new App::Action::Setting::ChangeDefaultRepresentation( _setting.getDefaultRepresentationIndex() )
		// ); VTX_ACTION( 	new App::Action::Setting::ChangeDefaultRenderEffectPreset(
		//_setting.getDefaultRenderEffectPresetIndex() ) );

		VTX_ACTION<ActiveVerticalSync>( _setting.getVSync() );

		VTX_ACTION<ChangeSnapshotFormat>( _setting.getSnapshotFormat() );
		VTX_ACTION<ChangeBackgroundOpacity>( _setting.getSnapshotBackgroundOpacity() );
		VTX_ACTION<ChangeSnapshotQuality>( _setting.getSnapshotQuality() );
		VTX_ACTION<ChangeSnapshotResolution>( _setting.getSnapshotResolution() );

		VTX_ACTION<ChangeCameraFov>( _setting.getCameraFOV() );
		VTX_ACTION<ChangeCameraClip>( _setting.getCameraNearClip(), _setting.getCameraFarClip() );
		VTX_ACTION<ChangeCameraProjectionToPerspective>( _setting.getCameraPerspective() );

		VTX_ACTION<ChangeTranslationSpeed>( _setting.getTranslationSpeed() );
		VTX_ACTION<ChangeAccelerationFactorSpeed>( _setting.getAccelerationSpeedFactor() );
		VTX_ACTION<ChangeDecelerationFactorSpeed>( _setting.getDecelerationSpeedFactor() );
		VTX_ACTION<ChangeRotationSpeed>( _setting.getRotationSpeed() );
		VTX_ACTION<ActiveYAxisInversion>( _setting.getYAxisInverted() );

		VTX_ACTION<ActiveControllerElasticity>( _setting.getControllerElasticityActive() );
		VTX_ACTION<ChangeControllerElasticity>( _setting.getControllerElasticityFactor() );

		VTX_ACTION<ChangeDefaultTrajectorySpeed>( _setting.getDefaultTrajectorySpeed() );
		VTX_ACTION<ChangeDefaultTrajectoryPlayMode>( _setting.getDefaultTrajectoryPlayMode() );
	}

	void ReloadSetting::execute()
	{
		VTX_ACTION<Load>();
		VTX_ACTION<ApplyAllSettings>( VTX_SETTING() );
	}

	void RestoreSetting::execute()
	{
		VTX_SETTING().restore();
		VTX_ACTION<ApplyAllSettings>( VTX_SETTING() );
	}
	void RestoreDefaultRepresentationPerCategory::execute() { VTX_SETTING().restoreDefaultRepresentationPerCategory(); }

} // namespace VTX::App::Action::Setting
