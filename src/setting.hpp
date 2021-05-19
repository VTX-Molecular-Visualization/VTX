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
		static const std::string LAYOUT_SETTINGS_FOLDER;
		static const std::string LAYOUT_SETTINGS_FILENAME;

		static const Style::SYMBOL_DISPLAY_MODE SYMBOL_DISPLAY_MODE_DEFAULT;

		static const int  WINDOW_WIDTH_DEFAULT;
		static const int  WINDOW_HEIGHT_DEFAULT;
		static const bool WINDOW_FULLSCREEN_DEFAULT;

		static const char * STYLESHEET_FILE_DEFAULT;
		static const int	CONSOLE_WIDGET_HEIGHT_DEFAULT;
		static const int	SCENE_WIDGET_WIDTH_DEFAULT;
		static const int	INSPECTOR_WIDGET_WIDTH_DEFAULT;
		static const int	RENDER_WIDGET_HEIGHT_DEFAULT;

		static const int STATUS_PROGRESS_BAR_CHUNKS;
		static const int STATUS_PROGRESS_BAR_WIDTH;

		static const QString MOLECULE_EXTENSIONS;
		static const QString VTX_EXTENSIONS;
		static const QString MOLECULE_FILE_FILTERS;
		static const QString OPEN_FILE_FILTERS;
		static const QString SAVE_FILE_FILTERS;

		// Rendering.
		static const bool ACTIVE_RENDERER_DEFAULT;
		static const bool FORCE_RENDERER_DEFAULT;

		static const Color::Rgb BACKGROUND_COLOR_DEFAULT;
		static const float		BACKGROUND_OPACITY_DEFAULT;

		static const int REPRESENTATION_DEFAULT_INDEX;

		static const Generic::REPRESENTATION DEFAULT_REPRESENTATION_TYPE;
		static const std::string			 NEW_REPRESENTATION_DEFAULT_NAME;

		static const float ATOMS_RADIUS_DEFAULT;
		static const float ATOMS_RADIUS_MIN;
		static const float ATOMS_RADIUS_MAX;

		static const float BONDS_RADIUS_DEFAULT;
		static const float BONDS_RADIUS_MIN;
		static const float BONDS_RADIUS_MAX;

		static const float ATOMS_RADIUS_ADD_DEFAULT;
		static const float ATOMS_RADIUS_ADD_MIN;
		static const float ATOMS_RADIUS_ADD_MAX;

		static const Generic::COLOR_MODE COLOR_MODE_DEFAULT;

		static const std::string NEW_RENDER_EFFECT_PRESET_DEFAULT_NAME;
		static const int		 RENDER_EFFECT_DEFAULT_INDEX;
		static const int		 MAX_QUICK_ACCESS_COUNT = 6;

		static const Renderer::SHADING SHADING_DEFAULT;

		static const bool ACTIVE_VSYNC_DEFAULT;

		static const bool ACTIVE_AO_DEFAULT;

		static const int AO_INTENSITY_DEFAULT;
		static const int AO_INTENSITY_MIN;
		static const int AO_INTENSITY_MAX;

		static const int AO_BLUR_SIZE_DEFAULT;
		static const int AO_BLUR_SIZE_MIN;
		static const int AO_BLUR_SIZE_MAX;

		static const bool ACTIVE_OUTLINE_DEFAULT;

		static const uint OUTLINE_THICKNESS_DEFAULT;
		static const uint OUTLINE_THICKNESS_MIN;
		static const uint OUTLINE_THICKNESS_MAX;

		static const float OUTLINE_SENSIVITY_DEFAULT;
		static const float OUTLINE_SENSIVITY_MIN;
		static const float OUTLINE_SENSIVITY_MAX;

		static const Color::Rgb OUTLINE_COLOR_DEFAULT;

		static const bool ACTIVE_FOG_DEFAULT;

		static const float FOG_NEAR_DEFAULT;
		static const float FOG_NEAR_MIN;
		static const float FOG_NEAR_MAX;

		static const float FOG_FAR_DEFAULT;
		static const float FOG_FAR_MIN;
		static const float FOG_FAR_MAX;

		static const float FOG_DENSITY_DEFAULT;

		static const Color::Rgb FOG_COLOR_DEFAULT;

		static const bool ACTIVE_AA_DEFAULT;

		static const Color::Rgb LIGHT_COLOR_DEFAULT;

		// Camera.
		static const float CAMERA_NEAR_DEFAULT;
		static const float CAMERA_NEAR_MIN;
		static const float CAMERA_NEAR_MAX;

		static const float CAMERA_FAR_DEFAULT;
		static const float CAMERA_FAR_MIN;
		static const float CAMERA_FAR_MAX;

		static const float CAMERA_FOV_DEFAULT;
		static const float CAMERA_FOV_MIN;
		static const float CAMERA_FOV_MAX;

		static const bool CAMERA_PERSPECTIVE_DEFAULT;

		// Controllers.
		static const float CONTROLLER_TRANSLATION_SPEED_DEFAULT;
		static const float CONTROLLER_TRANSLATION_SPEED_MIN;
		static const float CONTROLLER_TRANSLATION_SPEED_MAX;

		static const float CONTROLLER_TRANSLATION_FACTOR_DEFAULT;
		static const float CONTROLLER_TRANSLATION_FACTOR_MIN;
		static const float CONTROLLER_TRANSLATION_FACTOR_MAX;

		static const float CONTROLLER_ROTATION_SPEED_DEFAULT;
		static const float CONTROLLER_ROTATION_SPEED_MIN;
		static const float CONTROLLER_ROTATION_SPEED_MAX;

		static const bool CONTROLLER_Y_AXIS_INVERTED;

		static const bool CONTROLLER_ELASTICITY_ACTIVE_DEFAULT;

		static const float CONTROLLER_ELASTICITY_FACTOR_DEFAULT;
		static const float CONTROLLER_ELASTICITY_FACTOR_MIN;
		static const float CONTROLLER_ELASTICITY_FACTOR_MAX;

		static const float CONTROLLER_ELASTICITY_THRESHOLD;

		// Molecules
		static const float COPIED_MOLECULE_OFFSET;

		// Trajectory
		static const int				  MIN_TRAJECTORY_SPEED;
		static const int				  MAX_TRAJECTORY_SPEED;
		static const int				  DEFAULT_TRAJECTORY_SPEED;
		static const Trajectory::PlayMode DEFAULT_TRAJECTORY_PLAY_MODE;

		// Auto rotate.
		static const float AUTO_ROTATE_SPEED_DEFAULT;
		static const float AUTO_ROTATE_SPEED_MIN;
		static const float AUTO_ROTATE_SPEED_MAX;

		// Video.
		static const float PATH_DURATION_DEFAULT;
		static const uint  VIDEO_FPS_DEFAULT;
		static const uint  VIDEO_CRF_DEFAULT;

		// Misc.
		static const int  CONSOLE_SIZE;
		static const uint ACTION_BUFFER_SIZE; // For undo/redo

		// Parameters
		enum class PARAMETER
		{
			WINDOW_FULL_SCREEN,
			ACTIVATE_RENDERER,
			FORCE_RENDERER,
			VSYNC,
			SNAPSHOT_BACKGROUND_OPACITY,

			CONTROLLER_TRANSLATION_SPEED,
			CONTROLLER_TRANSLATION_SPEED_FACTOR,
			CONTROLLER_ROTATION_SPEED,
			CONTROLLER_ACTIVATE_ELASTICTY,
			CONTROLLER_ELASTICTY_FACTOR,
			CONTROLLER_Y_INVERSION,

			TRAJECTORY_DEFAULT_SPEED,
			TRAJECTORY_DEFAULT_PLAY_MODE,
			AUTO_ROTATION_DEFAULT_SPEED,

			SYMBOL_DISPLAY_MODE,

			COUNT,
			ALL,
		};

		inline bool	 getWindowFullscreen() const { return windowFullscreen; }
		void		 setWindowFullscreen( const bool p_fullscreen );
		inline bool	 getActivateRenderer() const { return activeRenderer; }
		void		 setActivateRenderer( const bool p_activeRenderer );
		inline bool	 getForceRenderer() const { return forceRenderer; }
		void		 setForceRenderer( const bool p_forceRenderer );
		inline bool	 getVSync() const { return activeVSync; }
		void		 setVSync( const bool p_activeVSync );
		inline float getSnapshotBackgroundOpacity() const { return backgroundOpacity; }
		void		 setSnapshotBackgroundOpacity( const float p_backgroundOpacity );

		inline float getTranslationSpeed() const { return translationSpeed; }
		void		 setTranslationSpeed( const float p_translationSpeed );
		inline float getTranslationSpeedFactor() const { return translationFactorSpeed; }
		void		 setTranslationSpeedFactor( const float p_translationFactorSpeed );
		inline float getRotationSpeed() const { return rotationSpeed; }
		void		 setRotationSpeed( const float p_rotationSpeed );
		inline bool	 getControllerElasticityActive() const { return activeControllerElasticity; }
		void		 setControllerElasticityActive( const bool p_activeControllerElasticity );
		inline float getControllerElasticityFactor() const { return controllerElasticityFactor; }
		void		 setControllerElasticityFactor( const float p_controllerElasticityFactor );
		inline bool	 getYAxisInverted() const { return yAxisInverted; }
		void		 setYAxisInverted( const bool p_yAxisInverted );

		inline int getDefaultRepresentationIndex() const { return representationDefaultIndex; }
		void	   setDefaultRepresentationIndex( const int p_representationDefaultIndex );
		inline int getDefaultRenderEffectPresetIndex() const { return renderEffectDefaultIndex; }
		void	   setDefaultRenderEffectPresetIndex( const int p_renderEffectDefaultIndex );

		inline int							getDefaultTrajectorySpeed() const { return defaultTrajectorySpeed; }
		void								setDefaultTrajectorySpeed( const int p_defaultTrajectorySpeed );
		inline const Trajectory::PlayMode & getDefaultTrajectoryPlayMode() const { return defaultTrajectoryPlayMode; }
		void				 setDefaultTrajectoryPlayMode( const Trajectory::PlayMode p_defaultTrajectoryPlayMode );
		inline const Vec3f & getAutoRotationSpeed() const { return autoRotationSpeed; }
		void				 setAutoRotationSpeed( const Vec3f p_autoRotationSpeed );

		inline const Style::SYMBOL_DISPLAY_MODE getSymbolDisplayMode() const { return symbolDisplayMode; }
		void setSymbolDisplayMode( const Style::SYMBOL_DISPLAY_MODE p_symbolDisplayMode );

		static const int							   RECENT_PATH_SAVED_MAX_COUNT;
		inline static std::list<const VTX::FilePath *> recentLoadingPath = std::list<const VTX::FilePath *>();
		static void									   enqueueNewLoadingPath( const FilePath & );
		static const VTX::FilePath *				   getRecentLoadingPath( const int p_index );
		static void									   cleanRecentPaths();
		static void									   loadRecentPaths();
		static void									   saveRecentPaths();

		const std::string & getTmpRepresentationDefaultName() const { return _tmpRepresentationDefaultName; };
		void				setTmpRepresentationDefaultName( const std::string & p_representationDefaultName )
		{
			_tmpRepresentationDefaultName = p_representationDefaultName;
		};
		const std::string & getTmpRenderEffectPresetDefaultName() const { return _tmpRenderEffectDefaultName; };
		void				setTmpRenderEffectPresetDefaultName( const std::string & p_tmpRenderEffectDefaultName )
		{
			_tmpRenderEffectDefaultName = p_tmpRenderEffectDefaultName;
		};

		void backup();
		void recover();
		void restore();

		// Dev.
		static const Renderer::MODE MODE_DEFAULT;
		Renderer::MODE				mode = MODE_DEFAULT;

	  private:
		bool  windowFullscreen	= WINDOW_FULLSCREEN_DEFAULT;
		bool  activeRenderer	= ACTIVE_RENDERER_DEFAULT;
		bool  forceRenderer		= FORCE_RENDERER_DEFAULT;
		bool  activeVSync		= ACTIVE_VSYNC_DEFAULT;
		float backgroundOpacity = BACKGROUND_OPACITY_DEFAULT;

		float translationSpeed			 = CONTROLLER_TRANSLATION_SPEED_DEFAULT;
		float translationFactorSpeed	 = CONTROLLER_TRANSLATION_FACTOR_DEFAULT;
		float rotationSpeed				 = CONTROLLER_ROTATION_SPEED_DEFAULT;
		bool  activeControllerElasticity = CONTROLLER_ELASTICITY_ACTIVE_DEFAULT;
		float controllerElasticityFactor = CONTROLLER_ELASTICITY_FACTOR_DEFAULT;
		bool  yAxisInverted				 = CONTROLLER_Y_AXIS_INVERTED;

		int representationDefaultIndex = REPRESENTATION_DEFAULT_INDEX;
		int renderEffectDefaultIndex   = RENDER_EFFECT_DEFAULT_INDEX;

		std::string _tmpRepresentationDefaultName = "";
		std::string _tmpRenderEffectDefaultName	  = "";

		int					 defaultTrajectorySpeed	   = DEFAULT_TRAJECTORY_SPEED;
		Trajectory::PlayMode defaultTrajectoryPlayMode = DEFAULT_TRAJECTORY_PLAY_MODE;
		Vec3f				 autoRotationSpeed		   = Vec3f( AUTO_ROTATE_SPEED_DEFAULT );

		Style::SYMBOL_DISPLAY_MODE symbolDisplayMode = SYMBOL_DISPLAY_MODE_DEFAULT;

		void _sendDataChangedEvent( const PARAMETER & p_parameter );
	};

} // namespace VTX
#endif
