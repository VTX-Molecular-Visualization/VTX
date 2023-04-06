#ifndef __VTX_DEFAULT_VALUES__
#define __VTX_DEFAULT_VALUES__

#include "generic/base_colorable.hpp"
#include "model/color/rgb.hpp"
#include "model/define.hpp"

namespace VTX
{
	enum class CATEGORY_ENUM;

	namespace Generic
	{
		enum class COLOR_MODE;
		enum class COLOR_BLENDING_MODE;
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

	namespace Path
	{
		enum class DURATION_MODE;
		enum class INTERPOLATION_MODE;
	} // namespace Path

	namespace Selection
	{
		enum class Granularity;
	}

	class Setting
	{
	  public:
		static const std::string REPRESENTATION_DEFAULT_NAME;
		static const int		 REPRESENTATION_DEFAULT_INDEX;

		static const Generic::REPRESENTATION DEFAULT_REPRESENTATION_TYPE;
		static const std::string			 NEW_REPRESENTATION_DEFAULT_NAME;

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

		// Path
		static const Path::DURATION_MODE	  DEFAULT_PATH_DURATION_MODE;
		static const Path::INTERPOLATION_MODE DEFAULT_PATH_INTERPOLATION_MODE;

		// Trajectory
		static const int				  MIN_TRAJECTORY_SPEED;
		static const int				  MAX_TRAJECTORY_SPEED;
		static const int				  DEFAULT_TRAJECTORY_SPEED;
		static const Trajectory::PlayMode DEFAULT_TRAJECTORY_PLAY_MODE;

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

		// Selection
		static const VTX::Selection::Granularity SELECTION_GRANULARITY_DEFAULT;

		// Misc.
		static const uint ACTION_BUFFER_SIZE; // For undo/redo

		// Dev.
		static const Renderer::MODE MODE_DEFAULT;
		Renderer::MODE				mode = MODE_DEFAULT;
	};

} // namespace VTX
#endif
