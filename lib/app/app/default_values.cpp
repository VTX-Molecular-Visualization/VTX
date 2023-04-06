#include "default_values.hpp"
#include "model/chemistry/enum/category_enum.hpp"
// #include "model/render_effetc/enum/render_effect_enum.hpp"
#include "model/chemistry/enum/trajectory_enum.hpp"
#include "model/representation/enum/representation_enum.hpp"
#include "model/selection/enum/selection_enum.hpp"
#include <exception>
#include <string>

namespace VTX
{
	const std::string				   Setting::REPRESENTATION_DEFAULT_NAME		  = "Stick";
	const int						   Setting::REPRESENTATION_DEFAULT_INDEX	  = 3;
	const Generic::REPRESENTATION	   Setting::DEFAULT_REPRESENTATION_TYPE		  = Generic::REPRESENTATION::STICK;
	const std::string				   Setting::NEW_REPRESENTATION_DEFAULT_NAME	  = "New representation";
	const float						   Setting::ATOMS_RADIUS_DEFAULT			  = 0.4f;
	const float						   Setting::ATOMS_RADIUS_MIN				  = 0.01f;
	const float						   Setting::ATOMS_RADIUS_MAX				  = 1.f;
	const float						   Setting::BONDS_RADIUS_DEFAULT			  = 0.15f;
	const float						   Setting::BONDS_RADIUS_MIN				  = 0.01f;
	const float						   Setting::BONDS_RADIUS_MAX				  = 1.f;
	const Generic::COLOR_BLENDING_MODE Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT = Generic::COLOR_BLENDING_MODE::HARD;
	const float						   Setting::ATOMS_RADIUS_ADD_DEFAULT		  = 0.f;
	const float						   Setting::ATOMS_RADIUS_ADD_MIN			  = -1.f;
	const float						   Setting::ATOMS_RADIUS_ADD_MAX			  = 1.f;
	const bool						   Setting::ATOMS_IS_RADIUS_FIXED_DEFAULT	  = true;
	const Generic::SECONDARY_STRUCTURE_COLOR_MODE Setting::SS_COLOR_MODE_DEFAULT
		= Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL;
	const Generic::COLOR_BLENDING_MODE Setting::SS_COLOR_BLENDING_MODE_DEFAULT = Generic::COLOR_BLENDING_MODE::HARD;
	const float						   Setting::SES_RESOLUTION_DEFAULT		   = 0.5f;
	const float						   Setting::SES_RESOLUTION_MIN			   = 0.3f;
	const float						   Setting::SES_RESOLUTION_MAX			   = 1.f;
	const Generic::COLOR_MODE		   Setting::COLOR_MODE_DEFAULT			   = Generic::COLOR_MODE::CHAIN;

	const std::vector<std::string> Setting::DEFAULT_REPRESENTATION_PER_CATEGORY_NAME = {
		"Stick", // POLYMER
		"Stick", // CARBOHYDRATE
		"Stick", // LIGAND
		"VdW",	 // ION
		"Stick", // SOLVENT
		"Stick", // WATER
		"Stick", // UNKNOWN
	};
	const std::vector<int> Setting::DEFAULT_REPRESENTATION_PER_CATEGORY_INDEX = {
		4, // POLYMER
		4, // CARBOHYDRATE
		4, // LIGAND
		6, // ION
		4, // SOLVENT
		4, // WATER
		4, // UNKNOWN
	};

	const std::string Setting::NEW_RENDER_EFFECT_PRESET_DEFAULT_NAME = "New render preset";
	const int		  Setting::RENDER_EFFECT_DEFAULT_INDEX			 = 0;

	//	const Renderer::SHADING Setting::SHADING_DEFAULT = Renderer::SHADING::DIFFUSE;
	// #ifdef VTX_PRODUCTION
	//	const bool Setting::ACTIVE_VSYNC_DEFAULT = true;
	// #else
	//	const bool Setting::ACTIVE_VSYNC_DEFAULT = false;
	// #endif
	//	const bool		 Setting::ACTIVE_AO_DEFAULT			= true;
	//	const int		 Setting::AO_INTENSITY_DEFAULT		= 5;
	//	const int		 Setting::AO_INTENSITY_MIN			= 1;
	//	const int		 Setting::AO_INTENSITY_MAX			= 20;
	//	const int		 Setting::AO_BLUR_SIZE_DEFAULT		= 17;
	//	const int		 Setting::AO_BLUR_SIZE_MIN			= 1;
	//	const int		 Setting::AO_BLUR_SIZE_MAX			= 99;
	//	const bool		 Setting::ACTIVE_OUTLINE_DEFAULT	= false;
	//	const uint		 Setting::OUTLINE_THICKNESS_DEFAULT = 1;
	//	const uint		 Setting::OUTLINE_THICKNESS_MIN		= 1;
	//	const uint		 Setting::OUTLINE_THICKNESS_MAX		= 5;
	//	const float		 Setting::OUTLINE_SENSIVITY_DEFAULT = 0.4f;
	//	const float		 Setting::OUTLINE_SENSIVITY_MIN		= 0.01f;
	//	const float		 Setting::OUTLINE_SENSIVITY_MAX		= 1.f;
	//	const Color::Rgb Setting::OUTLINE_COLOR_DEFAULT		= Color::Rgb::WHITE;
	//	const bool		 Setting::ACTIVE_FOG_DEFAULT		= false;
	//	const float		 Setting::FOG_NEAR_DEFAULT			= 30;
	//	const float		 Setting::FOG_NEAR_MIN				= 0.f;
	//	const float		 Setting::FOG_NEAR_MAX				= 1000.f;
	//	const float		 Setting::FOG_FAR_DEFAULT			= 80.f;
	//	const float		 Setting::FOG_FAR_MIN				= 0.f;
	//	const float		 Setting::FOG_FAR_MAX				= 1000.f;
	//	const float		 Setting::FOG_DENSITY_DEFAULT		= 0.8f;
	//	const Color::Rgb Setting::FOG_COLOR_DEFAULT			= Color::Rgb::WHITE;
	//	const bool		 Setting::ACTIVE_AA_DEFAULT			= true;
	//	const Color::Rgb Setting::LIGHT_COLOR_DEFAULT		= Color::Rgb::WHITE;

	// Scene.
	const Vec3f Setting::MIN_SCENE_POS = Vec3f( -10000, -10000, -10000 );
	const Vec3f Setting::MAX_SCENE_POS = Vec3f( 10000, 10000, 10000 );

	// Camera.
	const float Setting::CAMERA_NEAR_DEFAULT		= 0.00f;
	const float Setting::CAMERA_NEAR_MIN			= 0.00f;
	const float Setting::CAMERA_NEAR_MAX			= 1e4f;
	const float Setting::CAMERA_FAR_DEFAULT			= 1e4f;
	const float Setting::CAMERA_FAR_MIN				= 0.01f;
	const float Setting::CAMERA_FAR_MAX				= 1e4f;
	const float Setting::CAMERA_FOV_DEFAULT			= 45.f;
	const float Setting::CAMERA_FOV_MIN				= 10.f;
	const float Setting::CAMERA_FOV_MAX				= 90.f;
	const bool	Setting::CAMERA_PERSPECTIVE_DEFAULT = true;

	// Molecule
	const float Setting::COPIED_MOLECULE_OFFSET = 5.0f;

	// Trajectory
	const int				   Setting::MIN_TRAJECTORY_SPEED		 = 1;
	const int				   Setting::MAX_TRAJECTORY_SPEED		 = 60;
	const int				   Setting::DEFAULT_TRAJECTORY_SPEED	 = 5;
	const Trajectory::PlayMode Setting::DEFAULT_TRAJECTORY_PLAY_MODE = Trajectory::PlayMode::Loop;

	// Transform
	const float Setting::MIN_EULER	   = -10000.f;
	const float Setting::MAX_EULER	   = 10000.f;
	const Vec3f Setting::MIN_EULER_VEC = Vec3f( MIN_EULER, MIN_EULER, MIN_EULER );
	const Vec3f Setting::MAX_EULER_VEC = Vec3f( MAX_EULER, MAX_EULER, MAX_EULER );
	const float Setting::MIN_SCALE	   = 0.01f;
	const float Setting::MAX_SCALE	   = 10000.f;
	const Vec3f Setting::MIN_SCALE_VEC = Vec3f( MIN_SCALE, MIN_SCALE, MIN_SCALE );
	const Vec3f Setting::MAX_SCALE_VEC = Vec3f( MAX_SCALE, MAX_SCALE, MAX_SCALE );

	// Auto rotate.
	const float Setting::AUTO_ROTATE_SPEED_DEFAULT		 = 2.0f;
	const Vec3f Setting::AUTO_ROTATE_ORIENTATION_DEFAULT = Vec3f( 0.0f, 1.0f, 0.0f );
	const float Setting::AUTO_ROTATE_SPEED_MIN			 = 0.1f;
	const float Setting::AUTO_ROTATE_SPEED_MAX			 = 15.0f;

	// Selection
	const VTX::Selection::Granularity Setting::SELECTION_GRANULARITY_DEFAULT = VTX::Selection::Granularity::RESIDUE;

	// Misc.
	const uint Setting::ACTION_BUFFER_SIZE = 10;
} // namespace VTX
