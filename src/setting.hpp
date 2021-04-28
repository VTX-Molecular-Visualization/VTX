#ifndef __VTX_SETTING__
#define __VTX_SETTING__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "define.hpp"
#include "generic/base_colorable.hpp"
#include "style.hpp"
#include <QString>
#include <list>

namespace VTX
{
	namespace Generic
	{
		enum class COLOR_MODE;
		enum class REPRESENTATION;
	} // namespace Generic

	namespace Renderer
	{
		enum class SHADING;
		enum class MODE;
	} // namespace Renderer

	namespace Trajectory
	{
		enum class PlayMode;
	}

	class Setting
	{
	  public:
		// UI.
		static const std::string ORGANIZATION_NAME;
		static const std::string PROJECT_NAME;

		static const Style::SYMBOL_DISPLAY_MODE SYMBOL_DISPLAY_MODE_DEFAULT;
		Style::SYMBOL_DISPLAY_MODE				symbolDisplayMode = SYMBOL_DISPLAY_MODE_DEFAULT;

		static const int  WINDOW_WIDTH_DEFAULT;
		static const int  WINDOW_HEIGHT_DEFAULT;
		static const bool WINDOW_FULLSCREEN_DEFAULT;
		bool			  windowFullscreen = WINDOW_FULLSCREEN_DEFAULT;

		static const char * STYLESHEET_FILE_DEFAULT;
		static const int	CONSOLE_WIDGET_HEIGHT_DEFAULT;
		static const int	SCENE_WIDGET_WIDTH_DEFAULT;
		static const int	INSPECTOR_WIDGET_WIDTH_DEFAULT;
		static const int	RENDER_WIDGET_HEIGHT_DEFAULT;

		static const int STATUS_PROGRESS_BAR_CHUNKS;
		static const int STATUS_PROGRESS_BAR_WIDTH;

		static const QString DEFAULT_SAVE_FOLDER;
		static const QString DEFAULT_MOLECULE_FOLDER;
		static const QString MOLECULE_EXTENSIONS;
		static const QString VTX_EXTENSIONS;
		static const QString MOLECULE_FILE_FILTERS;
		static const QString OPEN_FILE_FILTERS;
		static const QString SAVE_FILE_FILTERS;

		// Rendering.
		static const bool ACTIVE_RENDERER_DEFAULT;
		bool			  activeRenderer = ACTIVE_RENDERER_DEFAULT;
		static const bool FORCE_RENDERER_DEFAULT;
		bool			  forceRenderer = FORCE_RENDERER_DEFAULT;

		static const Color::Rgb BACKGROUND_COLOR_DEFAULT;
		Color::Rgb				backgroundColor = BACKGROUND_COLOR_DEFAULT;

		static const int REPRESENTATION_DEFAULT_INDEX;
		int				 representation = REPRESENTATION_DEFAULT_INDEX;

		static const Generic::REPRESENTATION DEFAULT_REPRESENTATION_TYPE;
		Generic::REPRESENTATION				 defaultRepresentationType = DEFAULT_REPRESENTATION_TYPE;
		static const std::string			 NEW_REPRESENTATION_DEFAULT_NAME;

		static const float ATOMS_RADIUS_DEFAULT;
		static const float ATOMS_RADIUS_MIN;
		static const float ATOMS_RADIUS_MAX;
		float			   atomsRadius = ATOMS_RADIUS_DEFAULT;

		static const float BONDS_RADIUS_DEFAULT;
		static const float BONDS_RADIUS_MIN;
		static const float BONDS_RADIUS_MAX;
		float			   bondsRadius = BONDS_RADIUS_DEFAULT;
		static const float ATOMS_RADIUS_ADD_DEFAULT;
		static const float ATOMS_RADIUS_ADD_MIN;
		static const float ATOMS_RADIUS_ADD_MAX;

		static const Generic::COLOR_MODE COLOR_MODE_DEFAULT;
		Generic::COLOR_MODE				 colorMode = COLOR_MODE_DEFAULT;

		static const std::string NEW_RENDER_EFFECT_PRESET_DEFAULT_NAME;
		static const int		 RENDER_EFFECT_DEFAULT_INDEX;
		int						 renderEffectDefaultIndex = RENDER_EFFECT_DEFAULT_INDEX;
		static const int		 MAX_QUICK_ACCESS_COUNT	  = 6;

		static const Renderer::SHADING SHADING_DEFAULT;
		Renderer::SHADING			   shading = SHADING_DEFAULT;

		static const bool ACTIVE_VSYNC_DEFAULT;
		bool			  activeVSync = ACTIVE_VSYNC_DEFAULT;

		static const bool ACTIVE_AO_DEFAULT;
		bool			  activeAO = ACTIVE_AO_DEFAULT;

		static const int AO_INTENSITY_DEFAULT;
		static const int AO_INTENSITY_MIN;
		static const int AO_INTENSITY_MAX;
		int				 aoIntensity = AO_INTENSITY_DEFAULT;

		static const int AO_BLUR_SIZE_DEFAULT;
		static const int AO_BLUR_SIZE_MIN;
		static const int AO_BLUR_SIZE_MAX;
		int				 aoBlurSize = AO_BLUR_SIZE_DEFAULT;

		static const bool ACTIVE_OUTLINE_DEFAULT;
		bool			  activeOutline = ACTIVE_OUTLINE_DEFAULT;

		static const float OUTLINE_THICKNESS_DEFAULT;
		static const float OUTLINE_THICKNESS_MIN;
		static const float OUTLINE_THICKNESS_MAX;
		bool			   outlineThickness = OUTLINE_THICKNESS_DEFAULT;

		static const Color::Rgb OUTLINE_COLOR_DEFAULT;
		Color::Rgb				outlineColor = OUTLINE_COLOR_DEFAULT;

		static const bool ACTIVE_FOG_DEFAULT;
		bool			  activeFog = ACTIVE_FOG_DEFAULT;

		static const float FOG_NEAR_DEFAULT;
		static const float FOG_NEAR_MIN;
		static const float FOG_NEAR_MAX;
		float			   fogNear = FOG_NEAR_DEFAULT;

		static const float FOG_FAR_DEFAULT;
		static const float FOG_FAR_MIN;
		static const float FOG_FAR_MAX;
		float			   fogFar = FOG_FAR_DEFAULT;

		static const float FOG_DENSITY_DEFAULT;
		float			   fogDensity = FOG_DENSITY_DEFAULT;

		static const Color::Rgb FOG_COLOR_DEFAULT;
		Color::Rgb				fogColor = FOG_COLOR_DEFAULT;

		static const bool ACTIVE_AA_DEFAULT;
		bool			  activeAA = ACTIVE_AA_DEFAULT;

		static const Color::Rgb LIGHT_COLOR_DEFAULT;
		Color::Rgb				lightColor = LIGHT_COLOR_DEFAULT;

		// Camera.
		static const float CAMERA_NEAR_DEFAULT;
		static const float CAMERA_NEAR_MIN;
		static const float CAMERA_NEAR_MAX;
		float			   cameraNear = CAMERA_NEAR_DEFAULT;

		static const float CAMERA_FAR_DEFAULT;
		static const float CAMERA_FAR_MIN;
		static const float CAMERA_FAR_MAX;
		float			   cameraFar = CAMERA_FAR_DEFAULT;

		static const float CAMERA_FOV_DEFAULT;
		static const float CAMERA_FOV_MIN;
		static const float CAMERA_FOV_MAX;
		float			   cameraFov = CAMERA_FOV_DEFAULT;

		static const bool CAMERA_PERSPECTIVE_DEFAULT;
		bool			  cameraPerspective = CAMERA_PERSPECTIVE_DEFAULT;

		// Controllers.
		static const float CONTROLLER_TRANSLATION_SPEED_DEFAULT;
		static const float CONTROLLER_TRANSLATION_SPEED_MIN;
		static const float CONTROLLER_TRANSLATION_SPEED_MAX;
		float			   translationSpeed = CONTROLLER_TRANSLATION_SPEED_DEFAULT;

		static const float CONTROLLER_TRANSLATION_FACTOR_DEFAULT;
		static const float CONTROLLER_TRANSLATION_FACTOR_MIN;
		static const float CONTROLLER_TRANSLATION_FACTOR_MAX;
		float			   translationFactorSpeed = CONTROLLER_TRANSLATION_FACTOR_DEFAULT;

		static const float CONTROLLER_ROTATION_SPEED_DEFAULT;
		static const float CONTROLLER_ROTATION_SPEED_MIN;
		static const float CONTROLLER_ROTATION_SPEED_MAX;
		float			   rotationSpeed = CONTROLLER_ROTATION_SPEED_DEFAULT;

		static const bool CONTROLLER_Y_AXIS_INVERTED;
		bool			  yAxisInverted = CONTROLLER_Y_AXIS_INVERTED;

		static const bool CONTROLLER_ELASTICITY_ACTIVE_DEFAULT;
		bool			  activeControllerElasticity = CONTROLLER_ELASTICITY_ACTIVE_DEFAULT;

		static const float CONTROLLER_ELASTICITY_FACTOR_DEFAULT;
		static const float CONTROLLER_ELASTICITY_FACTOR_MIN;
		static const float CONTROLLER_ELASTICITY_FACTOR_MAX;
		float			   controllerElasticityFactor = CONTROLLER_ELASTICITY_FACTOR_DEFAULT;

		static const float CONTROLLER_ELASTICITY_THRESHOLD;

		// Molecules
		static const float COPIED_MOLECULE_OFFSET;

		// Trajectory
		static const int				  MIN_TRAJECTORY_SPEED;
		static const int				  MAX_TRAJECTORY_SPEED;
		static const int				  DEFAULT_TRAJECTORY_SPEED;
		int								  defaultTrajectorySpeed;
		static const Trajectory::PlayMode DEFAULT_TRAJECTORY_PLAY_MODE;
		Trajectory::PlayMode			  defaultTrajectoryPlayMode;

		// Auto rotate.
		static const float AUTO_ROTATE_SPEED_DEFAULT;
		static const float AUTO_ROTATE_SPEED_MIN;
		static const float AUTO_ROTATE_SPEED_MAX;
		Vec3f			   autoRotationSpeed = Vec3f( AUTO_ROTATE_SPEED_DEFAULT );

		// Video.
		static const float PATH_DURATION_DEFAULT;
		static const uint  VIDEO_FPS_DEFAULT;
		static const uint  VIDEO_CRF_DEFAULT;

		// Misc.
		static const int  CONSOLE_SIZE;
		static const uint ACTION_BUFFER_SIZE; // For undo/redo

		static const int					   RECENT_PATH_SAVED_MAX_COUNT;
		inline static std::list<VTX::FilePath> recentLoadingPath = std::list<VTX::FilePath>();
		static void							   enqueueNewLoadingPath( FilePath & );
		static VTX::FilePath				   getRecentLoadingPath( const int p_index );

		void backup();
		void recover();

		// Dev.
		static const Renderer::MODE MODE_DEFAULT;
		Renderer::MODE				mode = MODE_DEFAULT;

	  private:
		nlohmann::json _backup;
	};

} // namespace VTX
#endif
