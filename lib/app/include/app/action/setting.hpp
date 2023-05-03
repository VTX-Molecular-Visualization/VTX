#ifndef __VTX_APP_ACTION_SETTING__
#define __VTX_APP_ACTION_SETTING__

#include "app/action.hpp"
#include "app/application/selection/enum_selection.hpp"
#include "app/application/setting.hpp"
#include "app/component/chemistry/enum_trajectory.hpp"
#include "app/core/action/base_action.hpp"
#include "app/internal/chemdb/category.hpp"
#include "app/internal/chemdb/residue.hpp"
#include "app/internal/io/serialization/image_export.hpp"
#include "app/old_app/generic/base_colorable.hpp"
#include <util/color/rgba.hpp>
#include <util/logger.hpp>

namespace VTX::App::Action::Setting
{
	class Load : public App::Core::Action::BaseAction
	{
	  public:
		explicit Load() {}
		virtual void execute() override;
	};

	class Save : public App::Core::Action::BaseAction
	{
	  public:
		explicit Save() {}
		virtual void execute() override;
	};

	class ActiveRenderer : public App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveRenderer( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ForceRenderer : public App::Core::Action::BaseAction
	{
	  public:
		explicit ForceRenderer( const bool p_force ) : _force( p_force ) {}

		virtual void execute() override;

	  private:
		const bool _force;
	};

	class ChangeBackgroundColor : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeBackgroundColor( const Util::Color::Rgba & p_color ) : _color( p_color ) {}

		virtual void execute() override;

	  private:
		const Util::Color::Rgba _color;
	};

	class ChangeSnapshotFormat : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSnapshotFormat( const App::Internal::IO::Serialization::ImageExport::Format p_format ) :
			_format( p_format )
		{
		}

		virtual void execute() override;

	  private:
		const App::Internal::IO::Serialization::ImageExport::Format _format;
	};

	class ChangeBackgroundOpacity : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeBackgroundOpacity( const float p_opacity ) : _opacity( p_opacity ) {}

		virtual void execute() override;

	  private:
		const float _opacity;
	};

	class ChangeSnapshotQuality : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSnapshotQuality( const float p_quality ) : _quality( p_quality ) {}

		virtual void execute() override;

	  private:
		const float _quality;
	};

	class ChangeSnapshotResolution : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSnapshotResolution(
			const App::Internal::IO::Serialization::ImageExport::RESOLUTION & p_resolution ) :
			_resolution( p_resolution )
		{
		}

		virtual void execute() override;

	  private:
		const App::Internal::IO::Serialization::ImageExport::RESOLUTION _resolution;
	};

	class ChangeDefaultRepresentation : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDefaultRepresentation( const int p_representationIndex ) :
			_representationIndex( p_representationIndex )
		{
		}

		virtual void execute() override;

		virtual void displayUsage() override { VTX_INFO( "BALL_AND_STICK|VAN_DER_WAALS|STICK|SAS" ); }

	  private:
		const int _representationIndex;
	};

	class ChangeDefaultRenderEffectPreset : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDefaultRenderEffectPreset( const int p_renderEffectPresetIndex ) :
			_renderEffectPresetIndex( p_renderEffectPresetIndex )
		{
		}

		virtual void execute() override;

		virtual void displayUsage() override { VTX_INFO( "BALL_AND_STICK|VAN_DER_WAALS|STICK|SAS" ); }

	  private:
		const int _renderEffectPresetIndex;
	};

	class ChangeColorMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColorMode( const Generic::COLOR_MODE p_mode ) : _mode( p_mode ) {}

		virtual void execute() override;

		virtual void displayUsage() override { VTX_INFO( "ATOM|RESIDUE|CHAIN|PROTEIN" ); }

	  private:
		const Generic::COLOR_MODE _mode;
	};

	class ChangeShading : public App::Core::Action::BaseAction
	{
	  public:
		ChangeShading( const App::Render::Renderer::SHADING p_shading ) : _shading( p_shading ) {}

		virtual void execute() override;

		virtual void displayUsage() override { VTX_INFO( "DIFFUSE|GLOSSY|TOON|FLAT_COLOR" ); }

	  private:
		const App::Render::Renderer::SHADING _shading;
	};

	class ActiveVerticalSync : public App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveVerticalSync( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ActiveAO : public App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveAO( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ChangeAOIntensity : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeAOIntensity( const int p_intensity ) : _intensity( p_intensity ) {}

		virtual void execute() override;

	  private:
		const int _intensity;
	};

	class ChangeAOBlurSize : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeAOBlurSize( const int p_blurSize ) : _blurSize( p_blurSize ) {}

		virtual void execute() override;

	  private:
		const int _blurSize;
	};

	class ActiveOutline : public App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveOutline( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ChangeOutlineColor : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeOutlineColor( const Util::Color::Rgba & p_color ) : _color( p_color ) {}

		virtual void execute() override;

	  private:
		const Util::Color::Rgba _color;
	};

	class ChangeOutlineThickness : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeOutlineThickness( const uint p_thickness ) : _thickness( p_thickness ) {}

		virtual void execute() override;

	  private:
		const uint _thickness;
	};

	class ChangeOutlineSensivity : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeOutlineSensivity( const float p_sensivity ) : _sensivity( p_sensivity ) {}

		virtual void execute() override;

	  private:
		const float _sensivity;
	};

	class ActiveFog : public App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveFog( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ChangeFogNear : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeFogNear( const float p_near ) : _near( p_near ) {}

		virtual void execute() override;

	  private:
		const float _near;
	};

	class ChangeFogFar : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeFogFar( const float p_far ) : _far( p_far ) {}

		virtual void execute() override;

	  private:
		const float _far;
	};

	class ChangeFogDensity : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeFogDensity( const float p_density ) : _density( p_density ) {}

		virtual void execute() override;

	  private:
		const float _density;
	};

	class ChangeFogColor : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeFogColor( const Util::Color::Rgba & p_color ) : _color( p_color ) {}

		virtual void execute() override;

	  private:
		const Util::Color::Rgba _color;
	};

	class ActiveAA : public App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveAA( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ChangeLightColor : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeLightColor( const Util::Color::Rgba & p_color ) : _color( p_color ) {}

		virtual void execute() override;

	  private:
		const Util::Color::Rgba _color;
	};

	class ChangeCameraClip : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCameraClip( const float p_near, const float p_far ) : _near( p_near ), _far( p_far ) {}

		virtual void execute() override;

	  private:
		const float _near;
		const float _far;
	};

	class ChangeCameraFov : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCameraFov( const float p_fov ) : _fov( p_fov ) {}

		virtual void execute() override;

	  private:
		const float _fov;
	};

	class ChangeCameraProjectionToPerspective : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCameraProjectionToPerspective( const bool p_perspective ) : _perspective( p_perspective ) {}

		virtual void execute() override;

	  private:
		const bool _perspective;
	};

	class ChangeTranslationSpeed : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeTranslationSpeed( const float p_speed ) : _speed( p_speed ) {}

		virtual void execute() override;

	  private:
		const float _speed;
	};

	class ChangeAccelerationFactorSpeed : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeAccelerationFactorSpeed( const float p_factor ) : _factor( p_factor ) {}

		virtual void execute() override;

	  private:
		const float _factor;
	};

	class ChangeDecelerationFactorSpeed : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDecelerationFactorSpeed( const float p_factor ) : _factor( p_factor ) {}

		virtual void execute() override;

	  private:
		const float _factor;
	};

	class ChangeRotationSpeed : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRotationSpeed( const float p_speed ) : _speed( p_speed ) {}

		virtual void execute() override;

	  private:
		const float _speed;
	};

	class ActiveYAxisInversion : public App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveYAxisInversion( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ActiveControllerElasticity : public App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveControllerElasticity( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ChangeControllerElasticity : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeControllerElasticity( const float p_elasticity ) : _elasticity( p_elasticity ) {}

		virtual void execute() override;

	  private:
		const float _elasticity;
	};

	class ChangeDefaultTrajectorySpeed : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDefaultTrajectorySpeed( const int p_speed ) : _speed( p_speed ) {}

		virtual void execute() override;

	  private:
		const int _speed;
	};

	class ChangeDefaultTrajectoryPlayMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDefaultTrajectoryPlayMode( const Component::Chemistry::PlayMode p_playMode ) :
			_playMode( p_playMode )
		{
		}

		virtual void execute() override;

	  private:
		const Component::Chemistry::PlayMode _playMode;
	};

	class ChangeSymbolDisplayMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSymbolDisplayMode( const Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE & p_displayMode ) :
			_displayMode( p_displayMode )
		{
		}

		virtual void execute() override;

	  private:
		const Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE _displayMode;
	};

	class ChangeCheckVTXUpdateAtLaunch : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCheckVTXUpdateAtLaunch( const bool p_checkVtxUpdateAtLaunch ) :
			_checkVTXUpdateAtLaunch( p_checkVtxUpdateAtLaunch )
		{
		}

		virtual void execute() override;

	  private:
		const bool _checkVTXUpdateAtLaunch;
	};

	class ActivatePortableSave : public App::Core::Action::BaseAction
	{
	  public:
		explicit ActivatePortableSave( const bool p_activate ) : _activate( p_activate ) {}

		virtual void execute() override;

	  private:
		const bool _activate;
	};

	class ChangeRenderMode : public App::Core::Action::BaseAction
	{
	  public:
		ChangeRenderMode( const App::Render::Renderer::MODE p_mode ) : _mode( p_mode ) {}

		virtual void execute() override;

	  private:
		const App::Render::Renderer::MODE _mode;
	};

	class ChangeSelectionGranularity : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSelectionGranularity( const VTX::App::Application::Selection::GRANULARITY & p_granularity ) :
			_granularity( p_granularity )
		{
		}

		virtual void execute() override;

	  private:
		const VTX::App::Application::Selection::GRANULARITY _granularity;
	};

	class ChangeDefaultRepresentationPerCategory : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDefaultRepresentationPerCategory( const Internal::ChemDB::Category::TYPE & p_categoryEnum,
														 const int p_representationIndex ) :
			_categoryEnum( p_categoryEnum ),
			_representationIndex( p_representationIndex )
		{
		}

		virtual void execute() override;

	  private:
		const Internal::ChemDB::Category::TYPE _categoryEnum;
		const int							   _representationIndex;
	};

	class ApplyAllSettings : public App::Core::Action::BaseAction
	{
	  public:
		ApplyAllSettings( VTX::App::Application::Setting & p_setting ) : _setting( p_setting ) {}

		virtual void execute() override;

	  private:
		VTX::App::Application::Setting _setting;
	};

	class ReloadSetting : public App::Core::Action::BaseAction
	{
	  public:
		ReloadSetting() {}

		virtual void execute() override;
	};

	class RestoreSetting : public App::Core::Action::BaseAction
	{
	  public:
		RestoreSetting() {}

		virtual void execute() override;
	};

	class RestoreDefaultRepresentationPerCategory : public App::Core::Action::BaseAction
	{
	  public:
		explicit RestoreDefaultRepresentationPerCategory() {}

		virtual void execute() override;
	};
} // namespace VTX::App::Action::Setting

#endif
