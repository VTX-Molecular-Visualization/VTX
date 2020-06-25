#ifndef __VTX_ACTION_SETTING__
#define __VTX_ACTION_SETTING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "io/reader/setting.hpp"
#include "io/writer/setting.hpp"
#include "util/filesystem.hpp"
#include "util/molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Setting
		{
			class Load : public BaseAction
			{
			  public:
				explicit Load() {}
				virtual void execute() override
				{
					const Path & path = Util::Filesystem::SETTING_JSON_FILE;
					if ( Util::Filesystem::exists( path ) == false )
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

			class ChangeTheme : public BaseAction
			{
			  public:
				explicit ChangeTheme( const Style::THEME p_theme ) : _theme( p_theme ) {}

				virtual void execute() override
				{
					VTX_SETTING().theme = _theme;
					VTXApp::get().getUI().setTheme();
				};

			  private:
				const Style::THEME _theme;
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

			class ChangeRepresentation : public BaseAction
			{
			  public:
				explicit ChangeRepresentation( const Generic::REPRESENTATION p_representation ) :
					_representation( p_representation )
				{
				}

				virtual void execute() override
				{
					VTX_SETTING().representation = _representation;
					for ( const Object3D::Scene::PairMoleculePtrFloat & pair : VTXApp::get().getScene().getMolecules() )
					{
						Util::Molecule::refreshRepresentationState( *pair.first );
					}
				};

				virtual void displayUsage() override { VTX_INFO( "BALL_AND_STICK|VAN_DER_WAALS|STICK|SAS" ); }

			  private:
				const Generic::REPRESENTATION _representation;
			};

			class ChangeAtomsRadius : public BaseAction
			{
			  public:
				explicit ChangeAtomsRadius( const float p_atomsRadius ) :
					_atomsRadius( VTX_SETTING().representation == Generic::REPRESENTATION::BALL_AND_STICK
									  ? Util::Math::max( VTX_SETTING().bondsRadius, p_atomsRadius )
									  : p_atomsRadius )
				{
				}

				virtual void execute() override { VTX_SETTING().atomsRadius = _atomsRadius; }

			  private:
				const float _atomsRadius;
			};

			class ChangeBondsRadius : public BaseAction
			{
			  public:
				explicit ChangeBondsRadius( const float p_bondsRadius ) :
					_bondsRadius( VTX_SETTING().representation == Generic::REPRESENTATION::BALL_AND_STICK
									  ? Util::Math::min( VTX_SETTING().atomsRadius, p_bondsRadius )
									  : p_bondsRadius )
				{
				}

				virtual void execute() override { VTX_SETTING().bondsRadius = _bondsRadius; };

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
						pair.first->setColorMode();
					}
				};

				virtual void displayUsage() override { VTX_INFO( "ATOM|RESIDUE|CHAIN|PROTEIN" ); }

			  private:
				const Generic::COLOR_MODE _mode;
			};

			class ChangeShading : public BaseAction
			{
			  public:
				ChangeShading( const Renderer::SHADING p_shading ) : _shading( p_shading ) {}

				virtual void execute() override
				{
					VTX_SETTING().shading = _shading;
					VTXApp::get().getRendererGL().setShading();
				};

				virtual void displayUsage() override { VTX_INFO( "DIFFUSE|GLOSSY|TOON|FLAT_COLOR" ); }

			  private:
				const Renderer::SHADING _shading;
			};

			class ActiveVerticalSync : public BaseAction
			{
			  public:
				explicit ActiveVerticalSync( const bool p_active ) : _active( p_active ) {}

				virtual void execute() override
				{
					VTX_SETTING().activeVSync = _active;
					VTXApp::get().getUI().setVSync( VTX_SETTING().activeVSync );
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
					VTXApp::get().getRendererGL().activeSSAO( _active );
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
					VTX_SETTING().aoIntensity = Util::Math::clamp(
						_intensity, VTX::Setting::AO_INTENSITY_MIN, VTX::Setting::AO_INTENSITY_MAX );
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
					VTX_SETTING().aoBlurSize = Util::Math::clamp(
						_blurSize, VTX::Setting::AO_BLUR_SIZE_MIN, VTX::Setting::AO_BLUR_SIZE_MAX );
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
					VTXApp::get().getRendererGL().activeOutline( _active );
				};

			  private:
				const bool _active;
			};

			class ChangeOutlineColor : public BaseAction
			{
			  public:
				explicit ChangeOutlineColor( const Color::Rgb & p_color ) : _color( p_color ) {}

				virtual void execute() override { VTX_SETTING().outlineColor = _color; };

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
					VTXApp::get().getRendererGL().activeFog( _active );
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
				};

			  private:
				const float _far;
			};

			class ChangeFogDensity : public BaseAction
			{
			  public:
				explicit ChangeFogDensity( const float p_density ) : _density( p_density ) {}

				virtual void execute() override { VTX_SETTING().fogDensity = Util::Math::clamp( _density, 0.f, 1.f ); };

			  private:
				const float _density;
			};

			class ActiveAA : public BaseAction
			{
			  public:
				explicit ActiveAA( const bool p_active ) : _active( p_active ) {}

				virtual void execute() override
				{
					VTX_SETTING().activeAA = _active;
					VTXApp::get().getRendererGL().activeAA( _active );
				};

			  private:
				const bool _active;
			};

			class ChangeBackgroundColor : public BaseAction
			{
			  public:
				explicit ChangeBackgroundColor( const Color::Rgb & p_color ) : _color( p_color ) {}

				virtual void execute() override { VTX_SETTING().backgroundColor = _color; };

			  private:
				const Color::Rgb _color;
			};

			class ChangeLightColor : public BaseAction
			{
			  public:
				explicit ChangeLightColor( const Color::Rgb & p_color ) : _color( p_color ) {}

				virtual void execute() override { VTX_SETTING().lightColor = _color; };

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
					VTX_SETTING().translationSpeed
						= Util::Math::clamp( _speed,
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
					VTX_SETTING().translationFactorSpeed
						= Util::Math::clamp( _factor,
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
					VTX_SETTING().rotationSpeed = Util::Math::clamp( _speed,
																	 VTX::Setting::CONTROLLER_ROTATION_SPEED_MIN,
																	 VTX::Setting::CONTROLLER_ROTATION_SPEED_MAX );
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

			class ChangeAutoRotateSpeed : public BaseAction
			{
			  public:
				explicit ChangeAutoRotateSpeed( const Vec3f & p_value ) : _value( p_value ) {}

				virtual void execute() override
				{
					VTX_SETTING().autoRotationSpeed = Util::Math::clamp(
						_value, VTX::Setting::AUTO_ROTATE_SPEED_MIN, VTX::Setting::AUTO_ROTATE_SPEED_MAX );
				}

				virtual void displayUsage() override { VTX_INFO( "f f f|f" ); }

			  private:
				const Vec3f _value;
			};
		} // namespace Setting
	}	  // namespace Action
} // namespace VTX
#endif
