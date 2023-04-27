#include "app/old_app/setting.hpp"
#include "app/application/representation/enum_representation.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/component/chemistry/enum_trajectory.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/internal/chemdb/category.hpp"
#include "app/old_app/define.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/old_app/io/reader/serialized_object.hpp"
#include "app/old_app/io/serializer.hpp"
#include "app/old_app/io/struct/image_export.hpp"
#include "app/old_app/io/writer/serialized_object.hpp"
#include "app/old_app/path/path_enum.hpp"
#include "app/old_app/renderer/base_renderer.hpp"
#include "app/old_app/selection/selection_enum.hpp"
#include "app/old_app/vtx_app.hpp"
#include <exception>
#include <string>
#include <util/types.hpp>

namespace VTX
{
	// UI.
	const Style::SYMBOL_DISPLAY_MODE Setting::SYMBOL_DISPLAY_MODE_DEFAULT = Style::SYMBOL_DISPLAY_MODE::SHORT;
	const bool						 Setting::WINDOW_FULLSCREEN_DEFAULT	  = false;

	// Rendering.
	const bool Setting::ACTIVE_RENDERER_DEFAULT = true;
#ifdef VTX_PRODUCTION
	const bool Setting::FORCE_RENDERER_DEFAULT = false;
#else
	const bool Setting::FORCE_RENDERER_DEFAULT = true;
#endif
	const Color::Rgba Setting::BACKGROUND_COLOR_DEFAULT	  = Color::Rgba::BLACK;
	const float		  Setting::BACKGROUND_OPACITY_DEFAULT = 1.0f;
	const float		  Setting::SNAPSHOT_QUALITY_DEFAULT	  = 1.0f;

	const IO::Struct::ImageExport::RESOLUTION Setting::SNAPSHOT_RESOLUTION_DEFAULT
		= IO::Struct::ImageExport::RESOLUTION::Free;
	const IO::Struct::ImageExport::Format Setting::SNAPSHOT_FORMAT_DEFAULT = IO::Struct::ImageExport::Format::PNG;

	const std::string											Setting::REPRESENTATION_DEFAULT_NAME  = "Stick";
	const int													Setting::REPRESENTATION_DEFAULT_INDEX = 4;
	const App::Application::Representation::REPRESENTATION_ENUM Setting::DEFAULT_REPRESENTATION_TYPE
		= App::Application::Representation::REPRESENTATION_ENUM::STICK;
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
		5, // POLYMER
		5, // CARBOHYDRATE
		5, // LIGAND
		6, // ION
		5, // SOLVENT
		5, // WATER
		5, // UNKNOWN
	};

	const std::string Setting::NEW_RENDER_EFFECT_PRESET_DEFAULT_NAME = "New render preset";
	const int		  Setting::RENDER_EFFECT_DEFAULT_INDEX			 = 0;

	const Renderer::SHADING Setting::SHADING_DEFAULT = Renderer::SHADING::DIFFUSE;
#ifdef VTX_PRODUCTION
	const bool Setting::ACTIVE_VSYNC_DEFAULT = true;
#else
	const bool Setting::ACTIVE_VSYNC_DEFAULT   = false;
#endif
	const bool		  Setting::ACTIVE_AO_DEFAULT		 = true;
	const int		  Setting::AO_INTENSITY_DEFAULT		 = 5;
	const int		  Setting::AO_INTENSITY_MIN			 = 1;
	const int		  Setting::AO_INTENSITY_MAX			 = 20;
	const int		  Setting::AO_BLUR_SIZE_DEFAULT		 = 17;
	const int		  Setting::AO_BLUR_SIZE_MIN			 = 1;
	const int		  Setting::AO_BLUR_SIZE_MAX			 = 99;
	const bool		  Setting::ACTIVE_OUTLINE_DEFAULT	 = false;
	const uint		  Setting::OUTLINE_THICKNESS_DEFAULT = 1;
	const uint		  Setting::OUTLINE_THICKNESS_MIN	 = 1;
	const uint		  Setting::OUTLINE_THICKNESS_MAX	 = 5;
	const float		  Setting::OUTLINE_SENSIVITY_DEFAULT = 0.4f;
	const float		  Setting::OUTLINE_SENSIVITY_MIN	 = 0.01f;
	const float		  Setting::OUTLINE_SENSIVITY_MAX	 = 1.f;
	const Color::Rgba Setting::OUTLINE_COLOR_DEFAULT	 = Color::Rgba::WHITE;
	const bool		  Setting::ACTIVE_FOG_DEFAULT		 = false;
	const float		  Setting::FOG_NEAR_DEFAULT			 = 30;
	const float		  Setting::FOG_NEAR_MIN				 = 0.f;
	const float		  Setting::FOG_NEAR_MAX				 = 1000.f;
	const float		  Setting::FOG_FAR_DEFAULT			 = 80.f;
	const float		  Setting::FOG_FAR_MIN				 = 0.f;
	const float		  Setting::FOG_FAR_MAX				 = 1000.f;
	const float		  Setting::FOG_DENSITY_DEFAULT		 = 0.8f;
	const Color::Rgba Setting::FOG_COLOR_DEFAULT		 = Color::Rgba::WHITE;
	const bool		  Setting::ACTIVE_AA_DEFAULT		 = true;
	const Color::Rgba Setting::LIGHT_COLOR_DEFAULT		 = Color::Rgba::WHITE;

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

	// Controllers.
	const float Setting::CONTROLLER_TRANSLATION_SPEED_DEFAULT	= 150.f;
	const float Setting::CONTROLLER_TRANSLATION_SPEED_MIN		= 50.f;
	const float Setting::CONTROLLER_TRANSLATION_SPEED_MAX		= 300.f;
	const float Setting::CONTROLLER_ACCELERATION_FACTOR_DEFAULT = 2.0f;
	const float Setting::CONTROLLER_ACCELERATION_FACTOR_MIN		= 1.0f;
	const float Setting::CONTROLLER_ACCELERATION_FACTOR_MAX		= 20.0f;
	const float Setting::CONTROLLER_DECELERATION_FACTOR_DEFAULT = 10.0f;
	const float Setting::CONTROLLER_DECELERATION_FACTOR_MIN		= 1.0f;
	const float Setting::CONTROLLER_DECELERATION_FACTOR_MAX		= 50.0f;
	const float Setting::CONTROLLER_ROTATION_SPEED_DEFAULT		= 0.005f;
	const float Setting::CONTROLLER_ROTATION_SPEED_MIN			= 0.001f;
	const float Setting::CONTROLLER_ROTATION_SPEED_MAX			= 0.01f;
	const bool	Setting::CONTROLLER_Y_AXIS_INVERTED				= false;
	const bool	Setting::CONTROLLER_ELASTICITY_ACTIVE_DEFAULT	= true;
	const float Setting::CONTROLLER_ELASTICITY_FACTOR_DEFAULT	= 6.0f;
	const float Setting::CONTROLLER_ELASTICITY_FACTOR_MIN		= 1.0f;
	const float Setting::CONTROLLER_ELASTICITY_FACTOR_MAX		= 40.0f;
	const float Setting::CONTROLLER_ELASTICITY_THRESHOLD		= 1e-4f;

	// Molecule
	const float Setting::COPIED_MOLECULE_OFFSET = 5.0f;

	// Path
	const Path::DURATION_MODE	   Setting::DEFAULT_PATH_DURATION_MODE		= VTX::Path::DURATION_MODE::CONSTANT_SPEED;
	const Path::INTERPOLATION_MODE Setting::DEFAULT_PATH_INTERPOLATION_MODE = Path::INTERPOLATION_MODE::CATMULL_ROM;

	// Trajectory
	const int								  Setting::MIN_TRAJECTORY_SPEED		= 1;
	const int								  Setting::MAX_TRAJECTORY_SPEED		= 60;
	const int								  Setting::DEFAULT_TRAJECTORY_SPEED = 5;
	const App::Component::Chemistry::PlayMode Setting::DEFAULT_TRAJECTORY_PLAY_MODE
		= App::Component::Chemistry::PlayMode::Loop;

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

	// Video.
	const float		 Setting::PATH_DURATION_DEFAULT	  = 5.f;
	const ID::VTX_ID Setting::CONTROLLER_MODE_DEFAULT = ID::Controller::TRACKBALL;
	const ID::VTX_ID Setting::PICKER_MODE_DEFAULT	  = ID::Controller::PICKER;
	const uint		 Setting::VIDEO_FPS_DEFAULT		  = 60;
	const uint		 Setting::VIDEO_CRF_DEFAULT		  = 10;

	// Measurement
	const Color::Rgba Setting::DEFAULT_LABEL_COLOR = Color::Rgba( 0.f, 0.f, 1.f );

	// Selection
	const VTX::Selection::Granularity Setting::SELECTION_GRANULARITY_DEFAULT = VTX::Selection::Granularity::RESIDUE;

	// Structural Alignment
	const int	Setting::CE_ALIGN_WIN_SIZE_DEFAULT = 8;
	const int	Setting::CE_ALIGN_GAP_MAX_DEFAULT  = 30;
	const int	Setting::CE_ALIGN_MAX_PATH_DEFAULT = 20;
	const float Setting::CE_ALIGN_D0_DEFAULT	   = 3.f;
	const float Setting::CE_ALIGN_D1_DEFAULT	   = 4.f;

	// Misc.
	const uint Setting::ACTION_BUFFER_SIZE = 10;

	const bool	Setting::COMPUTE_BOND_ORDER_ON_CHEMFILE	 = false;
	const bool	Setting::CHECK_VTX_UPDATE_DEFAULT		 = true;
	const bool	Setting::PORTABLE_SAVE_ACTIVATED_DEFAULT = false;
	const float Setting::CELL_LIST_CUBE_SIZE			 = 8.f;

	// Dev.
	const Renderer::MODE Setting::MODE_DEFAULT = Renderer::MODE::GL;

	const int Setting::RECENT_PATH_SAVED_MAX_COUNT			= 10;
	const int Setting::RECENT_DOWNLOAD_CODE_SAVED_MAX_COUNT = 20;

	void Setting::enqueueNewLoadingPath( const FilePath & p_path )
	{
		for ( std::list<FilePath>::const_iterator itPath = recentLoadingPath.cbegin();
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

		saveRecentPaths();

		VTX_EVENT( VTX::App::Event::Global::RECENT_FILES_CHANGE );
	}
	const FilePath * const Setting::getRecentLoadingPath( const int p_index )
	{
		if ( p_index < 0 || p_index >= recentLoadingPath.size() )
			return nullptr;

		std::list<FilePath>::iterator it = recentLoadingPath.begin();

		for ( int i = 0; i < p_index; i++ )
			it++;

		return &( *it );
	}

	void Setting::enqueueNewDownloadCode( const std::string & p_code )
	{
		for ( std::list<std::string>::const_iterator itCode = recentDownloadCodes.cbegin();
			  itCode != recentDownloadCodes.cend();
			  itCode++ )
		{
			if ( *itCode == p_code )
			{
				recentDownloadCodes.erase( itCode );
				break;
			}
		}

		recentDownloadCodes.emplace_front( p_code );
		if ( recentDownloadCodes.size() > RECENT_DOWNLOAD_CODE_SAVED_MAX_COUNT )
			recentDownloadCodes.pop_back();

		saveRecentPaths();

		VTX_EVENT( VTX::App::Event::Global::RECENT_DOWNLOAD_CODE_CHANGE );
	}
	const std::string * const Setting::getRecentDownloadCode( const int p_index )
	{
		if ( p_index < 0 || p_index >= recentDownloadCodes.size() )
			return nullptr;

		std::list<std::string>::iterator it = recentDownloadCodes.begin();

		for ( int i = 0; i < p_index; i++ )
			it++;

		return &( *it );
	}

	// TODO reimplement that without Qt
	std::string RegisterMap::getStringValue( const std::string & p_key, const std::string & p_default )
	{
		return "";
		// const QSettings settings( QSettings::Format::NativeFormat,
		//						  QSettings::Scope::UserScope,
		//						  QString::fromStdString( VTX_PROJECT_NAME ),
		//						  QString::fromStdString( VTX_PROJECT_NAME ) );

		// return settings.value( p_key, p_default ).toString().toStdString();
	}
	void RegisterMap::setStringValue( const std::string & p_key, const std::string & p_value )
	{
		// QSettings settings( QSettings::Format::NativeFormat,
		//					QSettings::Scope::UserScope,
		//					QString::fromStdString( VTX_PROJECT_NAME ),
		//					QString::fromStdString( VTX_PROJECT_NAME ) );

		// settings.setValue( QString::fromStdString( p_key ), p_value );
	}
	bool RegisterMap::containKey( const std::string & p_key )
	{
		return false;

		// QSettings settings( QSettings::Format::NativeFormat,
		//					QSettings::Scope::UserScope,
		//					QString::fromStdString( VTX_PROJECT_NAME ),
		//					QString::fromStdString( VTX_PROJECT_NAME ) );

		// return settings.contains( QString::fromStdString( p_key ) );
	}

	void Setting::loadRecentPaths()
	{
		int			counter = 0;
		std::string key		= RegisterKey::RECENT_LOADED_PATH_PREFIX + std::to_string( counter );

		while ( RegisterMap::containKey( key ) )
		{
			const std::string strPath = RegisterMap::getStringValue( key );

			const FilePath path = FilePath( strPath );
			if ( std::filesystem::exists( path ) )
			{
				recentLoadingPath.push_back( path );
			}

			counter++;
			key = RegisterKey::RECENT_LOADED_PATH_PREFIX + std::to_string( counter );
		}

		counter = 0;
		key		= RegisterKey::RECENT_DOWNLOADED_CODE_PREFIX + std::to_string( counter );

		while ( RegisterMap::containKey( key ) )
		{
			const std::string code = RegisterMap::getStringValue( key );
			recentDownloadCodes.push_back( code );

			counter++;
			key = RegisterKey::RECENT_DOWNLOADED_CODE_PREFIX + std::to_string( counter );
		}
	}
	void Setting::saveRecentPaths()
	{
		int counter = 0;
		for ( const FilePath & path : recentLoadingPath )
		{
			const std::string key = RegisterKey::RECENT_LOADED_PATH_PREFIX + std::to_string( counter );
			RegisterMap::setStringValue( key, path.string() );

			counter++;
		}

		counter = 0;
		for ( const std::string & code : recentDownloadCodes )
		{
			const std::string key = RegisterKey::RECENT_DOWNLOADED_CODE_PREFIX + std::to_string( counter );
			RegisterMap::setStringValue( key, code );

			counter++;
		}
	}

	std::string Setting::getLastLoadedSessionFolder()
	{
		return _getFileInRegisterKey( RegisterKey::LAST_OPEN_SESSION_FOLDER,
									  IO::Filesystem::DEFAULT_SAVE_FOLDER.string() );
	}
	void Setting::saveLastLoadedSessionFolder( const std::string & p_path )
	{
		RegisterMap::setStringValue( RegisterKey::LAST_OPEN_SESSION_FOLDER, p_path );
	}

	std::string Setting::getLastSavedSessionFolder()
	{
		return _getFileInRegisterKey( RegisterKey::LAST_SAVED_SESSION_FOLDER,
									  IO::Filesystem::DEFAULT_SAVE_FOLDER.string() );
	}
	void Setting::saveLastSavedSessionFolder( const std::string & p_path )
	{
		RegisterMap::setStringValue( RegisterKey::LAST_SAVED_SESSION_FOLDER, p_path );
	}

	std::string Setting::getLastImportedMoleculeFolder()
	{
		return _getFileInRegisterKey( RegisterKey::LAST_IMPORTED_MOLECULE_FOLDER,
									  IO::Filesystem::DEFAULT_MOLECULE_FOLDER.string() );
	}
	void Setting::saveLastImportedMoleculeFolder( const std::string & p_path )
	{
		RegisterMap::setStringValue( RegisterKey::LAST_IMPORTED_MOLECULE_FOLDER, p_path );
	}

	std::string Setting::getLastExportedMoleculeFolder()
	{
		return _getFileInRegisterKey( RegisterKey::LAST_EXPORTED_MOLECULE_FOLDER,
									  IO::Filesystem::DEFAULT_MOLECULE_FOLDER.string() );
	}
	void Setting::saveLastExportedMoleculeFolder( const std::string & p_path )
	{
		RegisterMap::setStringValue( RegisterKey::LAST_EXPORTED_MOLECULE_FOLDER, p_path );
	}

	std::string Setting::getLastExportedImageFolder()
	{
		return _getFileInRegisterKey( RegisterKey::LAST_EXPORTED_IMAGE_FOLDER,
									  IO::Filesystem::getSnapshotsDir().string() );
	}
	void Setting::saveLastExportedImageFolder( const std::string & p_path )
	{
		RegisterMap::setStringValue( RegisterKey::LAST_EXPORTED_IMAGE_FOLDER, p_path );
	}

	std::string Setting::_getFileInRegisterKey( const std::string & p_key, const std::string & p_default )
	{
		const FilePath path = RegisterMap::getStringValue( p_key, p_default );

		if ( path.empty() || std::filesystem::exists( path ) == false )
			return p_default;

		return path.string();
	}

	void Setting::restoreDefaultRepresentationPerCategory()
	{
		representationPerCategory = DEFAULT_REPRESENTATION_PER_CATEGORY_INDEX;
		_sendDataChangedEvent( PARAMETER::DEFAULT_REPRESENTATION_PER_CATEGORY );
	}

	void Setting::backup()
	{
		IO::Writer::SerializedObject<VTX::Setting> writer = IO::Writer::SerializedObject<VTX::Setting>();
		try
		{
			writer.writeFile( IO::Filesystem::getSettingJsonFile(), *this );
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
			reader.readFile( IO::Filesystem::getSettingJsonFile(), VTX_SETTING() );
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

	void Setting::setSnapshotFormat( const IO::Struct::ImageExport::Format p_format )
	{
		snapshotFormat = p_format;
		_sendDataChangedEvent( PARAMETER::SNAPSHOT_FORMAT );
	}

	void Setting::setSnapshotBackgroundOpacity( const float p_backgroundOpacity )
	{
		backgroundOpacity = Util::Math::clamp( p_backgroundOpacity, 0.f, 1.f );
		_sendDataChangedEvent( PARAMETER::SNAPSHOT_BACKGROUND_OPACITY );
	}
	void Setting::setSnapshotQuality( const float p_quality )
	{
		snapshotQuality = Util::Math::clamp( p_quality, 0.f, 1.f );
		_sendDataChangedEvent( PARAMETER::SNAPSHOT_QUALITY );
	}
	void Setting::setSnapshotResolution( const IO::Struct::ImageExport::RESOLUTION & p_snapshotResolution )
	{
		snapshotResolution = p_snapshotResolution;
		_sendDataChangedEvent( PARAMETER::SNAPSHOT_RESOLUTION );
	}

	// Camera Settings
	void Setting::setCameraFOV( const float p_cameraFOV )
	{
		cameraFOV = p_cameraFOV;
		_sendDataChangedEvent( PARAMETER::CAMERA_FOV );
	}
	void Setting::setCameraNearClip( const float p_cameraNearClip )
	{
		cameraNearClip = p_cameraNearClip;
		_sendDataChangedEvent( PARAMETER::CAMERA_NEAR_CLIP );
	}
	void Setting::setCameraFarClip( const float p_cameraFarClip )
	{
		cameraFarClip = p_cameraFarClip;
		_sendDataChangedEvent( PARAMETER::CAMERA_FAR_CLIP );
	}
	void Setting::setAA( const bool p_antiAliasing )
	{
		antiAliasing = p_antiAliasing;
		_sendDataChangedEvent( PARAMETER::CAMERA_ANTI_ALIASING );
	}
	void Setting::setCameraPerspectiveProjection( const bool p_cameraPerspective )
	{
		cameraPerspective = p_cameraPerspective;
		_sendDataChangedEvent( PARAMETER::CAMERA_PROJECTION );
	}

	void Setting::setTranslationSpeed( const float p_translationSpeed )
	{
		translationSpeed = Util::Math::clamp(
			p_translationSpeed, CONTROLLER_TRANSLATION_SPEED_MIN, CONTROLLER_TRANSLATION_SPEED_MAX );

		_sendDataChangedEvent( PARAMETER::CONTROLLER_TRANSLATION_SPEED );
	}
	void Setting::setAccelerationSpeedFactor( const float p_factorSpeed )
	{
		accelerationFactorSpeed = Util::Math::clamp( p_factorSpeed,
													 VTX::Setting::CONTROLLER_ACCELERATION_FACTOR_MIN,
													 VTX::Setting::CONTROLLER_ACCELERATION_FACTOR_MAX );

		_sendDataChangedEvent( PARAMETER::CONTROLLER_TRANSLATION_SPEED_FACTOR );
	}
	void Setting::setDecelerationSpeedFactor( const float p_factorSpeed )
	{
		decelerationFactorSpeed = Util::Math::clamp( p_factorSpeed,
													 VTX::Setting::CONTROLLER_DECELERATION_FACTOR_MIN,
													 VTX::Setting::CONTROLLER_DECELERATION_FACTOR_MAX );

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

	int Setting::getDefaultRepresentationIndexPerCategory(
		const App::Internal::ChemDB::Category::TYPE & p_categoryEnum ) const
	{
		return representationPerCategory[ int( p_categoryEnum ) ];
	}
	void Setting::setDefaultRepresentationIndexPerCategory(
		const App::Internal::ChemDB::Category::TYPE & p_categoryEnum,
		const int									  p_representationDefaultIndex )
	{
		representationPerCategory[ int( p_categoryEnum ) ] = p_representationDefaultIndex;
		_sendDataChangedEvent( PARAMETER::DEFAULT_REPRESENTATION_PER_CATEGORY );
	}
	const std::string & Setting::getTmpDefaultRepresentationNamePerCategory(
		const App::Internal::ChemDB::Category::TYPE & p_categoryEnum )
	{
		return _tmpRepresentationPerCategory[ int( p_categoryEnum ) ];
	}

	void Setting::setTmpDefaultRepresentationNamePerCategory(
		const App::Internal::ChemDB::Category::TYPE & p_categoryEnum,
		const std::string &							  p_representationDefaultName )
	{
		_tmpRepresentationPerCategory[ int( p_categoryEnum ) ] = p_representationDefaultName;
	}

	void Setting::setSelectionGranularity( const Selection::Granularity & p_selectionGranularity )
	{
		selectionGranularity = p_selectionGranularity;
		_sendDataChangedEvent( PARAMETER::SELECTION_GRANULARITY );
	}

	void Setting::setDefaultTrajectorySpeed( const int p_defaultTrajectorySpeed )
	{
		defaultTrajectorySpeed = Util::Math::clamp(
			p_defaultTrajectorySpeed, VTX::Setting::MIN_TRAJECTORY_SPEED, VTX::Setting::MAX_TRAJECTORY_SPEED );
		_sendDataChangedEvent( PARAMETER::TRAJECTORY_DEFAULT_SPEED );
	}
	void Setting::setDefaultTrajectoryPlayMode( const App::Component::Chemistry::PlayMode p_defaultTrajectoryPlayMode )
	{
		defaultTrajectoryPlayMode = p_defaultTrajectoryPlayMode;
		_sendDataChangedEvent( PARAMETER::TRAJECTORY_DEFAULT_PLAY_MODE );
	}

	void Setting::setSymbolDisplayMode( const Style::SYMBOL_DISPLAY_MODE p_symbolDisplayMode )
	{
		symbolDisplayMode = p_symbolDisplayMode;
		_sendDataChangedEvent( PARAMETER::SYMBOL_DISPLAY_MODE );
	}

	void Setting::setCheckVTXUpdateAtLaunch( const bool p_checkVTXUpdate )
	{
		checkVTXUpdate = p_checkVTXUpdate;
		_sendDataChangedEvent( PARAMETER::CHECK_VTX_UPDATE );
	}

	void Setting::activatePortableSave( const bool p_activate )
	{
		portableSaveActivated = p_activate;
		_sendDataChangedEvent( PARAMETER::PORTABLE_SAVE_ACTIVATED );
	}

	void Setting::restore()
	{
		symbolDisplayMode = SYMBOL_DISPLAY_MODE_DEFAULT;
		windowFullscreen  = WINDOW_FULLSCREEN_DEFAULT;

		activeRenderer = ACTIVE_RENDERER_DEFAULT;
		forceRenderer  = FORCE_RENDERER_DEFAULT;
		representationDefaultIndex
			= App::Application::Representation::RepresentationManager::get().getDefaultRepresentationIndex();
		renderEffectDefaultIndex = RENDER_EFFECT_DEFAULT_INDEX;

		activeVSync = ACTIVE_VSYNC_DEFAULT;

		cameraFOV		  = CAMERA_FOV_DEFAULT;
		cameraNearClip	  = CAMERA_NEAR_DEFAULT;
		cameraFarClip	  = CAMERA_FAR_DEFAULT;
		antiAliasing	  = ACTIVE_AA_DEFAULT;
		cameraPerspective = CAMERA_PERSPECTIVE_DEFAULT;

		snapshotFormat	   = SNAPSHOT_FORMAT_DEFAULT;
		backgroundOpacity  = BACKGROUND_OPACITY_DEFAULT;
		snapshotQuality	   = SNAPSHOT_QUALITY_DEFAULT;
		snapshotResolution = SNAPSHOT_RESOLUTION_DEFAULT;

		translationSpeed		= CONTROLLER_TRANSLATION_SPEED_DEFAULT;
		accelerationFactorSpeed = CONTROLLER_ACCELERATION_FACTOR_DEFAULT;
		decelerationFactorSpeed = CONTROLLER_DECELERATION_FACTOR_DEFAULT;
		rotationSpeed			= CONTROLLER_ROTATION_SPEED_DEFAULT;
		yAxisInverted			= CONTROLLER_Y_AXIS_INVERTED;

		activeControllerElasticity = CONTROLLER_ELASTICITY_ACTIVE_DEFAULT;
		controllerElasticityFactor = CONTROLLER_ELASTICITY_FACTOR_DEFAULT;

		defaultTrajectorySpeed	  = DEFAULT_TRAJECTORY_SPEED;
		defaultTrajectoryPlayMode = DEFAULT_TRAJECTORY_PLAY_MODE;

		checkVTXUpdate		  = CHECK_VTX_UPDATE_DEFAULT;
		portableSaveActivated = PORTABLE_SAVE_ACTIVATED_DEFAULT;

		representationPerCategory = DEFAULT_REPRESENTATION_PER_CATEGORY_INDEX;

		_sendDataChangedEvent( PARAMETER::ALL );
	}

	void Setting::_sendDataChangedEvent( const PARAMETER & p_parameter )
	{
		if ( _freezeEvent )
			return;

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

		VTX_EVENT<const std::set<PARAMETER> &>( VTX::App::Event::Global::SETTINGS_CHANGE, parameters );
	}

} // namespace VTX
