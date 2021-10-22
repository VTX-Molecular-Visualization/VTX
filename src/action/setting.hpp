#ifndef __VTX_ACTION_SETTING__
#define __VTX_ACTION_SETTING__

#include "base_action.hpp"
#include "io/reader/serialized_object.hpp"
#include "io/writer/serialized_object.hpp"
#include "model/molecule.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/representation/representation_library.hpp"
#include "object3d/scene.hpp"
#include "renderer/base_renderer.hpp"
#include "renderer/gl/gl.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "trajectory/trajectory_enum.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include "worker/snapshoter.hpp"
#include <QWindow>

namespace VTX::Action::Setting
{
	class RestoreLayout : public BaseAction
	{
	  public:
		explicit RestoreLayout() {}
		virtual void execute() override
		{
			// Delete previous save file in case of corruption
			VTXApp::get().getMainWindow().deleteLayoutSaveFile();
			VTXApp::get().getMainWindow().restoreDefaultLayout();

			VTX_INFO( "Default layout restored " );
		}
	};

	class Load : public BaseAction
	{
	  public:
		explicit Load() {}
		virtual void execute() override
		{
			const IO::FilePath & path = Util::Filesystem::getSettingJsonFile();
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
	};

	class Save : public BaseAction
	{
	  public:
		explicit Save() {}
		virtual void execute() override
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
	};

	class WindowMode : public BaseAction
	{
	  public:
		explicit WindowMode( const UI::WindowMode & p_windowMode ) : _windowMode( p_windowMode ) {}
		void execute() override
		{
			VTX_SETTING().setWindowFullscreen( _windowMode == UI::WindowMode::Fullscreen );
			VTXApp::get().getMainWindow().setWindowMode( _windowMode );
		}

	  private:
		const UI::WindowMode _windowMode;
	};

	class ActiveRenderer : public BaseAction
	{
	  public:
		explicit ActiveRenderer( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override { VTX_SETTING().setActivateRenderer( _active ); }

	  private:
		const bool _active;
	};

	class ForceRenderer : public BaseAction
	{
	  public:
		explicit ForceRenderer( const bool p_force ) : _force( p_force ) {}

		virtual void execute() override { VTX_SETTING().setForceRenderer( _force ); }

	  private:
		const bool _force;
	};

	class ChangeBackgroundColor : public BaseAction
	{
	  public:
		explicit ChangeBackgroundColor( const Color::Rgb & p_color ) : _color( p_color ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setBackgroundColor( _color );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb _color;
	};

	class ChangeBackgroundOpacity : public BaseAction
	{
	  public:
		explicit ChangeBackgroundOpacity( const float p_opacity ) : _opacity( p_opacity ) {}

		virtual void execute() override
		{
			VTX_SETTING().setSnapshotBackgroundOpacity( _opacity );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float _opacity;
	};

	class ChangeSnapshotResolution : public BaseAction
	{
	  public:
		explicit ChangeSnapshotResolution( const Worker::SNAPSHOT_RESOLUTION & p_resolution ) :
			_resolution( p_resolution )
		{
		}

		virtual void execute() override { VTX_SETTING().setSnapshotResolution( _resolution ); };

	  private:
		const Worker::SNAPSHOT_RESOLUTION _resolution;
	};

	class ChangeDefaultRepresentation : public BaseAction
	{
	  public:
		explicit ChangeDefaultRepresentation( const int p_representationIndex ) :
			_representationIndex( p_representationIndex )
		{
		}

		virtual void execute() override
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

		virtual void displayUsage() override { VTX_INFO( "BALL_AND_STICK|VAN_DER_WAALS|STICK|SAS" ); }

	  private:
		const int _representationIndex;
	};

	class ChangeDefaultRenderEffectPreset : public BaseAction
	{
	  public:
		explicit ChangeDefaultRenderEffectPreset( const int p_renderEffectPresetIndex ) :
			_renderEffectPresetIndex( p_renderEffectPresetIndex )
		{
		}

		virtual void execute() override
		{
			VTX_SETTING().setDefaultRenderEffectPresetIndex(
				Util::Math::clamp( _renderEffectPresetIndex,
								   0,
								   VTX::Model::Renderer::RenderEffectPresetLibrary::get().getPresetCount() ) );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

		virtual void displayUsage() override { VTX_INFO( "BALL_AND_STICK|VAN_DER_WAALS|STICK|SAS" ); }

	  private:
		const int _renderEffectPresetIndex;
	};

	class ChangeColorMode : public BaseAction
	{
	  public:
		explicit ChangeColorMode( const Generic::COLOR_MODE p_mode ) : _mode( p_mode ) {}

		virtual void execute() override
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

		virtual void displayUsage() override { VTX_INFO( "ATOM|RESIDUE|CHAIN|PROTEIN" ); }

	  private:
		const Generic::COLOR_MODE _mode;
	};

	class ChangeShading : public BaseAction
	{
	  public:
		ChangeShading( const VTX::Renderer::SHADING p_shading ) : _shading( p_shading ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setShading( _shading );
			VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL().setShading();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

		virtual void displayUsage() override { VTX_INFO( "DIFFUSE|GLOSSY|TOON|FLAT_COLOR" ); }

	  private:
		const VTX::Renderer::SHADING _shading;
	};

	class ActiveVerticalSync : public BaseAction
	{
	  public:
		explicit ActiveVerticalSync( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override
		{
			VTX_SETTING().setVSync( _active );
			VTXApp::get().getMainWindow().getOpenGLWidget().activeVSync( _active );
		};

	  private:
		const bool _active;
	};

	class ActiveAO : public BaseAction
	{
	  public:
		explicit ActiveAO( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().enableSSAO( _active );
			VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL().activeSSAO( _active );

			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const bool _active;
	};

	class ChangeAOIntensity : public BaseAction
	{
	  public:
		explicit ChangeAOIntensity( const int p_intensity ) : _intensity( p_intensity ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setSSAOIntensity( _intensity );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const int _intensity;
	};

	class ChangeAOBlurSize : public BaseAction
	{
	  public:
		explicit ChangeAOBlurSize( const int p_blurSize ) : _blurSize( p_blurSize ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setSSAOBlurSize( _blurSize );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const int _blurSize;
	};

	class ActiveOutline : public BaseAction
	{
	  public:
		explicit ActiveOutline( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().enableOutline( _active );
			VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL().activeOutline( _active );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const bool _active;
	};

	class ChangeOutlineColor : public BaseAction
	{
	  public:
		explicit ChangeOutlineColor( const Color::Rgb & p_color ) : _color( p_color ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setOutlineColor( _color );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb _color;
	};

	class ChangeOutlineThickness : public BaseAction
	{
	  public:
		explicit ChangeOutlineThickness( const uint p_thickness ) : _thickness( p_thickness ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setOutlineThickness( _thickness );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const uint _thickness;
	};

	class ChangeOutlineSensivity : public BaseAction
	{
	  public:
		explicit ChangeOutlineSensivity( const float p_sensivity ) : _sensivity( p_sensivity ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setOutlineSensivity( _sensivity );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float _sensivity;
	};

	class ActiveFog : public BaseAction
	{
	  public:
		explicit ActiveFog( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().enableFog( _active );
			VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL().activeFog( _active );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const bool _active;
	};

	class ChangeFogNear : public BaseAction
	{
	  public:
		explicit ChangeFogNear( const float p_near ) : _near( p_near ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setFogNear( _near );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float _near;
	};

	class ChangeFogFar : public BaseAction
	{
	  public:
		explicit ChangeFogFar( const float p_far ) : _far( p_far ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setFogFar( _far );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float _far;
	};

	class ChangeFogDensity : public BaseAction
	{
	  public:
		explicit ChangeFogDensity( const float p_density ) : _density( p_density ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setFogDensity( _density );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float _density;
	};

	class ChangeFogColor : public BaseAction
	{
	  public:
		explicit ChangeFogColor( const Color::Rgb & p_color ) : _color( p_color ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setFogColor( _color );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb _color;
	};

	class ActiveAA : public BaseAction
	{
	  public:
		explicit ActiveAA( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setAA( _active );
			VTXApp::get().getMainWindow().getOpenGLWidget().getRendererGL().activeAA( _active );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const bool _active;
	};

	class ChangeLightColor : public BaseAction
	{
	  public:
		explicit ChangeLightColor( const Color::Rgb & p_color ) : _color( p_color ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setCameraLightColor( _color );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb _color;
	};

	class ChangeCameraClip : public BaseAction
	{
	  public:
		explicit ChangeCameraClip( const float p_near, const float p_far ) : _near( p_near ), _far( p_far ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setCameraNearClip( Util::Math::min( _near, _far ) );
			VTX_RENDER_EFFECT().setCameraFarClip( Util::Math::max( _near, _far ) );

			VTXApp::get().getScene().getCamera().setNear( VTX_RENDER_EFFECT().getCameraNearClip() );
			VTXApp::get().getScene().getCamera().setFar( VTX_RENDER_EFFECT().getCameraFarClip() );
		};

	  private:
		const float _near;
		const float _far;
	};

	class ChangeCameraFov : public BaseAction
	{
	  public:
		explicit ChangeCameraFov( const float p_fov ) : _fov( p_fov ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setCameraFOV( _fov );
			VTXApp::get().getScene().getCamera().setFov( VTX_RENDER_EFFECT().getCameraFOV() );
		};

	  private:
		const float _fov;
	};

	class ChangeCameraProjection : public BaseAction
	{
	  public:
		explicit ChangeCameraProjection( const bool p_perspective ) : _perspective( p_perspective ) {}

		virtual void execute() override
		{
			VTX_RENDER_EFFECT().setPerspectiveProjection( _perspective );
			VTXApp::get().getScene().getCamera().setPerspective( VTX_RENDER_EFFECT().isPerspectiveProjection() );
		};

	  private:
		const bool _perspective;
	};

	class ChangeTranslationSpeed : public BaseAction
	{
	  public:
		explicit ChangeTranslationSpeed( const float p_speed ) : _speed( p_speed ) {}

		virtual void execute() override { VTX_SETTING().setTranslationSpeed( _speed ); };

	  private:
		const float _speed;
	};

	class ChangeTranslationFactorSpeed : public BaseAction
	{
	  public:
		explicit ChangeTranslationFactorSpeed( const float p_factor ) : _factor( p_factor ) {}

		virtual void execute() override { VTX_SETTING().setTranslationSpeedFactor( _factor ); };

	  private:
		const float _factor;
	};

	class ChangeRotationSpeed : public BaseAction
	{
	  public:
		explicit ChangeRotationSpeed( const float p_speed ) : _speed( p_speed ) {}

		virtual void execute() override { VTX_SETTING().setRotationSpeed( _speed ); };

	  private:
		const float _speed;
	};

	class ActiveYAxisInversion : public BaseAction
	{
	  public:
		explicit ActiveYAxisInversion( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override { VTX_SETTING().setYAxisInverted( _active ); };

	  private:
		const bool _active;
	};

	class ActiveControllerElasticity : public BaseAction
	{
	  public:
		explicit ActiveControllerElasticity( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override { VTX_SETTING().setControllerElasticityActive( _active ); };

	  private:
		const bool _active;
	};

	class ChangeControllerElasticity : public BaseAction
	{
	  public:
		explicit ChangeControllerElasticity( const float p_elasticity ) : _elasticity( p_elasticity ) {}

		virtual void execute() override { VTX_SETTING().setControllerElasticityFactor( _elasticity ); };

	  private:
		const float _elasticity;
	};

	class ChangeDefaultTrajectorySpeed : public BaseAction
	{
	  public:
		explicit ChangeDefaultTrajectorySpeed( const int p_speed ) : _speed( p_speed ) {}

		virtual void execute() override { VTX_SETTING().setDefaultTrajectorySpeed( _speed ); };

	  private:
		const int _speed;
	};

	class ChangeDefaultTrajectoryPlayMode : public BaseAction
	{
	  public:
		explicit ChangeDefaultTrajectoryPlayMode( const Trajectory::PlayMode p_playMode ) : _playMode( p_playMode ) {}

		virtual void execute() override { VTX_SETTING().setDefaultTrajectoryPlayMode( _playMode ); };

	  private:
		const Trajectory::PlayMode _playMode;
	};

	class ChangeSymbolDisplayMode : public BaseAction
	{
	  public:
		explicit ChangeSymbolDisplayMode( const Style::SYMBOL_DISPLAY_MODE & p_displayMode ) :
			_displayMode( p_displayMode )
		{
		}

		virtual void execute() override { VTX_SETTING().setSymbolDisplayMode( _displayMode ); };

	  private:
		const Style::SYMBOL_DISPLAY_MODE _displayMode;
	};

	class ChangeCheckVTXUpdateAtLaunch : public BaseAction
	{
	  public:
		explicit ChangeCheckVTXUpdateAtLaunch( const bool p_checkVtxUpdateAtLaunch ) :
			_checkVTXUpdateAtLaunch( p_checkVtxUpdateAtLaunch )
		{
		}

		virtual void execute() override { VTX_SETTING().setCheckVTXUpdateAtLaunch( _checkVTXUpdateAtLaunch ); };

	  private:
		const bool _checkVTXUpdateAtLaunch;
	};

	class ActivatePortableSave : public BaseAction
	{
	  public:
		explicit ActivatePortableSave( const bool p_activate ) : _activate( p_activate ) {}

		virtual void execute() override { VTX_SETTING().activatePortableSave( _activate ); };

	  private:
		const bool _activate;
	};

	class ChangeRenderMode : public BaseAction
	{
	  public:
		ChangeRenderMode( const VTX::Renderer::MODE p_mode ) : _mode( p_mode ) {}

		virtual void execute() override
		{
			VTX_SETTING().mode = _mode;
			VTXApp::get().getMainWindow().getOpenGLWidget().setRenderMode();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const VTX::Renderer::MODE _mode;
	};

	class ApplyAllSettings : public BaseAction
	{
	  public:
		ApplyAllSettings( VTX::Setting & p_setting ) : _setting( p_setting ) {}

		virtual void execute() override
		{
			VTX_ACTION( new Action::Setting::ChangeSymbolDisplayMode( _setting.getSymbolDisplayMode() ) );
			VTX_ACTION( new Action::Setting::ChangeCheckVTXUpdateAtLaunch( _setting.getCheckVTXUpdateAtLaunch() ) );
			VTX_ACTION( new Action::Setting::ActivatePortableSave( _setting.isPortableSaveActivated() ) );

			VTX_ACTION( new Action::Setting::WindowMode(
				_setting.getWindowFullscreen() ? VTX::UI::WindowMode::Fullscreen : VTX::UI::WindowMode::Windowed ) );

			VTX_ACTION( new Action::Setting::ActiveRenderer( _setting.getActivateRenderer() ) );
			VTX_ACTION( new Action::Setting::ForceRenderer( _setting.getForceRenderer() ) );
			VTX_ACTION( new Action::Setting::ChangeDefaultRepresentation( _setting.getDefaultRepresentationIndex() ) );
			VTX_ACTION(
				new Action::Setting::ChangeDefaultRenderEffectPreset( _setting.getDefaultRenderEffectPresetIndex() ) );

			VTX_ACTION( new Action::Setting::ActiveVerticalSync( _setting.getVSync() ) );
			VTX_ACTION( new Action::Setting::ChangeBackgroundOpacity( _setting.getSnapshotBackgroundOpacity() ) );
			VTX_ACTION( new Action::Setting::ChangeSnapshotResolution( _setting.getSnapshotResolution() ) );

			VTX_ACTION( new Action::Setting::ChangeTranslationSpeed( _setting.getTranslationSpeed() ) );
			VTX_ACTION( new Action::Setting::ChangeTranslationFactorSpeed( _setting.getTranslationSpeedFactor() ) );
			VTX_ACTION( new Action::Setting::ChangeRotationSpeed( _setting.getRotationSpeed() ) );
			VTX_ACTION( new Action::Setting::ActiveYAxisInversion( _setting.getYAxisInverted() ) );

			VTX_ACTION( new Action::Setting::ActiveControllerElasticity( _setting.getControllerElasticityActive() ) );
			VTX_ACTION( new Action::Setting::ChangeControllerElasticity( _setting.getControllerElasticityFactor() ) );

			VTX_ACTION( new Action::Setting::ChangeDefaultTrajectorySpeed( _setting.getDefaultTrajectorySpeed() ) );
			VTX_ACTION(
				new Action::Setting::ChangeDefaultTrajectoryPlayMode( _setting.getDefaultTrajectoryPlayMode() ) );
		}

	  private:
		VTX::Setting _setting;
	};

	class ReloadSetting : public BaseAction
	{
	  public:
		ReloadSetting() {}

		virtual void execute() override
		{
			VTX_ACTION( new Action::Setting::Load() );
			VTX_ACTION( new Action::Setting::ApplyAllSettings( VTX_SETTING() ) );
		}
	};

	class RestoreSetting : public BaseAction
	{
	  public:
		RestoreSetting() {}

		virtual void execute() override
		{
			VTX_SETTING().restore();
			VTX_ACTION( new Action::Setting::ApplyAllSettings( VTX_SETTING() ) );
		}
	};
} // namespace VTX::Action::Setting

#endif
