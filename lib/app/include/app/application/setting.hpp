#ifndef __VTX_APP_APPLICATION_SETTING__
#define __VTX_APP_APPLICATION_SETTING__

#include "app/application/representation/_fwd.hpp"
#include "app/application/selection/enum_selection.hpp"
#include "app/component/chemistry/enum_trajectory.hpp"
#include "app/component/video/enum_path.hpp"
#include "app/internal/chemdb/category.hpp"
#include "app/internal/io/serialization/image_export.hpp"
#include "app/old_app/generic/base_colorable.hpp"
#include "app/old_app/id.hpp"
#include "app/old_app/style.hpp"
#include "app/render/renderer/enum_renderer.hpp"
#include <list>
#include <string>
#include <util/color/rgba.hpp>
#include <util/types.hpp>

namespace VTX
{
	namespace Generic
	{
		enum class COLOR_MODE;
		enum class COLOR_BLENDING_MODE;
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

	namespace App::Application
	{
		class Setting
		{
		  public:
			// UI.
			static const Style::SYMBOL_DISPLAY_MODE SYMBOL_DISPLAY_MODE_DEFAULT;
			static const bool						WINDOW_FULLSCREEN_DEFAULT;

			// Rendering.
			static const bool ACTIVE_RENDERER_DEFAULT;
			static const bool FORCE_RENDERER_DEFAULT;

			static const Util::Color::Rgba BACKGROUND_COLOR_DEFAULT;

			static const App::Internal::IO::Serialization::ImageExport::RESOLUTION SNAPSHOT_RESOLUTION_DEFAULT;
			static const App::Internal::IO::Serialization::ImageExport::Format	   SNAPSHOT_FORMAT_DEFAULT;
			static const float													   BACKGROUND_OPACITY_DEFAULT;
			static const float													   SNAPSHOT_QUALITY_DEFAULT;

			static const std::string REPRESENTATION_DEFAULT_NAME;
			static const int		 REPRESENTATION_DEFAULT_INDEX;

			static const Representation::REPRESENTATION_ENUM DEFAULT_REPRESENTATION_TYPE;
			static const std::string						 NEW_REPRESENTATION_DEFAULT_NAME;

			static const float ATOMS_RADIUS_DEFAULT;
			static const float ATOMS_RADIUS_MIN;
			static const float ATOMS_RADIUS_MAX;

			static const float						  BONDS_RADIUS_DEFAULT;
			static const float						  BONDS_RADIUS_MIN;
			static const float						  BONDS_RADIUS_MAX;
			static const Generic::COLOR_BLENDING_MODE BONDS_COLOR_BLENDING_MODE_DEFAULT;

			static const float ATOMS_RADIUS_ADD_DEFAULT;
			static const float ATOMS_RADIUS_ADD_MIN;
			static const float ATOMS_RADIUS_ADD_MAX;

			static const bool ATOMS_IS_RADIUS_FIXED_DEFAULT;

			static const Generic::SECONDARY_STRUCTURE_COLOR_MODE SS_COLOR_MODE_DEFAULT;
			static const Generic::COLOR_BLENDING_MODE			 SS_COLOR_BLENDING_MODE_DEFAULT;

			static const float SES_RESOLUTION_DEFAULT;
			static const float SES_RESOLUTION_MIN;
			static const float SES_RESOLUTION_MAX;

			static const Generic::COLOR_MODE COLOR_MODE_DEFAULT;

			static const std::vector<std::string> DEFAULT_REPRESENTATION_PER_CATEGORY_NAME;
			static const std::vector<int>		  DEFAULT_REPRESENTATION_PER_CATEGORY_INDEX;

			static const std::string NEW_RENDER_EFFECT_PRESET_DEFAULT_NAME;
			static const int		 RENDER_EFFECT_DEFAULT_INDEX;
			static const int		 MAX_QUICK_ACCESS_COUNT = 6;

			static const App::Render::Renderer::SHADING SHADING_DEFAULT;

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

			static const Util::Color::Rgba OUTLINE_COLOR_DEFAULT;

			static const bool ACTIVE_FOG_DEFAULT;

			static const float FOG_NEAR_DEFAULT;
			static const float FOG_NEAR_MIN;
			static const float FOG_NEAR_MAX;

			static const float FOG_FAR_DEFAULT;
			static const float FOG_FAR_MIN;
			static const float FOG_FAR_MAX;

			static const float FOG_DENSITY_DEFAULT;

			static const Util::Color::Rgba FOG_COLOR_DEFAULT;

			static const bool ACTIVE_AA_DEFAULT;

			static const Util::Color::Rgba LIGHT_COLOR_DEFAULT;

			// Scene.
			static const Vec3f MIN_SCENE_POS;
			static const Vec3f MAX_SCENE_POS;

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

			static const float CONTROLLER_ACCELERATION_FACTOR_DEFAULT;
			static const float CONTROLLER_ACCELERATION_FACTOR_MIN;
			static const float CONTROLLER_ACCELERATION_FACTOR_MAX;
			static const float CONTROLLER_DECELERATION_FACTOR_DEFAULT;
			static const float CONTROLLER_DECELERATION_FACTOR_MIN;
			static const float CONTROLLER_DECELERATION_FACTOR_MAX;

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

			// Path
			static const App::Component::Video::PATH_DURATION_MODE		DEFAULT_PATH_DURATION_MODE;
			static const App::Component::Video::PATH_INTERPOLATION_MODE DEFAULT_PATH_INTERPOLATION_MODE;

			// Trajectory
			static const int								 MIN_TRAJECTORY_SPEED;
			static const int								 MAX_TRAJECTORY_SPEED;
			static const int								 DEFAULT_TRAJECTORY_SPEED;
			static const App::Component::Chemistry::PlayMode DEFAULT_TRAJECTORY_PLAY_MODE;

			// Transform
			static const float MIN_EULER;
			static const float MAX_EULER;
			static const Vec3f MIN_EULER_VEC;
			static const Vec3f MAX_EULER_VEC;
			static const float MIN_SCALE;
			static const float MAX_SCALE;
			static const Vec3f MIN_SCALE_VEC;
			static const Vec3f MAX_SCALE_VEC;

			// Auto rotate.
			static const float AUTO_ROTATE_SPEED_DEFAULT;
			static const Vec3f AUTO_ROTATE_ORIENTATION_DEFAULT;
			static const float AUTO_ROTATE_SPEED_MIN;
			static const float AUTO_ROTATE_SPEED_MAX;

			// Video.
			static const float		PATH_DURATION_DEFAULT;
			static const ID::VTX_ID CONTROLLER_MODE_DEFAULT;
			static const ID::VTX_ID PICKER_MODE_DEFAULT;
			static const uint		VIDEO_FPS_DEFAULT;
			static const uint		VIDEO_CRF_DEFAULT;

			// Measurement
			static const Util::Color::Rgba DEFAULT_LABEL_COLOR;

			// Selection
			static const VTX::App::Application::Selection::GRANULARITY SELECTION_GRANULARITY_DEFAULT;

			// Structural Alignment
			static const int   CE_ALIGN_WIN_SIZE_DEFAULT;
			static const int   CE_ALIGN_GAP_MAX_DEFAULT;
			static const int   CE_ALIGN_MAX_PATH_DEFAULT;
			static const float CE_ALIGN_D0_DEFAULT;
			static const float CE_ALIGN_D1_DEFAULT;

			// Misc.
			static const int   CONSOLE_SIZE;
			static const uint  ACTION_BUFFER_SIZE; // For undo/redo
			static const bool  COMPUTE_BOND_ORDER_ON_CHEMFILE;
			static const bool  CHECK_VTX_UPDATE_DEFAULT;
			static const bool  PORTABLE_SAVE_ACTIVATED_DEFAULT;
			static const float CELL_LIST_CUBE_SIZE;

			// Parameters
			enum class PARAMETER : int
			{
				WINDOW_FULL_SCREEN,
				ACTIVATE_RENDERER,
				FORCE_RENDERER,
				VSYNC,

				SNAPSHOT_FORMAT,
				SNAPSHOT_BACKGROUND_OPACITY,
				SNAPSHOT_RESOLUTION,
				SNAPSHOT_QUALITY,

				CAMERA_FOV,
				CAMERA_NEAR_CLIP,
				CAMERA_FAR_CLIP,
				CAMERA_ANTI_ALIASING,
				CAMERA_PROJECTION,

				CONTROLLER_TRANSLATION_SPEED,
				CONTROLLER_TRANSLATION_SPEED_FACTOR,
				CONTROLLER_ROTATION_SPEED,
				CONTROLLER_ACTIVATE_ELASTICTY,
				CONTROLLER_ELASTICTY_FACTOR,
				CONTROLLER_Y_INVERSION,

				SELECTION_GRANULARITY,

				TRAJECTORY_DEFAULT_SPEED,
				TRAJECTORY_DEFAULT_PLAY_MODE,
				AUTO_ROTATION_DEFAULT_SPEED,

				SYMBOL_DISPLAY_MODE,
				CHECK_VTX_UPDATE,
				PORTABLE_SAVE_ACTIVATED,

				DEFAULT_REPRESENTATION_PER_CATEGORY,

				COUNT,
				ALL,
			};

			inline void freezeEvent( const bool p_freeze ) { _freezeEvent = p_freeze; }

			inline bool getWindowFullscreen() const { return windowFullscreen; }
			void		setWindowFullscreen( const bool p_fullscreen );
			inline bool getActivateRenderer() const { return activeRenderer; }
			void		setActivateRenderer( const bool p_activeRenderer );
			inline bool getForceRenderer() const { return forceRenderer; }
			void		setForceRenderer( const bool p_forceRenderer );
			inline bool getVSync() const { return activeVSync; }
			void		setVSync( const bool p_activeVSync );

			inline App::Internal::IO::Serialization::ImageExport::Format getSnapshotFormat() const
			{
				return snapshotFormat;
			}
			void		 setSnapshotFormat( const App::Internal::IO::Serialization::ImageExport::Format p_format );
			inline float getSnapshotBackgroundOpacity() const { return backgroundOpacity; }
			void		 setSnapshotBackgroundOpacity( const float p_backgroundOpacity );
			inline float getSnapshotQuality() const { return snapshotQuality; }
			void		 setSnapshotQuality( const float p_snapshotQuality );
			inline App::Internal::IO::Serialization::ImageExport::RESOLUTION getSnapshotResolution() const
			{
				return snapshotResolution;
			}
			void setSnapshotResolution(
				const App::Internal::IO::Serialization::ImageExport::RESOLUTION & p_snapshotResolution );

			// Camera Settings
			inline float getCameraFOV() const { return cameraFOV; };
			void		 setCameraFOV( const float p_cameraFOV );
			inline float getCameraNearClip() const { return cameraNearClip; };
			void		 setCameraNearClip( const float p_cameraNearClip );
			inline float getCameraFarClip() const { return cameraFarClip; };
			void		 setCameraFarClip( const float p_cameraFarClip );
			inline bool	 getAA() const { return antiAliasing; };
			void		 setAA( const bool p_antiAliasing );
			inline bool	 getCameraPerspective() const { return cameraPerspective; };
			void		 setCameraPerspectiveProjection( const bool p_cameraPerspective );

			inline float getTranslationSpeed() const { return translationSpeed; }
			void		 setTranslationSpeed( const float p_translationSpeed );
			inline float getAccelerationSpeedFactor() const { return accelerationFactorSpeed; }
			inline float getDecelerationSpeedFactor() const { return decelerationFactorSpeed; }
			void		 setAccelerationSpeedFactor( const float p_factorSpeed );
			void		 setDecelerationSpeedFactor( const float p_factorSpeed );

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

			int getDefaultRepresentationIndexPerCategory(
				const App::Internal::ChemDB::Category::TYPE & p_categoryEnum ) const;
			void setDefaultRepresentationIndexPerCategory( const App::Internal::ChemDB::Category::TYPE & p_categoryEnum,
														   const int p_representationDefaultIndex );
			const std::string & getTmpDefaultRepresentationNamePerCategory(
				const App::Internal::ChemDB::Category::TYPE & p_categoryEnum );
			void setTmpDefaultRepresentationNamePerCategory(
				const App::Internal::ChemDB::Category::TYPE & p_categoryEnum,
				const std::string &							  p_representationDefaultName );

			inline VTX::App::Application::Selection::GRANULARITY getSelectionGranularity() const
			{
				return selectionGranularity;
			}
			void setSelectionGranularity(
				const VTX::App::Application::Selection::GRANULARITY & p_selectionGranularity );

			inline int getDefaultTrajectorySpeed() const { return defaultTrajectorySpeed; }
			void	   setDefaultTrajectorySpeed( const int p_defaultTrajectorySpeed );
			inline const App::Component::Chemistry::PlayMode & getDefaultTrajectoryPlayMode() const
			{
				return defaultTrajectoryPlayMode;
			}
			void setDefaultTrajectoryPlayMode( const App::Component::Chemistry::PlayMode p_defaultTrajectoryPlayMode );

			inline const Style::SYMBOL_DISPLAY_MODE getSymbolDisplayMode() const { return symbolDisplayMode; }
			void setSymbolDisplayMode( const Style::SYMBOL_DISPLAY_MODE p_symbolDisplayMode );

			inline const bool getCheckVTXUpdateAtLaunch() const { return checkVTXUpdate; }
			void			  setCheckVTXUpdateAtLaunch( const bool p_checkVTXUpdate );

			inline const bool isPortableSaveActivated() const { return portableSaveActivated; }
			void			  activatePortableSave( const bool p_activate );

			static const int				  RECENT_PATH_SAVED_MAX_COUNT;
			static const int				  RECENT_DOWNLOAD_CODE_SAVED_MAX_COUNT;
			inline static std::list<FilePath> recentLoadingPath = std::list<FilePath>();
			static void						  enqueueNewLoadingPath( const FilePath & );
			static const FilePath * const	  getRecentLoadingPath( const int p_index );

			inline static std::list<std::string> recentDownloadCodes = std::list<std::string>();
			static void							 enqueueNewDownloadCode( const std::string & );
			static const std::string * const	 getRecentDownloadCode( const int p_index );

			static void loadRecentPaths();
			static void saveRecentPaths();

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

			static std::string getLastLoadedSessionFolder();
			static void		   saveLastLoadedSessionFolder( const std::string & p_path );
			static std::string getLastSavedSessionFolder();
			static void		   saveLastSavedSessionFolder( const std::string & p_path );
			static std::string getLastImportedMoleculeFolder();
			static void		   saveLastImportedMoleculeFolder( const std::string & p_path );
			static std::string getLastExportedMoleculeFolder();
			static void		   saveLastExportedMoleculeFolder( const std::string & p_path );
			static std::string getLastExportedImageFolder();
			static void		   saveLastExportedImageFolder( const std::string & p_path );

			void restoreDefaultRepresentationPerCategory();

			void backup();
			void recover();
			void restore();

			// Dev.
			static const App::Render::Renderer::MODE MODE_DEFAULT;
			App::Render::Renderer::MODE				 mode = MODE_DEFAULT;

		  private:
			bool _freezeEvent = false;

			bool windowFullscreen = WINDOW_FULLSCREEN_DEFAULT;
			bool activeRenderer	  = ACTIVE_RENDERER_DEFAULT;
			bool forceRenderer	  = FORCE_RENDERER_DEFAULT;
			bool activeVSync	  = ACTIVE_VSYNC_DEFAULT;

			App::Internal::IO::Serialization::ImageExport::Format	  snapshotFormat	 = SNAPSHOT_FORMAT_DEFAULT;
			App::Internal::IO::Serialization::ImageExport::RESOLUTION snapshotResolution = SNAPSHOT_RESOLUTION_DEFAULT;
			float													  backgroundOpacity	 = BACKGROUND_OPACITY_DEFAULT;
			float													  snapshotQuality	 = SNAPSHOT_QUALITY_DEFAULT;

			float cameraFOV			= CAMERA_FOV_DEFAULT;
			float cameraNearClip	= CAMERA_NEAR_DEFAULT;
			float cameraFarClip		= CAMERA_FAR_DEFAULT;
			bool  antiAliasing		= ACTIVE_AA_DEFAULT;
			bool  cameraPerspective = CAMERA_PERSPECTIVE_DEFAULT;

			float translationSpeed			 = CONTROLLER_TRANSLATION_SPEED_DEFAULT;
			float accelerationFactorSpeed	 = CONTROLLER_ACCELERATION_FACTOR_DEFAULT;
			float decelerationFactorSpeed	 = CONTROLLER_DECELERATION_FACTOR_DEFAULT;
			float rotationSpeed				 = CONTROLLER_ROTATION_SPEED_DEFAULT;
			bool  activeControllerElasticity = CONTROLLER_ELASTICITY_ACTIVE_DEFAULT;
			float controllerElasticityFactor = CONTROLLER_ELASTICITY_FACTOR_DEFAULT;
			bool  yAxisInverted				 = CONTROLLER_Y_AXIS_INVERTED;

			int representationDefaultIndex = REPRESENTATION_DEFAULT_INDEX;
			int renderEffectDefaultIndex   = RENDER_EFFECT_DEFAULT_INDEX;

			std::vector<int> representationPerCategory = DEFAULT_REPRESENTATION_PER_CATEGORY_INDEX;

			std::string _tmpRepresentationDefaultName = "";
			std::string _tmpRenderEffectDefaultName	  = "";

			std::vector<std::string> _tmpRepresentationPerCategory = DEFAULT_REPRESENTATION_PER_CATEGORY_NAME;

			VTX::App::Application::Selection::GRANULARITY selectionGranularity = SELECTION_GRANULARITY_DEFAULT;

			int									defaultTrajectorySpeed	  = DEFAULT_TRAJECTORY_SPEED;
			App::Component::Chemistry::PlayMode defaultTrajectoryPlayMode = DEFAULT_TRAJECTORY_PLAY_MODE;

			Style::SYMBOL_DISPLAY_MODE symbolDisplayMode	 = SYMBOL_DISPLAY_MODE_DEFAULT;
			bool					   checkVTXUpdate		 = CHECK_VTX_UPDATE_DEFAULT;
			bool					   portableSaveActivated = PORTABLE_SAVE_ACTIVATED_DEFAULT;

			static std::string _getFileInRegisterKey( const std::string & p_key, const std::string & p_default );

			void _sendDataChangedEvent( const PARAMETER & p_parameter );
		};

	} // namespace App::Application
} // namespace VTX
#endif
