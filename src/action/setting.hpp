#ifndef __VTX_ACTION_SETTING__
#define __VTX_ACTION_SETTING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "io/reader/setting.hpp"
#include "io/writer/setting.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation_library.hpp"
#include "object3d/scene.hpp"
#include "renderer/base_renderer.hpp"
#include "renderer/gl/gl.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
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
			const VTX::FilePath & path = Util::Filesystem::SETTING_JSON_FILE;
			if ( std::filesystem::exists( path ) == false )
			{
				VTX_INFO( "No settings file found" );
				return;
			}

			IO::Reader::Setting reader = IO::Reader::Setting();
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
			IO::Writer::Setting writer = IO::Writer::Setting();
			try
			{
				writer.writeFile( Util::Filesystem::SETTING_JSON_FILE, VTX_SETTING() );
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
		void execute() override { VTXApp::get().getMainWindow().setWindowMode( _windowMode ); }

	  private:
		const UI::WindowMode _windowMode;
	};

	class ChangeDisplayMode : public BaseAction
	{
	  public:
		explicit ChangeDisplayMode( const Style::SYMBOL_DISPLAY_MODE p_mode ) : _mode( p_mode ) {}

		virtual void execute() override { VTX_SETTING().symbolDisplayMode = _mode; };

	  private:
		const Style::SYMBOL_DISPLAY_MODE _mode;
	};

	class ActiveRenderer : public BaseAction
	{
	  public:
		explicit ActiveRenderer( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override { VTX_SETTING().activeRenderer = _active; }

	  private:
		const bool _active;
	};

	class ChangeBackgroundColor : public BaseAction
	{
	  public:
		explicit ChangeBackgroundColor( const Color::Rgb & p_color ) : _color( p_color ) {}

		virtual void execute() override
		{
			VTX_SETTING().backgroundColor = _color;
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb _color;
	};

	class ChangeRepresentation : public BaseAction
	{
	  public:
		explicit ChangeRepresentation( const int p_representationIndex ) : _representationIndex( p_representationIndex )
		{
		}

		virtual void execute() override
		{
			VTX::Model::Representation::RepresentationLibrary::get().setDefaultRepresentation( _representationIndex );

			for ( const Object3D::Scene::PairMoleculePtrFloat & pair : VTXApp::get().getScene().getMolecules() )
				pair.first->computeRepresentationTargets();

			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

		virtual void displayUsage() override { VTX_INFO( "BALL_AND_STICK|VAN_DER_WAALS|STICK|SAS" ); }

	  private:
		const int _representationIndex;
	};

	class ChangeAtomsRadius : public BaseAction
	{
	  public:
		explicit ChangeAtomsRadius( const float p_atomsRadius ) : _atomsRadius( p_atomsRadius ) {}

		virtual void execute() override
		{
			VTX_SETTING().atomsRadius = _atomsRadius;
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		}

	  private:
		const float _atomsRadius;
	};

	class ChangeBondsRadius : public BaseAction
	{
	  public:
		explicit ChangeBondsRadius( const float p_bondsRadius ) : _bondsRadius( p_bondsRadius ) {}

		virtual void execute() override
		{
			VTX_SETTING().bondsRadius = _bondsRadius;
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float _bondsRadius;
	};

	class ChangeColorMode : public BaseAction
	{
	  public:
		explicit ChangeColorMode( const Generic::COLOR_MODE p_mode ) : _mode( p_mode ) {}

		virtual void execute() override
		{
			VTX_SETTING().colorMode = _mode;
			for ( const Object3D::Scene::PairMoleculePtrFloat & pair : VTXApp::get().getScene().getMolecules() )
			{
				pair.first->refreshColors();
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
			VTX_SETTING().shading = _shading;
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
			VTX_SETTING().activeVSync = _active;
			// TODO
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
			VTX_SETTING().activeAO = _active;
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
			VTX_SETTING().aoIntensity
				= Util::Math::clamp( _intensity, VTX::Setting::AO_INTENSITY_MIN, VTX::Setting::AO_INTENSITY_MAX );
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
			VTX_SETTING().aoBlurSize
				= Util::Math::clamp( _blurSize, VTX::Setting::AO_BLUR_SIZE_MIN, VTX::Setting::AO_BLUR_SIZE_MAX );
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
			VTX_SETTING().activeOutline = _active;
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
			VTX_SETTING().outlineColor = _color;
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb _color;
	};

	class ActiveFog : public BaseAction
	{
	  public:
		explicit ActiveFog( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override
		{
			VTX_SETTING().activeFog = _active;
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
			VTX_SETTING().fogNear = Util::Math::min( _near, VTX_SETTING().fogFar );
			VTX_SETTING().fogFar  = Util::Math::max( _near, VTX_SETTING().fogFar );
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
			VTX_SETTING().fogNear = Util::Math::min( VTX_SETTING().fogNear, _far );
			VTX_SETTING().fogFar  = Util::Math::max( VTX_SETTING().fogNear, _far );
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
			VTX_SETTING().fogDensity = Util::Math::clamp( _density, 0.f, 1.f );
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
			VTX_SETTING().fogColor = _color;
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
			VTX_SETTING().activeAA = _active;
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
			VTX_SETTING().lightColor = _color;
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
			VTX_SETTING().cameraNear = Util::Math::min( _near, _far );
			VTX_SETTING().cameraFar	 = Util::Math::max( _near, _far );

			VTXApp::get().getScene().getCamera().setNear( VTX_SETTING().cameraNear );
			VTXApp::get().getScene().getCamera().setFar( VTX_SETTING().cameraFar );
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
			VTX_SETTING().cameraFov = _fov;

			VTXApp::get().getScene().getCamera().setFov( VTX_SETTING().cameraFov );
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
			VTX_SETTING().cameraPerspective = _perspective;
			VTXApp::get().getScene().getCamera().setPerspective( VTX_SETTING().cameraPerspective );
		};

	  private:
		const bool _perspective;
	};

	class ChangeTranslationSpeed : public BaseAction
	{
	  public:
		explicit ChangeTranslationSpeed( const float p_speed ) : _speed( p_speed ) {}

		virtual void execute() override
		{
			VTX_SETTING().translationSpeed = Util::Math::clamp( _speed,
																VTX::Setting::CONTROLLER_TRANSLATION_SPEED_MIN,
																VTX::Setting::CONTROLLER_TRANSLATION_SPEED_MAX );
		};

	  private:
		const float _speed;
	};

	class ChangeTranslationFactorSpeed : public BaseAction
	{
	  public:
		explicit ChangeTranslationFactorSpeed( const float p_factor ) : _factor( p_factor ) {}

		virtual void execute() override
		{
			VTX_SETTING().translationFactorSpeed = Util::Math::clamp( _factor,
																	  VTX::Setting::CONTROLLER_TRANSLATION_FACTOR_MIN,
																	  VTX::Setting::CONTROLLER_TRANSLATION_FACTOR_MAX );
		};

	  private:
		const float _factor;
	};

	class ChangeRotationSpeed : public BaseAction
	{
	  public:
		explicit ChangeRotationSpeed( const float p_speed ) : _speed( p_speed ) {}

		virtual void execute() override
		{
			VTX_SETTING().rotationSpeed = Util::Math::clamp(
				_speed, VTX::Setting::CONTROLLER_ROTATION_SPEED_MIN, VTX::Setting::CONTROLLER_ROTATION_SPEED_MAX );
		};

	  private:
		const float _speed;
	};

	class ActiveYAxisInversion : public BaseAction
	{
	  public:
		explicit ActiveYAxisInversion( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override { VTX_SETTING().yAxisInverted = _active; };

	  private:
		const bool _active;
	};

	class ActiveControllerElasticity : public BaseAction
	{
	  public:
		explicit ActiveControllerElasticity( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override { VTX_SETTING().activeControllerElasticity = _active; };

	  private:
		const bool _active;
	};

	class ChangeControllerElasticity : public BaseAction
	{
	  public:
		explicit ChangeControllerElasticity( const float p_elasticity ) : _elasticity( p_elasticity ) {}

		virtual void execute() override
		{
			VTX_SETTING().controllerElasticityFactor
				= Util::Math::clamp( _elasticity,
									 VTX::Setting::CONTROLLER_ELASTICITY_FACTOR_MIN,
									 VTX::Setting::CONTROLLER_ELASTICITY_FACTOR_MAX );
		};

	  private:
		const float _elasticity;
	};

	class ChangeAutoRotateSpeed : public BaseAction
	{
	  public:
		explicit ChangeAutoRotateSpeed( const Vec3f & p_value ) : _value( p_value ) {}

		virtual void execute() override
		{
			VTX_SETTING().autoRotationSpeed
				= Util::Math::clamp( _value, VTX::Setting::AUTO_ROTATE_SPEED_MIN, VTX::Setting::AUTO_ROTATE_SPEED_MAX );
		}

		virtual void displayUsage() override { VTX_INFO( "f f f|f" ); }

	  private:
		const Vec3f _value;
	};

	class ChangeRenderMode : public BaseAction
	{
	  public:
		ChangeRenderMode( const VTX::Renderer::MODE p_mode ) : _mode( p_mode ) {}

		virtual void execute() override
		{
			VTX_SETTING().mode = _mode;
			VTXApp::get().getMainWindow().getOpenGLWidget().switchRenderer( _mode );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const VTX::Renderer::MODE _mode;
	};
} // namespace VTX::Action::Setting

#endif
