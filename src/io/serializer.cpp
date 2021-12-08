#include "serializer.hpp"
#include "action/main.hpp"
#include "action/renderer.hpp"
#include "action/setting.hpp"
#include "define.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "generic/base_colorable.hpp"
#include "io/reader/lib_chemfiles.hpp"
#include "io/struct/image_export.hpp"
#include "io/struct/scene_path_data.hpp"
#include "model/chain.hpp"
#include "model/configuration/molecule.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/residue.hpp"
#include "model/viewpoint.hpp"
#include "mvc/mvc_manager.hpp"
#include "representation/representation_manager.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "trajectory/trajectory_enum.hpp"
#include "worker/base_thread.hpp"
#include <algorithm>
#include <magic_enum.hpp>

namespace VTX::IO
{
	Serializer::Serializer( const Worker::BaseThread * const p_thread ) : _thread( p_thread ) {}

	nlohmann::json Serializer::serialize( const VTXApp & p_app ) const
	{
		return nlohmann::json { { "SCENE", serialize( p_app.getScene() ) } };
	}

	nlohmann::json Serializer::serialize( const Object3D::Scene & p_scene ) const
	{
		nlohmann::json jsonArrayMolecules = nlohmann::json::array();
		for ( const Object3D::Scene::PairMoleculePtrFloat & pair : p_scene.getMolecules() )
		{
			nlohmann::json json
				= { { "INDEX", pair.first->getConfiguration().sceneIndex }, { "MOLECULE", serialize( *pair.first ) } };

			jsonArrayMolecules.emplace_back( json );
		}

		nlohmann::json jsonArrayPaths = nlohmann::json::array();
		for ( const Model::Path * const path : p_scene.getPaths() )
		{
			jsonArrayPaths.emplace_back( serialize( *path ) );
		}

		return { { "CAMERA_POSITION", serialize( p_scene.getCamera().getPosition() ) },
				 { "CAMERA_ROTATION", serialize( p_scene.getCamera().getRotation() ) },
				 { "MOLECULE_COUNT", p_scene.getMolecules().size() },
				 { "MOLECULES", jsonArrayMolecules },
				 { "PATHS", jsonArrayPaths } };
	}

	nlohmann::json Serializer::serialize( const Model::Molecule & p_molecule ) const
	{
		const IO::FilePath moleculePath = VTXApp::get().getScenePathData().getFilepath( &p_molecule );

		const Writer::ChemfilesWriter * const writer
			= VTXApp::get().getScenePathData().getData( &p_molecule ).getWriter();

		return { { "TRANSFORM", serialize( p_molecule.getTransform() ) },
				 { "PATH", moleculePath.path() },
				 { "REPRESENTATIONS", _serializeMoleculeRepresentations( p_molecule, writer ) },
				 { "VISIBILITIES", _serializeMoleculeVisibilities( p_molecule, writer ) },
				 { "NAME", p_molecule.getName() },
				 { "PDB_ID", p_molecule.getPdbIdCode() },
				 { "DISPLAY_NAME", p_molecule.getDisplayName() },
				 { "CURRENT_FRAME", p_molecule.getFrame() },
				 { "TRAJECTORY_FPS", p_molecule.getFPS() },
				 { "TRAJECTORY_PLAYMODE", magic_enum::enum_name( p_molecule.getPlayMode() ) },
				 { "TRAJECTORY_ISPLAYING", p_molecule.isPlaying() } };
	}

	nlohmann::json Serializer::serialize( const Model::Path & p_path ) const
	{
		nlohmann::json jsonArray = nlohmann::json::array();
		for ( const Model::Viewpoint * const viewpoint : p_path.getViewpoints() )
		{
			jsonArray.emplace_back( serialize( *viewpoint ) );
		}

		return { { "NAME", p_path.getName() },
				 { "MODE_DURATION", magic_enum::enum_name( p_path.getDurationMode() ) },
				 { "MODE_INTERPOLATION", magic_enum::enum_name( p_path.getInterpolationMode() ) },
				 { "DURATION", p_path.getDuration() },
				 { "IS_LOOPING", p_path.isLooping() },
				 { "VIEWPOINTS", jsonArray } };
	}

	nlohmann::json Serializer::serialize( const Model::Viewpoint & p_viewpoint ) const
	{
		nlohmann::json json = { { "NAME", p_viewpoint.getDefaultName() },
								{ "DURATION", p_viewpoint.getDuration() },
								{ "POSITION", serialize( p_viewpoint.getPosition() ) },
								{ "ROTATION", serialize( p_viewpoint.getRotation() ) },
								{ "CONTROLLER", p_viewpoint.getController() } };

		if ( p_viewpoint.getController() == ID::Controller::TRACKBALL )
		{
			json[ "TARGET" ]   = serialize( p_viewpoint.getTarget() );
			json[ "DISTANCE" ] = p_viewpoint.getDistance();
		}
		return json;
	}

	nlohmann::json Serializer::serialize(
		const Model::Representation::InstantiatedRepresentation & p_representation ) const
	{
		nlohmann::json json = nlohmann::json();

		const Model::Representation::Representation * const linkedRep = p_representation.getLinkedRepresentation();
		const int presetIndex = Model::Representation::RepresentationLibrary::get().getRepresentationIndex( linkedRep );

		json[ "REPRESENTATION_PRESET_INDEX" ] = presetIndex;

		if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_ADD ) )
			json[ "SPHERE_RADIUS_ADD" ] = p_representation.getSphereData()._radiusAdd;
		if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED ) )
			json[ "SPHERE_RADIUS_FIXED" ] = p_representation.getSphereData()._radiusFixed;
		if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS ) )
			json[ "CYLINDER_RADIUS" ] = p_representation.getCylinderData()._radius;
		if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::COLOR_MODE ) )
			json[ "COLOR_MODE" ] = magic_enum::enum_name( p_representation.getColorMode() );
		if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::SS_COLOR_MODE ) )
			json[ "SS_COLOR_MODE" ] = magic_enum::enum_name( p_representation.getSecondaryStructureColorMode() );
		if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::COLOR ) )
			json[ "COLOR" ] = serialize( p_representation.getColor() );

		return json;
	}

	nlohmann::json Serializer::serialize( const Model::Representation::Representation & p_representation ) const
	{
		return {
			{ "COLOR", serialize( p_representation.getColor() ) },
			{ "QUICK_ACCESS", p_representation.hasQuickAccess() },
			{ "TYPE", magic_enum::enum_name( p_representation.getRepresentationType() ) },
			{ "SPHERE_RADIUS", p_representation.getData().getSphereRadius() },
			{ "CYLINDER_RADIUS", p_representation.getData().getCylinderRadius() },
			{ "COLOR_MODE", magic_enum::enum_name( p_representation.getData().getColorMode() ) },
			{ "SS_COLOR_MODE", magic_enum::enum_name( p_representation.getData().getSecondaryStructureColorMode() ) },
		};
	}
	nlohmann::json Serializer::serialize( const Model::Renderer::RenderEffectPreset & p_preset ) const
	{
		return {
			{ "QUICK_ACCESS", p_preset.hasQuickAccess() },
			{ "SHADING", magic_enum::enum_name( p_preset.getShading() ) },
			{ "SSAO", p_preset.isSSAOEnabled() },
			{ "SSAO_INTENSITY", p_preset.getSSAOIntensity() },
			{ "SSAO_BLUR_SIZE", p_preset.getSSAOBlurSize() },
			{ "OUTLINE", p_preset.isOutlineEnabled() },
			{ "OUTLINE_THICKNESS", p_preset.getOutlineThickness() },
			{ "OUTLINE_SENSIVITY", p_preset.getOutlineSensivity() },
			{ "OUTLINE_COLOR", serialize( p_preset.getOutlineColor() ) },
			{ "FOG", p_preset.isFogEnabled() },
			{ "FOG_NEAR", p_preset.getFogNear() },
			{ "FOG_FAR", p_preset.getFogFar() },
			{ "FOG_DENSITY", p_preset.getFogDensity() },
			{ "FOG_COLOR", serialize( p_preset.getFogColor() ) },
			{ "BACKGROUND_COLOR", serialize( p_preset.getBackgroundColor() ) },
			{ "CAMERA_LIGHT_COLOR", serialize( p_preset.getCameraLightColor() ) },
			{ "CAMERA_FOV", p_preset.getCameraFOV() },
			{ "CAMERA_NEAR_CLIP", p_preset.getCameraNearClip() },
			{ "CAMERA_FAR_CLIP", p_preset.getCameraFarClip() },
			{ "CAMERA_AA", p_preset.getAA() },
			{ "CAMERA_PERSPECTIVE_PROJECTION", p_preset.isPerspectiveProjection() },
		};
	}

	nlohmann::json Serializer::serialize( const Color::Rgb & p_color ) const
	{
		return { { "R", p_color.getR() }, { "G", p_color.getG() }, { "B", p_color.getB() } };
	}

	nlohmann::json Serializer::serialize( const Math::Transform & p_transform ) const
	{
		return { { "POSITION", serialize( p_transform.getTranslationVector() ) },
				 { "ROTATION", serialize( p_transform.getEulerAngles() ) },
				 { "SCALE", serialize( p_transform.getScaleVector() ) } };
	}

	template<typename T, glm::qualifier Q>
	nlohmann::json Serializer::serialize( const glm::vec<2, T, Q> & p_vec ) const
	{
		return nlohmann::json( { { "X", p_vec.x }, { "Y", p_vec.y } } );
	}
	template<typename T, glm::qualifier Q>
	nlohmann::json Serializer::serialize( const glm::vec<3, T, Q> & p_vec ) const
	{
		return nlohmann::json( { { "X", p_vec.x }, { "Y", p_vec.y }, { "Z", p_vec.z } } );
	}
	template<typename T, glm::qualifier Q>
	nlohmann::json Serializer::serialize( const glm::vec<4, T, Q> & p_vec ) const
	{
		return nlohmann::json( { { "X", p_vec.x }, { "Y", p_vec.y }, { "Z", p_vec.z }, { "W", p_vec.w } } );
	}

	template<typename T, glm::qualifier Q>
	nlohmann::json Serializer::serialize( const glm::qua<T, Q> & p_quat ) const
	{
		return nlohmann::json( { { "X", p_quat.x }, { "Y", p_quat.y }, { "Z", p_quat.z }, { "W", p_quat.w } } );
	}

	nlohmann::json Serializer::serialize( const Setting & p_setting ) const
	{
		const std::string & defaultRepresentationName
			= Model::Representation::RepresentationLibrary::get()
				  .getRepresentation( p_setting.getDefaultRepresentationIndex() )
				  ->getName();

		const std::string & defaultRenderEffectPresetName
			= Model::Renderer::RenderEffectPresetLibrary::get()
				  .getPreset( p_setting.getDefaultRenderEffectPresetIndex() )
				  ->getName();

		return {
			{ "SYMBOL_DISPLAY_MODE", p_setting.getSymbolDisplayMode() },
			{ "WINDOW_FULLSCREEN", p_setting.getWindowFullscreen() },
			{ "ACTIVE_RENDERER", p_setting.getActivateRenderer() },
			{ "FORCE_RENDERER", p_setting.getForceRenderer() },
			{ "REPRESENTATION", defaultRepresentationName },
			{ "RENDER_EFFECT_DEFAULT", defaultRenderEffectPresetName },
			{ "ACTIVE_VSYNC", p_setting.getVSync() },
			{ "SNAPSHOT_FORMAT", p_setting.getSnapshotFormat() },
			{ "BACKGROUND_OPACITY", p_setting.getSnapshotBackgroundOpacity() },
			{ "SNAPSHOT_QUALITY", p_setting.getSnapshotQuality() },
			{ "BACKGROUND_RESOLUTION", magic_enum::enum_name( p_setting.getSnapshotResolution() ) },

			{ "CONTROLLER_TRANSLATION_SPEED", p_setting.getTranslationSpeed() },
			{ "CONTROLLER_TRANSLATION_FACTOR", p_setting.getTranslationSpeedFactor() },
			{ "CONTROLLER_ROTATION_SPEED", p_setting.getRotationSpeed() },
			{ "CONTROLLER_Y_AXIS_INVERTED", p_setting.getYAxisInverted() },

			{ "ACTIVE_CONTROLLER_ELASTICITY", p_setting.getControllerElasticityActive() },
			{ "CONTROLLER_ELASTICITY_FACTOR", p_setting.getControllerElasticityFactor() },

			{ "DEFAULT_TRAJECTORY_SPEED", p_setting.getDefaultTrajectorySpeed() },
			{ "DEFAULT_TRAJECTORY_PLAY_MODE", magic_enum::enum_name( p_setting.getDefaultTrajectoryPlayMode() ) },
			{ "CHECK_VTX_UPDATE_AT_LAUNCH", p_setting.getCheckVTXUpdateAtLaunch() },
			{ "PORTABLE_SAVE_ACTIVATED", p_setting.isPortableSaveActivated() },
			{ "SELECTION_GRANULARITY", magic_enum::enum_name( p_setting.getSelectionGranularity() ) },
		};
	}

	void Serializer::deserialize( const nlohmann::json & p_json, VTXApp & p_app ) const
	{
		deserialize( p_json.at( "SCENE" ), p_app.getScene() );
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Object3D::Scene & p_scene ) const
	{
		Vec3f cameraPos;
		if ( p_json.contains( "CAMERA_POSITION" ) )
			deserialize( p_json.at( "CAMERA_POSITION" ), cameraPos );

		Quatf cameraRot;
		if ( p_json.contains( "CAMERA_ROTATION" ) )
			deserialize( p_json.at( "CAMERA_ROTATION" ), cameraRot );

		std::vector<Model::Molecule *> molecules = std::vector<Model::Molecule *>();

		const int moleculeCount = _get<int>( p_json, "MOLECULE_COUNT" );
		molecules.resize( moleculeCount );

		if ( p_json.contains( "MOLECULES" ) )
		{
			for ( const nlohmann::json & jsonMolecule : p_json.at( "MOLECULES" ) )
			{
				if ( jsonMolecule.contains( "MOLECULE" ) )
				{
					Model::Molecule * const molecule = MVC::MvcManager::get().instantiateModel<Model::Molecule>();

					try
					{
						deserialize( jsonMolecule.at( "MOLECULE" ), *molecule );
					}
					catch ( std::exception e )
					{
						MVC::MvcManager::get().deleteModel( molecule );
						throw e;
					}

					molecule->getConfiguration().sceneIndex				 = _get<int>( jsonMolecule, "INDEX", INT_MAX );
					molecules[ molecule->getConfiguration().sceneIndex ] = molecule;
				}
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				if ( molecule == nullptr )
					continue;

				molecule->refreshVisibilities( false );
				VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_CREATED, molecule ) );
				p_scene.addMolecule( molecule );
			}
		}

		if ( p_json.contains( "PATHS" ) )
		{
			for ( const nlohmann::json & jsonPath : p_json.at( "PATHS" ) )
			{
				Model::Path * const path = MVC::MvcManager::get().instantiateModel<Model::Path>();
				deserialize( jsonPath, *path );
				p_scene.addPath( path );
			}
		}

		VTXApp::get()
			.getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->resetCameraController();
		p_scene.getCamera().setPosition( cameraPos );
		p_scene.getCamera().setRotation( cameraRot );
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Model::Molecule & p_molecule ) const
	{
		if ( p_json.contains( "TRANSFORM" ) )
		{
			Math::Transform transform;
			deserialize( p_json.at( "TRANSFORM" ), transform );
			p_molecule.applyTransform( transform );
		}

		IO::FilePath molPath = _get<std::string>( p_json, "PATH" );

		if ( Util::Filesystem::isRelativePath( molPath ) )
		{
			const IO::FilePath sceneFolder
				= Util::Filesystem::getSceneSaveDirectory( VTXApp::get().getScenePathData().getCurrentPath() );
			molPath = sceneFolder / molPath;
		}

		try
		{
			IO::Reader::LibChemfiles reader = IO::Reader::LibChemfiles( _thread );
			reader.readFile( molPath, p_molecule );
			VTXApp::get().getScenePathData().registerLoading( &p_molecule, molPath );
		}
		catch ( const std::exception & p_exception )
		{
			_logWarning( "Error when loading " + molPath.path() + " : " + p_exception.what() );
			throw p_exception;
		}

		if ( p_json.contains( "REPRESENTATIONS" ) )
			_deserializeMoleculeRepresentations( p_json.at( "REPRESENTATIONS" ), p_molecule );
		if ( p_json.contains( "VISIBILITIES" ) )
			_deserializeMoleculeVisibilities( p_json.at( "VISIBILITIES" ), p_molecule );

		p_molecule.setName( _get<std::string>( p_json, "NAME" ) );
		p_molecule.setPdbIdCode( _get<std::string>( p_json, "PDB_ID" ) );
		p_molecule.setDisplayName( _get<std::string>( p_json, "DISPLAY_NAME" ) );

		p_molecule.setFrame( _get<uint>( p_json, "CURRENT_FRAME" ) );
		p_molecule.setFPS( _get<uint>( p_json, "TRAJECTORY_FPS" ) );
		p_molecule.setPlayMode( _getEnum<Trajectory::PlayMode>( p_json, "TRAJECTORY_PLAYMODE" ) );
		p_molecule.setIsPlaying( _get<bool>( p_json, "TRAJECTORY_ISPLAYING" ) );
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Model::Path & p_path ) const
	{
		p_path.setName( _get<std::string>( p_json, "NAME" ) );
		if ( p_json.contains( "MODE_DURATION" ) )
		{
			std::string value	  = p_json.at( "MODE_DURATION" ).get<std::string>();
			auto		valueEnum = magic_enum::enum_cast<Model::Path::DURATION_MODE>( value );
			if ( valueEnum.has_value() )
			{
				p_path.setDurationMode( valueEnum.value() );
			}
		}

		if ( p_json.contains( "MODE_INTERPOLATION" ) )
		{
			std::string value	  = p_json.at( "MODE_INTERPOLATION" ).get<std::string>();
			auto		valueEnum = magic_enum::enum_cast<Model::Path::INTERPOLATION_MODE>( value );
			if ( valueEnum.has_value() )
			{
				p_path.setInterpolationMode( valueEnum.value() );
			}
		}

		p_path.setDuration( _get<float>( p_json, "DURATION", Setting::PATH_DURATION_DEFAULT ) );
		p_path.setIsLooping( _get<bool>( p_json, "IS_LOOPING" ) );

		if ( p_json.contains( "VIEWPOINTS" ) )
		{
			for ( const nlohmann::json & jsonViewpoint : p_json.at( "VIEWPOINTS" ) )
			{
				Model::Viewpoint * const viewpoint
					= MVC::MvcManager::get().instantiateModel<Model::Viewpoint>( &p_path );
				deserialize( jsonViewpoint, *viewpoint );
				p_path.addViewpoint( viewpoint );
			}
		}

		p_path.refreshAllDurations();
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Model::Viewpoint & p_viewpoint ) const
	{
		p_viewpoint.setName( _get<std::string>( p_json, "NAME" ) );
		p_viewpoint.setController( _get<ID::VTX_ID>( p_json, "CONTROLLER", Setting::CONTROLLER_MODE_DEFAULT ) );
		p_viewpoint.setDuration( _get<float>( p_json, "DURATION" ) );

		if ( p_json.contains( "POSITION" ) )
		{
			Vec3f position;
			deserialize( p_json.at( "POSITION" ), position );
			p_viewpoint.setPosition( position );
		}

		if ( p_json.contains( "ROTATION" ) )
		{
			Quatf rotation;
			deserialize( p_json.at( "ROTATION" ), rotation );
			p_viewpoint.setRotation( rotation );
		}

		if ( p_viewpoint.getController() == ID::Controller::TRACKBALL )
		{
			if ( p_json.contains( "TARGET" ) )
			{
				Vec3f target;
				deserialize( p_json.at( "TARGET" ), target );
				p_viewpoint.setTarget( target );
			}

			p_viewpoint.setDistance( _get<float>( p_json, "DISTANCE" ) );
		}
	}

	void Serializer::deserialize( const nlohmann::json &							  p_json,
								  Model::Representation::InstantiatedRepresentation & p_representation ) const
	{
		const int representationPresetIndex = p_json.at( "REPRESENTATION_PRESET_INDEX" ).get<int>();
		const Model::Representation::Representation * const sourceRepresentation
			= Model::Representation::RepresentationLibrary::get().getRepresentation( representationPresetIndex );

		p_representation.setLinkedRepresentation( sourceRepresentation );

		if ( p_json.contains( "SPHERE_RADIUS_ADD" ) && !p_representation.getSphereData()._isRadiusFixed )
		{
			p_representation.setSphereRadius( p_json.at( "SPHERE_RADIUS_ADD" ).get<float>() );
		}
		if ( p_json.contains( "SPHERE_RADIUS_FIXED" ) && p_representation.getSphereData()._isRadiusFixed )
		{
			p_representation.setSphereRadius( p_json.at( "SPHERE_RADIUS_FIXED" ).get<float>() );
		}
		if ( p_json.contains( "CYLINDER_RADIUS" ) )
		{
			p_representation.setCylinderRadius( p_json.at( "CYLINDER_RADIUS" ).get<float>() );
		}
		if ( p_json.contains( "COLOR_MODE" ) )
		{
			std::string value	  = p_json.at( "COLOR_MODE" ).get<std::string>();
			auto		valueEnum = magic_enum::enum_cast<Generic::COLOR_MODE>( value );
			if ( valueEnum.has_value() )
			{
				p_representation.setColorMode( valueEnum.value() );
			}
		}
		if ( p_json.contains( "SS_COLOR_MODE" ) )
		{
			std::string value	  = p_json.at( "SS_COLOR_MODE" ).get<std::string>();
			auto		valueEnum = magic_enum::enum_cast<Generic::SECONDARY_STRUCTURE_COLOR_MODE>( value );
			if ( valueEnum.has_value() )
			{
				p_representation.setSecondaryStructureColorMode( valueEnum.value() );
			}
		}
		if ( p_json.contains( "COLOR" ) )
		{
			Color::Rgb color;
			deserialize( p_json.at( "COLOR" ), color );
			p_representation.setColor( color );
		}
	}

	void Serializer::deserialize( const nlohmann::json &				  p_json,
								  Model::Representation::Representation & p_representation ) const
	{
		Color::Rgb color;
		if ( p_json.contains( "COLOR" ) )
		{
			deserialize( p_json.at( "COLOR" ), color );
			p_representation.setColor( color );
		}

		p_representation.setQuickAccess( _get<bool>( p_json, "QUICK_ACCESS", false ) );

		p_representation.changeRepresentationType(
			_getEnum<Generic::REPRESENTATION>( p_json, "TYPE", Setting::DEFAULT_REPRESENTATION_TYPE ), false );
		p_representation.getData().setSphereRadius(
			_get<float>( p_json, "SPHERE_RADIUS", Setting::ATOMS_RADIUS_DEFAULT ) );
		p_representation.getData().setCylinderRadius(
			_get<float>( p_json, "CYLINDER_RADIUS", Setting::BONDS_RADIUS_DEFAULT ) );
		p_representation.getData().setColorMode(
			_getEnum<Generic::COLOR_MODE>( p_json, "COLOR_MODE", Setting::COLOR_MODE_DEFAULT ) );
		p_representation.getData().setSecondaryStructureColorMode( _getEnum<Generic::SECONDARY_STRUCTURE_COLOR_MODE>(
			p_json, "SS_COLOR_MODE", Setting::SS_COLOR_MODE_DEFAULT ) );
	}
	void Serializer::deserialize( const nlohmann::json & p_json, Model::Renderer::RenderEffectPreset & p_preset ) const
	{
		Color::Rgb color;

		p_preset.setQuickAccess( _get<bool>( p_json, "QUICK_ACCESS", false ) );
		p_preset.setShading( _getEnum<Renderer::SHADING>( p_json, "SHADING", Setting::SHADING_DEFAULT ) );
		p_preset.enableSSAO( _get<bool>( p_json, "SSAO", Setting::ACTIVE_AO_DEFAULT ) );
		p_preset.setSSAOIntensity( _get<int>( p_json, "SSAO_INTENSITY", Setting::AO_INTENSITY_DEFAULT ) );
		p_preset.setSSAOBlurSize( _get<int>( p_json, "SSAO_BLUR_SIZE", Setting::AO_BLUR_SIZE_DEFAULT ) );
		p_preset.enableOutline( _get<bool>( p_json, "OUTLINE", Setting::ACTIVE_OUTLINE_DEFAULT ) );
		p_preset.setOutlineThickness( _get<int>( p_json, "OUTLINE_THICKNESS", Setting::OUTLINE_THICKNESS_DEFAULT ) );
		p_preset.setOutlineSensivity( _get<float>( p_json, "OUTLINE_SENSIVITY", Setting::OUTLINE_SENSIVITY_DEFAULT ) );

		if ( p_json.contains( "OUTLINE_COLOR" ) )
			deserialize( p_json.at( "OUTLINE_COLOR" ), color );
		else
			color = Setting::OUTLINE_COLOR_DEFAULT;
		p_preset.setOutlineColor( color );

		p_preset.enableFog( _get<bool>( p_json, "FOG", Setting::ACTIVE_FOG_DEFAULT ) );
		p_preset.setFogNear( _get<float>( p_json, "FOG_NEAR", Setting::FOG_NEAR_DEFAULT ) );
		p_preset.setFogFar( _get<float>( p_json, "FOG_FAR", Setting::FOG_FAR_DEFAULT ) );
		p_preset.setFogDensity( _get<float>( p_json, "FOG_DENSITY", Setting::FOG_DENSITY_DEFAULT ) );

		if ( p_json.contains( "FOG_COLOR" ) )
			deserialize( p_json.at( "FOG_COLOR" ), color );
		else
			color = Setting::FOG_COLOR_DEFAULT;
		p_preset.setFogColor( color );

		if ( p_json.contains( "BACKGROUND_COLOR" ) )
			deserialize( p_json.at( "BACKGROUND_COLOR" ), color );
		else
			color = Setting::BACKGROUND_COLOR_DEFAULT;
		p_preset.setBackgroundColor( color );

		if ( p_json.contains( "CAMERA_LIGHT_COLOR" ) )
			deserialize( p_json.at( "CAMERA_LIGHT_COLOR" ), color );
		else
			color = Setting::LIGHT_COLOR_DEFAULT;
		p_preset.setCameraLightColor( color );

		p_preset.setCameraFOV( _get<float>( p_json, "CAMERA_FOV", Setting::CAMERA_FOV_DEFAULT ) );
		p_preset.setCameraNearClip( _get<float>( p_json, "CAMERA_NEAR_CLIP", Setting::CAMERA_NEAR_DEFAULT ) );
		p_preset.setCameraFarClip( _get<float>( p_json, "CAMERA_FAR_CLIP", Setting::CAMERA_FAR_DEFAULT ) );
		p_preset.setAA( _get<bool>( p_json, "CAMERA_AA", Setting::ACTIVE_AA_DEFAULT ) );
		p_preset.setPerspectiveProjection(
			_get<bool>( p_json, "CAMERA_PERSPECTIVE_PROJECTION", Setting::CAMERA_PERSPECTIVE_DEFAULT ) );
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Color::Rgb & p_color ) const
	{
		p_color.setR( _get<float>( p_json, "R" ) );
		p_color.setG( _get<float>( p_json, "G" ) );
		p_color.setB( _get<float>( p_json, "B" ) );
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Math::Transform & p_transform ) const
	{
		if ( p_json.contains( "POSITION" ) )
		{
			Vec3f position;
			deserialize( p_json.at( "POSITION" ), position );
			p_transform.setTranslation( position );
		}

		if ( p_json.contains( "ROTATION" ) )
		{
			Vec3f euler;
			deserialize( p_json.at( "ROTATION" ), euler );
			p_transform.setRotation( euler );
		}

		if ( p_json.contains( "SCALE" ) )
		{
			Vec3f scale;
			deserialize( p_json.at( "SCALE" ), scale );
			p_transform.setScale( scale );
		}
	}

	template<typename T, glm::qualifier Q>
	void Serializer::deserialize( const nlohmann::json & p_json, glm::vec<2, T, Q> & p_vec ) const
	{
		p_vec.x = _get<T>( p_json, "X" );
		p_vec.y = _get<T>( p_json, "Y" );
	}
	template<typename T, glm::qualifier Q>
	void Serializer::deserialize( const nlohmann::json & p_json, glm::vec<3, T, Q> & p_vec ) const
	{
		p_vec.x = _get<T>( p_json, "X" );
		p_vec.y = _get<T>( p_json, "Y" );
		p_vec.z = _get<T>( p_json, "Z" );
	}
	template<typename T, glm::qualifier Q>
	void Serializer::deserialize( const nlohmann::json & p_json, glm::vec<4, T, Q> & p_vec ) const
	{
		p_vec.x = _get<T>( p_json, "X" );
		p_vec.y = _get<T>( p_json, "Y" );
		p_vec.z = _get<T>( p_json, "Z" );
		p_vec.w = _get<T>( p_json, "W" );
	}

	template<typename T, glm::qualifier Q>
	void Serializer::deserialize( const nlohmann::json & p_json, glm::qua<T, Q> & p_quat ) const
	{
		p_quat.x = _get<T>( p_json, "X" );
		p_quat.y = _get<T>( p_json, "Y" );
		p_quat.z = _get<T>( p_json, "Z" );
		p_quat.w = _get<T>( p_json, "W" );
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Setting & p_setting ) const
	{
		const Style::SYMBOL_DISPLAY_MODE symbolDisplayMode
			= _get<Style::SYMBOL_DISPLAY_MODE>( p_json, "SYMBOL_DISPLAY_MODE", Setting::SYMBOL_DISPLAY_MODE_DEFAULT );
		p_setting.setSymbolDisplayMode(
			Style::SYMBOL_DISPLAY_MODE( int( symbolDisplayMode ) % int( Style::SYMBOL_DISPLAY_MODE::COUNT ) ) );

		p_setting.setWindowFullscreen( _get<bool>( p_json, "WINDOW_FULLSCREEN", Setting::WINDOW_FULLSCREEN_DEFAULT ) );

		p_setting.setActivateRenderer( _get<bool>( p_json, "ACTIVE_RENDERER", Setting::ACTIVE_RENDERER_DEFAULT ) );
		p_setting.setForceRenderer( _get<bool>( p_json, "FORCE_RENDERER", Setting::FORCE_RENDERER_DEFAULT ) );

		p_setting.setTmpRepresentationDefaultName( _get<std::string>( p_json, "REPRESENTATION" ) );
		p_setting.setTmpRenderEffectPresetDefaultName( _get<std::string>( p_json, "RENDER_EFFECT_DEFAULT" ) );

		p_setting.setVSync( _get<bool>( p_json, "ACTIVE_VSYNC", Setting::ACTIVE_VSYNC_DEFAULT ) );

		p_setting.setSnapshotFormat(
			_getEnum<IO::Struct::ImageExport::Format>( p_json, "SNAPSHOT_FORMAT", Setting::SNAPSHOT_FORMAT_DEFAULT ) );
		p_setting.setSnapshotBackgroundOpacity(
			_get<float>( p_json, "BACKGROUND_OPACITY", Setting::BACKGROUND_OPACITY_DEFAULT ) );
		p_setting.setSnapshotQuality( _get<float>( p_json, "SNAPSHOT_QUALITY", Setting::SNAPSHOT_QUALITY_DEFAULT ) );
		p_setting.setSnapshotResolution( _getEnum<IO::Struct::ImageExport::RESOLUTION>(
			p_json, "BACKGROUND_RESOLUTION", Setting::SNAPSHOT_RESOLUTION_DEFAULT ) );

		p_setting.setTranslationSpeed(
			_get<float>( p_json, "CONTROLLER_TRANSLATION_SPEED", Setting::CONTROLLER_TRANSLATION_SPEED_DEFAULT ) );
		p_setting.setTranslationSpeedFactor(
			_get<float>( p_json, "CONTROLLER_TRANSLATION_FACTOR", Setting::CONTROLLER_TRANSLATION_FACTOR_DEFAULT ) );
		p_setting.setRotationSpeed(
			_get<float>( p_json, "CONTROLLER_ROTATION_SPEED", Setting::CONTROLLER_ROTATION_SPEED_DEFAULT ) );
		p_setting.setYAxisInverted(
			_get<bool>( p_json, "CONTROLLER_Y_AXIS_INVERTED", Setting::CONTROLLER_Y_AXIS_INVERTED ) );

		p_setting.setControllerElasticityActive(
			_get<bool>( p_json, "ACTIVE_CONTROLLER_ELASTICITY", Setting::CONTROLLER_ELASTICITY_ACTIVE_DEFAULT ) );
		p_setting.setControllerElasticityFactor(
			_get<float>( p_json, "CONTROLLER_ELASTICITY_FACTOR", Setting::CONTROLLER_ELASTICITY_FACTOR_DEFAULT ) );

		p_setting.setDefaultTrajectorySpeed(
			_get<int>( p_json, "DEFAULT_TRAJECTORY_SPEED", Setting::DEFAULT_TRAJECTORY_SPEED ) );
		p_setting.setDefaultTrajectoryPlayMode( _getEnum<Trajectory::PlayMode>(
			p_json, "DEFAULT_TRAJECTORY_PLAY_MODE", Setting::DEFAULT_TRAJECTORY_PLAY_MODE ) );

		p_setting.setCheckVTXUpdateAtLaunch(
			_get<bool>( p_json, "CHECK_VTX_UPDATE_AT_LAUNCH", Setting::CHECK_VTX_UPDATE_DEFAULT ) );

		p_setting.activatePortableSave(
			_get<bool>( p_json, "PORTABLE_SAVE_ACTIVATED", Setting::PORTABLE_SAVE_ACTIVATED_DEFAULT ) );

		p_setting.setSelectionGranularity(
			_getEnum( p_json, "SELECTION_GRANULARITY", Setting::SELECTION_GRANULARITY_DEFAULT ) );
	}

	nlohmann::json Serializer::_serializeMoleculeRepresentations( const Model::Molecule &		  p_molecule,
																  const Writer::ChemfilesWriter * p_writer ) const
	{
		nlohmann::json jsonArrayRepresentations = nlohmann::json::array();
		nlohmann::json jsonRepresentation		= { { "TARGET_TYPE", VTX::ID::Model::MODEL_MOLECULE },
												{ "INDEX", 0 },
												{ "REPRESENTATION", serialize( *p_molecule.getRepresentation() ) } };
		jsonArrayRepresentations.emplace_back( jsonRepresentation );

		for ( const Model::Chain * const chain : p_molecule.getChains() )
		{
			if ( chain == nullptr )
				continue;

			if ( chain->hasCustomRepresentation() )
			{
				jsonRepresentation = { { "TARGET_TYPE", VTX::ID::Model::MODEL_CHAIN },
									   { "INDEX", chain->getIndex() },
									   { "REPRESENTATION", serialize( *chain->getRepresentation() ) } };
				jsonArrayRepresentations.emplace_back( jsonRepresentation );
			}

			for ( uint residueIndex = 0; residueIndex < chain->getResidueCount(); residueIndex++ )
			{
				const Model::Residue * const residue
					= p_molecule.getResidue( chain->getIndexFirstResidue() + residueIndex );

				if ( residue == nullptr )
					continue;

				if ( residue->hasCustomRepresentation() )
				{
					const uint newResidueIndex
						= p_writer != nullptr ? p_writer->getNewResidueIndex( *residue ) : residue->getIndex();
					jsonRepresentation = { { "TARGET_TYPE", VTX::ID::Model::MODEL_RESIDUE },
										   { "INDEX", newResidueIndex },
										   { "REPRESENTATION", serialize( *residue->getRepresentation() ) } };

					jsonArrayRepresentations.emplace_back( jsonRepresentation );
				}
			}
		}

		return jsonArrayRepresentations;
	}
	nlohmann::json Serializer::_serializeMoleculeVisibilities( const Model::Molecule &		   p_molecule,
															   const Writer::ChemfilesWriter * p_writer ) const
	{
		nlohmann::json jsonChainVisibilitiesArray	= nlohmann::json::array();
		nlohmann::json jsonResidueVisibilitiesArray = nlohmann::json::array();
		nlohmann::json jsonAtomVisibilitiesArray	= nlohmann::json::array();

		for ( const Model::Chain * const chain : p_molecule.getChains() )
		{
			if ( chain == nullptr )
				continue;

			if ( !chain->isVisible() )
			{
				jsonChainVisibilitiesArray.emplace_back( chain->getIndex() );
			}
		}

		for ( const Model::Residue * const residue : p_molecule.getResidues() )
		{
			if ( residue == nullptr )
				continue;

			if ( !residue->isVisible() )
			{
				const uint newResidueIndex
					= p_writer != nullptr ? p_writer->getNewResidueIndex( *residue ) : residue->getIndex();
				jsonResidueVisibilitiesArray.emplace_back( newResidueIndex );
			}
		}

		for ( const Model::Atom * const atom : p_molecule.getAtoms() )
		{
			if ( atom == nullptr )
				continue;

			if ( !atom->isVisible() )
			{
				const uint newAtomIndex = p_writer != nullptr ? p_writer->getNewAtomIndex( *atom ) : atom->getIndex();
				jsonAtomVisibilitiesArray.emplace_back( newAtomIndex );
			}
		}

		return { { "SELF", p_molecule.isVisible() },
				 { "CHAINS", jsonChainVisibilitiesArray },
				 { "RESIDUES", jsonResidueVisibilitiesArray },
				 { "ATOMS", jsonAtomVisibilitiesArray } };
	}

	void Serializer::_deserializeMoleculeRepresentations( const nlohmann::json & p_json,
														  Model::Molecule &		 p_molecule ) const
	{
		for ( const nlohmann::json & jsonRepresentations : p_json )
		{
			if ( !jsonRepresentations.contains( "TARGET_TYPE" ) || !jsonRepresentations.contains( "INDEX" ) )
				continue;

			const ID::VTX_ID type  = jsonRepresentations.at( "TARGET_TYPE" ).get<ID::VTX_ID>();
			const uint		 index = jsonRepresentations.at( "INDEX" ).get<uint>();

			const bool dataValid = ( type == VTX::ID::Model::MODEL_MOLECULE )
								   || ( type == VTX::ID::Model::MODEL_CHAIN && index < p_molecule.getChainCount() )
								   || ( type == VTX::ID::Model::MODEL_RESIDUE && index < p_molecule.getResidueCount() );

			// Currently prevent app from crash when lodading out of range (because of deletion)
			if ( !dataValid )
				continue;

			Model::Representation::InstantiatedRepresentation * const representation
				= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>();
			deserialize( jsonRepresentations.at( "REPRESENTATION" ), *representation );

			if ( type == VTX::ID::Model::MODEL_MOLECULE )
			{
				Representation::RepresentationManager::get().assignRepresentation(
					representation, p_molecule, false, false );
			}
			else if ( type == VTX::ID::Model::MODEL_CHAIN )
			{
				Representation::RepresentationManager::get().assignRepresentation(
					representation, *p_molecule.getChain( index ), false, false );
			}
			else if ( type == VTX::ID::Model::MODEL_RESIDUE )
			{
				Representation::RepresentationManager::get().assignRepresentation(
					representation, *p_molecule.getResidue( index ), false, false );
			}
		}
	}
	void Serializer::_deserializeMoleculeVisibilities( const nlohmann::json & p_json,
													   Model::Molecule &	  p_molecule ) const
	{
		p_molecule.setVisible( _get<bool>( p_json, "SELF", true ) );

		if ( p_json.contains( "CHAINS" ) )
		{
			for ( const uint invisibleChainIndex : p_json.at( "CHAINS" ) )
			{
				p_molecule.getChain( invisibleChainIndex )->setVisible( false );
			}
		}

		if ( p_json.contains( "RESIDUES" ) )
		{
			for ( const uint invisibleResidueIndex : p_json.at( "RESIDUES" ) )
			{
				p_molecule.getResidue( invisibleResidueIndex )->setVisible( false );
			}
		}

		if ( p_json.contains( "ATOMS" ) )
		{
			for ( const uint invisibleAtomIndex : p_json.at( "ATOMS" ) )
			{
				p_molecule.getAtom( invisibleAtomIndex )->setVisible( false );
			}
		}
	}

	void Serializer::_logError( const std::string & p_msg ) const
	{
		if ( _thread != nullptr )
			_thread->logError( p_msg );
		else
			VTX_ERROR( p_msg );
	}
	void Serializer::_logWarning( const std::string & p_msg ) const
	{
		if ( _thread != nullptr )
			_thread->logWarning( p_msg );
		else
			VTX_WARNING( p_msg );
	}
	void Serializer::_logDebug( const std::string & p_msg ) const
	{
		if ( _thread != nullptr )
			_thread->logDebug( p_msg );
		else
			VTX_DEBUG( p_msg );
	}
	void Serializer::_logInfo( const std::string & p_msg ) const
	{
		if ( _thread != nullptr )
			_thread->logInfo( p_msg );
		else
			VTX_INFO( p_msg );
	}

} // namespace VTX::IO
