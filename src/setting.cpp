#include "setting.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "io/reader/serialized_object.hpp"
#include "io/serializer.hpp"
#include "io/writer/serialized_object.hpp"
#include "model/representation/representation_enum.hpp"
#include "renderer/base_renderer.hpp"
#include "trajectory/trajectory_enum.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include <QSettings>
#include <exception>
#include <string>

namespace VTX
{
	// UI.
	const std::string Setting::ORGANIZATION_NAME		= "CNAM_GBCM";
	const std::string Setting::PROJECT_NAME				= "VTX";
	const std::string Setting::LAYOUT_SETTINGS_FOLDER	= PROJECT_NAME;
	const std::string Setting::LAYOUT_SETTINGS_FILENAME = "Settings";

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

	// Rendering.
	const bool					  Setting::ACTIVE_RENDERER_DEFAULT		   = true;
	const bool					  Setting::FORCE_RENDERER_DEFAULT		   = false;
	const Color::Rgb			  Setting::BACKGROUND_COLOR_DEFAULT		   = Color::Rgb::BLACK;
	const float					  Setting::BACKGROUND_OPACITY_DEFAULT	   = 1.0f;
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
	const float				Setting::OUTLINE_THICKNESS_DEFAULT = 0.1f;
	const float				Setting::OUTLINE_THICKNESS_MIN	   = 0.00001f;
	const float				Setting::OUTLINE_THICKNESS_MAX	   = 0.1;
	const Color::Rgb		Setting::OUTLINE_COLOR_DEFAULT	   = Color::Rgb::WHITE;
	const bool				Setting::ACTIVE_FOG_DEFAULT		   = false;
	const float				Setting::FOG_NEAR_DEFAULT		   = 30;
	const float				Setting::FOG_NEAR_MIN			   = 0.f;
	const float				Setting::FOG_NEAR_MAX			   = 1000.f;
	const float				Setting::FOG_FAR_DEFAULT		   = 80.f;
	const float				Setting::FOG_FAR_MIN			   = 0.f;
	const float				Setting::FOG_FAR_MAX			   = 1000.f;
	const float				Setting::FOG_DENSITY_DEFAULT	   = 0.8f;
	const Color::Rgb		Setting::FOG_COLOR_DEFAULT		   = Color::Rgb::WHITE;
	const bool				Setting::ACTIVE_AA_DEFAULT		   = true;
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
	void	  Setting::enqueueNewLoadingPath( const FilePath & p_path )
	{
		for ( std::list<const FilePath *>::const_iterator & itPath = recentLoadingPath.cbegin();
			  itPath != recentLoadingPath.cend();
			  itPath++ )
		{
			if ( **itPath == p_path )
			{
				delete *itPath;
				recentLoadingPath.erase( itPath );
				break;
			}
		}

		recentLoadingPath.emplace_front( new FilePath( p_path ) );
		if ( recentLoadingPath.size() > RECENT_PATH_SAVED_MAX_COUNT )
			recentLoadingPath.pop_back();

		saveRecentPaths();

		VTX_EVENT( new Event::VTXEvent( Event::Global::RECENT_FILES_CHANGE ) );
	}
	const VTX::FilePath * Setting::getRecentLoadingPath( const int p_index )
	{
		if ( p_index < 0 || p_index >= recentLoadingPath.size() )
			return nullptr;

		std::list<const FilePath *>::iterator it = recentLoadingPath.begin();

		for ( int i = 0; i < p_index; i++ )
			it++;

		return *it;
	}
	void Setting::cleanRecentPaths()
	{
		while ( recentLoadingPath.size() > 0 )
		{
			delete recentLoadingPath.front();
			recentLoadingPath.pop_front();
		}
	}

	void Setting::loadRecentPaths()
	{
		const QSettings settings( QSettings::Format::NativeFormat,
								  QSettings::Scope::UserScope,
								  QString::fromStdString( Setting::PROJECT_NAME ),
								  QString::fromStdString( Setting::PROJECT_NAME ) );

		int		counter = 0;
		QString key		= QString::fromStdString( "RecentLoadedPath" + std::to_string( counter ) );

		while ( settings.contains( key ) )
		{
			const std::string strPath = settings.value( key ).toString().toStdString();

			const FilePath * path = new FilePath( strPath );
			if ( Util::Filesystem::exists( *path ) )
			{
				recentLoadingPath.push_back( path );
			}
			else
			{
				delete path;
			}

			counter++;
			key = QString::fromStdString( "RecentLoadedPath" + std::to_string( counter ) );
		}
	}
	void Setting::saveRecentPaths()
	{
		QSettings settings( QSettings::Format::NativeFormat,
							QSettings::Scope::UserScope,
							QString::fromStdString( Setting::PROJECT_NAME ),
							QString::fromStdString( Setting::PROJECT_NAME ) );

		int counter = 0;
		for ( const FilePath * const path : recentLoadingPath )
		{
			const QString key = QString::fromStdString( "RecentLoadedPath" + std::to_string( counter ) );
			settings.setValue( key, QString::fromStdString( path->string() ) );

			counter++;
		}
	}

	void Setting::backup()
	{
		IO::Writer::SerializedObject<VTX::Setting> writer = IO::Writer::SerializedObject<VTX::Setting>();
		try
		{
			writer.writeFile( Util::Filesystem::SETTING_JSON_FILE, *this );
			VTX_INFO( "Settings Saved " );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Cannot save settings: " + std::string( p_e.what() ) );
		}
	}
	void Setting::recover()
	{
		IO::Reader::SerializedObject<VTX::Setting> reader = IO::Reader::SerializedObject<VTX::Setting>();
		try
		{
			reader.readFile( Util::Filesystem::SETTING_JSON_FILE, VTX_SETTING() );
			VTX_INFO( "Settings loaded " );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Cannot load settings: " + std::string( p_e.what() ) );
		}
	}

	void Setting::setWindowFullscreen( const bool p_fullscreen )
	{
		windowFullscreen = p_fullscreen;
		_sendDataChangedEvent( PARAMETER::WINDOW_FULL_SCREEN );
	}
	void Setting::setActivateRenderer( const bool p_activeRenderer )
	{
		activeRenderer = p_activeRenderer;
		_sendDataChangedEvent( PARAMETER::ACTIVATE_RENDERER );
	}
	void Setting::setForceRenderer( const bool p_forceRenderer )
	{
		forceRenderer = p_forceRenderer;
		_sendDataChangedEvent( PARAMETER::FORCE_RENDERER );
	}
	void Setting::setVSync( const bool p_activeVSync )
	{
		activeVSync = p_activeVSync;
		_sendDataChangedEvent( PARAMETER::VSYNC );
	}
	void Setting::setSnapshotBackgroundOpacity( const float p_backgroundOpacity )
	{
		backgroundOpacity = Util::Math::clamp( p_backgroundOpacity, 0.f, 1.f );
		_sendDataChangedEvent( PARAMETER::SNAPSHOT_BACKGROUND_OPACITY );
	}

	void Setting::setTranslationSpeed( const float p_translationSpeed )
	{
		translationSpeed = Util::Math::clamp(
			p_translationSpeed, CONTROLLER_TRANSLATION_SPEED_MIN, CONTROLLER_TRANSLATION_SPEED_MAX );

		_sendDataChangedEvent( PARAMETER::CONTROLLER_TRANSLATION_SPEED );
	}
	void Setting::setTranslationSpeedFactor( const float p_translationFactorSpeed )
	{
		translationFactorSpeed = Util::Math::clamp( p_translationFactorSpeed,
													VTX::Setting::CONTROLLER_TRANSLATION_FACTOR_MIN,
													VTX::Setting::CONTROLLER_TRANSLATION_FACTOR_MAX );

		_sendDataChangedEvent( PARAMETER::CONTROLLER_TRANSLATION_SPEED_FACTOR );
	}
	void Setting::setRotationSpeed( const float p_rotationSpeed )
	{
		rotationSpeed = Util::Math::clamp(
			p_rotationSpeed, VTX::Setting::CONTROLLER_ROTATION_SPEED_MIN, VTX::Setting::CONTROLLER_ROTATION_SPEED_MAX );
		_sendDataChangedEvent( PARAMETER::CONTROLLER_ROTATION_SPEED );
	}
	void Setting::setControllerElasticityActive( const bool p_activeControllerElasticity )
	{
		activeControllerElasticity = p_activeControllerElasticity;
		_sendDataChangedEvent( PARAMETER::CONTROLLER_ACTIVATE_ELASTICTY );
	}
	void Setting::setControllerElasticityFactor( const float p_controllerElasticityFactor )
	{
		controllerElasticityFactor = Util::Math::clamp( p_controllerElasticityFactor,
														VTX::Setting::CONTROLLER_ELASTICITY_FACTOR_MIN,
														VTX::Setting::CONTROLLER_ELASTICITY_FACTOR_MAX );
		;
		_sendDataChangedEvent( PARAMETER::CONTROLLER_ELASTICTY_FACTOR );
	}
	void Setting::setYAxisInverted( const bool p_yAxisInverted )
	{
		yAxisInverted = p_yAxisInverted;
		_sendDataChangedEvent( PARAMETER::CONTROLLER_Y_INVERSION );
	}

	void Setting::setDefaultRepresentationIndex( const int p_representationDefaultIndex )
	{
		representationDefaultIndex = p_representationDefaultIndex;
	}
	void Setting::setDefaultRenderEffectPresetIndex( const int p_renderEffectDefaultIndex )
	{
		renderEffectDefaultIndex = p_renderEffectDefaultIndex;
	}

	void Setting::setDefaultTrajectorySpeed( const int p_defaultTrajectorySpeed )
	{
		defaultTrajectorySpeed = Util::Math::clamp(
			p_defaultTrajectorySpeed, VTX::Setting::MIN_TRAJECTORY_SPEED, VTX::Setting::MAX_TRAJECTORY_SPEED );
		_sendDataChangedEvent( PARAMETER::TRAJECTORY_DEFAULT_SPEED );
	}
	void Setting::setDefaultTrajectoryPlayMode( const Trajectory::PlayMode p_defaultTrajectoryPlayMode )
	{
		defaultTrajectoryPlayMode = p_defaultTrajectoryPlayMode;
		_sendDataChangedEvent( PARAMETER::TRAJECTORY_DEFAULT_PLAY_MODE );
	}
	void Setting::setAutoRotationSpeed( const Vec3f p_autoRotationSpeed )
	{
		autoRotationSpeed = Util::Math::clamp(
			p_autoRotationSpeed, VTX::Setting::AUTO_ROTATE_SPEED_MIN, VTX::Setting::AUTO_ROTATE_SPEED_MAX );
		_sendDataChangedEvent( PARAMETER::AUTO_ROTATION_DEFAULT_SPEED );
	}

	void Setting::setSymbolDisplayMode( const Style::SYMBOL_DISPLAY_MODE p_symbolDisplayMode )
	{
		symbolDisplayMode = p_symbolDisplayMode;
		_sendDataChangedEvent( PARAMETER::SYMBOL_DISPLAY_MODE );
	}

	void Setting::restore()
	{
		symbolDisplayMode = SYMBOL_DISPLAY_MODE_DEFAULT;
		windowFullscreen  = WINDOW_FULLSCREEN_DEFAULT;

		activeRenderer			   = ACTIVE_RENDERER_DEFAULT;
		forceRenderer			   = FORCE_RENDERER_DEFAULT;
		representationDefaultIndex = REPRESENTATION_DEFAULT_INDEX;
		renderEffectDefaultIndex   = RENDER_EFFECT_DEFAULT_INDEX;

		activeVSync		  = ACTIVE_VSYNC_DEFAULT;
		backgroundOpacity = BACKGROUND_OPACITY_DEFAULT;

		translationSpeed	   = CONTROLLER_TRANSLATION_SPEED_DEFAULT;
		translationFactorSpeed = CONTROLLER_TRANSLATION_FACTOR_DEFAULT;
		rotationSpeed		   = CONTROLLER_ROTATION_SPEED_DEFAULT;
		yAxisInverted		   = CONTROLLER_Y_AXIS_INVERTED;

		activeControllerElasticity = CONTROLLER_ELASTICITY_ACTIVE_DEFAULT;
		controllerElasticityFactor = CONTROLLER_ELASTICITY_FACTOR_DEFAULT;

		defaultTrajectorySpeed	  = DEFAULT_TRAJECTORY_SPEED;
		defaultTrajectoryPlayMode = DEFAULT_TRAJECTORY_PLAY_MODE;

		autoRotationSpeed = Vec3f( AUTO_ROTATE_SPEED_DEFAULT );
		_sendDataChangedEvent( PARAMETER::ALL );
	}

	void Setting::_sendDataChangedEvent( const PARAMETER & p_parameter )
	{
		std::set<PARAMETER> parameters = std::set<PARAMETER>();

		if ( p_parameter == PARAMETER::ALL )
		{
			for ( int i = 0; i < int( PARAMETER::COUNT ); i++ )
			{
				parameters.emplace( PARAMETER( i ) );
			}
		}
		else
		{
			parameters.emplace( p_parameter );
		}

		VTX_EVENT( new Event::VTXEventRef( Event::Global::SETTINGS_CHANGE, parameters ) );
	}

} // namespace VTX
