#include "setting.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "io/serializer.hpp"
#include "model/representation/representation_enum.hpp"
#include "renderer/base_renderer.hpp"
#include "trajectory/trajectory_enum.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	// UI.

	const std::string Setting::ORGANIZATION_NAME = "CNAM";
	const std::string Setting::PROJECT_NAME		 = "VTX";

	const Style::SYMBOL_DISPLAY_MODE Setting::SYMBOL_DISPLAY_MODE_DEFAULT = Style::SYMBOL_DISPLAY_MODE::SHORT;
	const int						 Setting::WINDOW_WIDTH_DEFAULT		  = 1280;
	const int						 Setting::WINDOW_HEIGHT_DEFAULT		  = 720;
	const bool						 Setting::WINDOW_FULLSCREEN_DEFAULT	  = false;

	const char * Setting::STYLESHEET_FILE_DEFAULT		 = ":/stylesheet.css";
	const int	 Setting::CONSOLE_WIDGET_HEIGHT_DEFAULT	 = 200;
	const int	 Setting::SCENE_WIDGET_WIDTH_DEFAULT	 = 50;
	const int	 Setting::INSPECTOR_WIDGET_WIDTH_DEFAULT = 50;
	const int	 Setting::RENDER_WIDGET_HEIGHT_DEFAULT
		= Setting::WINDOW_HEIGHT_DEFAULT - Setting::CONSOLE_WIDGET_HEIGHT_DEFAULT;

	const int Setting::STATUS_PROGRESS_BAR_CHUNKS = 10;
	const int Setting::STATUS_PROGRESS_BAR_WIDTH  = 100;

	const QString Setting::DEFAULT_SAVE_FOLDER	   = "../save";
	const QString Setting::DEFAULT_MOLECULE_FOLDER = "../data";

	const QString Setting::MOLECULE_EXTENSIONS = "*.pdb *.cif *.mmtf *.mol2 *.arc *.psf *.prm";
	const QString Setting::VTX_EXTENSIONS	   = "*.vtx";

	const QString Setting::MOLECULE_FILE_FILTERS = "Molecule file (" + MOLECULE_EXTENSIONS + ")";
	const QString Setting::OPEN_FILE_FILTERS	 = "VTX file (" + VTX_EXTENSIONS + " " + MOLECULE_EXTENSIONS + ")";
	const QString Setting::SAVE_FILE_FILTERS	 = "VTX file (" + VTX_EXTENSIONS + ")";

	// Rendering.
	const bool					  Setting::ACTIVE_RENDERER_DEFAULT		   = true;
	const bool					  Setting::FORCE_RENDERER_DEFAULT		   = false;
	const Color::Rgb			  Setting::BACKGROUND_COLOR_DEFAULT		   = Color::Rgb::BLACK;
	const float					  Setting::BACKGROUND_OPACITY_DEFAULT	   = 0.f;
	const int					  Setting::REPRESENTATION_DEFAULT_INDEX	   = 0;
	const Generic::REPRESENTATION Setting::DEFAULT_REPRESENTATION_TYPE	   = Generic::REPRESENTATION::STICK;
	const std::string			  Setting::NEW_REPRESENTATION_DEFAULT_NAME = "New representation";
	const float					  Setting::ATOMS_RADIUS_DEFAULT			   = 0.4f;
	const float					  Setting::ATOMS_RADIUS_MIN				   = 0.01f;
	const float					  Setting::ATOMS_RADIUS_MAX				   = 1.f;
	const float					  Setting::BONDS_RADIUS_DEFAULT			   = 0.15f;
	const float					  Setting::BONDS_RADIUS_MIN				   = 0.01f;
	const float					  Setting::BONDS_RADIUS_MAX				   = 1.f;
	const float					  Setting::ATOMS_RADIUS_ADD_DEFAULT		   = 0.f;
	const float					  Setting::ATOMS_RADIUS_ADD_MIN			   = -1.f;
	const float					  Setting::ATOMS_RADIUS_ADD_MAX			   = 1.f;
	const Generic::COLOR_MODE	  Setting::COLOR_MODE_DEFAULT			   = Generic::COLOR_MODE::CHAIN;

	const std::string Setting::NEW_RENDER_EFFECT_PRESET_DEFAULT_NAME = "New render preset";
	const int		  Setting::RENDER_EFFECT_DEFAULT_INDEX			 = 0;

	const Renderer::SHADING Setting::SHADING_DEFAULT		   = Renderer::SHADING::DIFFUSE;
	const bool				Setting::ACTIVE_VSYNC_DEFAULT	   = false;
	const bool				Setting::ACTIVE_AO_DEFAULT		   = true;
	const int				Setting::AO_INTENSITY_DEFAULT	   = 5;
	const int				Setting::AO_INTENSITY_MIN		   = 1;
	const int				Setting::AO_INTENSITY_MAX		   = 20;
	const int				Setting::AO_BLUR_SIZE_DEFAULT	   = 17;
	const int				Setting::AO_BLUR_SIZE_MIN		   = 1;
	const int				Setting::AO_BLUR_SIZE_MAX		   = 99;
	const bool				Setting::ACTIVE_OUTLINE_DEFAULT	   = false;
	const float				Setting::OUTLINE_THICKNESS_DEFAULT = 1.0f;
	const float				Setting::OUTLINE_THICKNESS_MIN	   = 0.1f;
	const float				Setting::OUTLINE_THICKNESS_MAX	   = 10.0f;
	const Color::Rgb		Setting::OUTLINE_COLOR_DEFAULT	   = Color::Rgb::WHITE;
	const bool				Setting::ACTIVE_FOG_DEFAULT		   = false;
	const float				Setting::FOG_NEAR_DEFAULT		   = 400.f;
	const float				Setting::FOG_NEAR_MIN			   = 0.f;
	const float				Setting::FOG_NEAR_MAX			   = 1000.f;
	const float				Setting::FOG_FAR_DEFAULT		   = 600.f;
	const float				Setting::FOG_FAR_MIN			   = 0.f;
	const float				Setting::FOG_FAR_MAX			   = 1000.f;
	const float				Setting::FOG_DENSITY_DEFAULT	   = 0.8f;
	const Color::Rgb		Setting::FOG_COLOR_DEFAULT		   = Color::Rgb::WHITE;
	const bool				Setting::ACTIVE_AA_DEFAULT		   = false;
	const Color::Rgb		Setting::LIGHT_COLOR_DEFAULT	   = Color::Rgb::WHITE;

	// Camera.
	const float Setting::CAMERA_NEAR_DEFAULT		= 0.f;
	const float Setting::CAMERA_NEAR_MIN			= 0.f;
	const float Setting::CAMERA_NEAR_MAX			= 1e4f;
	const float Setting::CAMERA_FAR_DEFAULT			= 1e4f;
	const float Setting::CAMERA_FAR_MIN				= 0.f;
	const float Setting::CAMERA_FAR_MAX				= 1e4f;
	const float Setting::CAMERA_FOV_DEFAULT			= 45.f;
	const float Setting::CAMERA_FOV_MIN				= 10.f;
	const float Setting::CAMERA_FOV_MAX				= 90.f;
	const bool	Setting::CAMERA_PERSPECTIVE_DEFAULT = true;

	// Controllers.
	const float Setting::CONTROLLER_TRANSLATION_SPEED_DEFAULT  = 150.f;
	const float Setting::CONTROLLER_TRANSLATION_SPEED_MIN	   = 50.f;
	const float Setting::CONTROLLER_TRANSLATION_SPEED_MAX	   = 300.f;
	const float Setting::CONTROLLER_TRANSLATION_FACTOR_DEFAULT = 2.0f;
	const float Setting::CONTROLLER_TRANSLATION_FACTOR_MIN	   = 1.0f;
	const float Setting::CONTROLLER_TRANSLATION_FACTOR_MAX	   = 5.0f;
	const float Setting::CONTROLLER_ROTATION_SPEED_DEFAULT	   = 0.005f;
	const float Setting::CONTROLLER_ROTATION_SPEED_MIN		   = 0.001f;
	const float Setting::CONTROLLER_ROTATION_SPEED_MAX		   = 0.01f;
	const bool	Setting::CONTROLLER_Y_AXIS_INVERTED			   = false;
	const bool	Setting::CONTROLLER_ELASTICITY_ACTIVE_DEFAULT  = true;
	const float Setting::CONTROLLER_ELASTICITY_FACTOR_DEFAULT  = 6.0f;
	const float Setting::CONTROLLER_ELASTICITY_FACTOR_MIN	   = 1.0f;
	const float Setting::CONTROLLER_ELASTICITY_FACTOR_MAX	   = 40.0f;
	const float Setting::CONTROLLER_ELASTICITY_THRESHOLD	   = 1e-4f;

	// Molecule
	const float Setting::COPIED_MOLECULE_OFFSET = 5.0f;

	// Trajectory
	const int				   Setting::MIN_TRAJECTORY_SPEED		 = 1;
	const int				   Setting::MAX_TRAJECTORY_SPEED		 = 60;
	const int				   Setting::DEFAULT_TRAJECTORY_SPEED	 = 5;
	const Trajectory::PlayMode Setting::DEFAULT_TRAJECTORY_PLAY_MODE = Trajectory::PlayMode::Loop;

	// Auto rotate.
	const float Setting::AUTO_ROTATE_SPEED_DEFAULT = 0.0f;
	const float Setting::AUTO_ROTATE_SPEED_MIN	   = 0.0f;
	const float Setting::AUTO_ROTATE_SPEED_MAX	   = 1.0f;

	// Video.
	const float Setting::PATH_DURATION_DEFAULT = 5.f;
	const uint	Setting::VIDEO_FPS_DEFAULT	   = 60;
	const uint	Setting::VIDEO_CRF_DEFAULT	   = 10;

	// Misc.
	const int  Setting::CONSOLE_SIZE	   = 80;
	const uint Setting::ACTION_BUFFER_SIZE = 10;

	// Dev.
	const Renderer::MODE Setting::MODE_DEFAULT = Renderer::MODE::GL;

	const int Setting::RECENT_PATH_SAVED_MAX_COUNT = 6;
	void	  Setting::enqueueNewLoadingPath( FilePath & p_path )
	{
		for ( std::list<FilePath>::const_iterator & itPath = recentLoadingPath.cbegin();
			  itPath != recentLoadingPath.cend();
			  itPath++ )
		{
			if ( *itPath == p_path )
			{
				recentLoadingPath.erase( itPath );
				break;
			}
		}

		recentLoadingPath.emplace_front( p_path );
		if ( recentLoadingPath.size() > RECENT_PATH_SAVED_MAX_COUNT )
			recentLoadingPath.pop_back();

		VTX_EVENT( new Event::VTXEvent( Event::Global::RECENT_FILES_CHANGE ) );
	}
	VTX::FilePath Setting::getRecentLoadingPath( const int p_index )
	{
		if ( p_index < 0 || p_index >= recentLoadingPath.size() )
			return FilePath();

		std::list<FilePath>::iterator it = recentLoadingPath.begin();

		for ( int i = 0; i < p_index; i++ )
			it++;

		return *it;
	}

	void Setting::backup()
	{
		IO::Serializer serializer = IO::Serializer();
		_backup					  = serializer.serialize( *this );
	}
	void Setting::recover() { IO::Serializer serializer = IO::Serializer(); }

} // namespace VTX
