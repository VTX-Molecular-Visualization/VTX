#include "app/internal/io/serializer.hpp"
#include "app/action/main.hpp"
#include "app/action/renderer.hpp"
#include "app/action/setting.hpp"
#include "app/application/render_effect/render_effect_preset.hpp"
#include "app/application/representation/instantiated_representation.hpp"
#include "app/application/representation/representation_library.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/application/representation/representation_preset.hpp"
#include "app/application/scene.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/enum_trajectory.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/object3d/label.hpp"
#include "app/component/object3d/mesh_triangle.hpp"
#include "app/component/object3d/viewpoint.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/video/path.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/core/scene/base_scene_item.hpp"
#include "app/core/worker/base_thread.hpp"
#include "app/event.hpp"
#include "app/internal/chemdb/category.hpp"
#include "app/internal/io/filesystem.hpp"
#include "app/internal/io/reader/lib_chemfiles.hpp"
#include "app/internal/io/serialization/image_export.hpp"
#include "app/internal/io/serialization/scene_path_data.hpp"
#include "app/mvc.hpp"
#include "app/old_app/generic/base_colorable.hpp"
#include <algorithm>
#include <magic_enum.hpp>
#include <map>
#include <util/types.hpp>

namespace VTX::App::Internal::IO
{
	Serializer::Serializer( const Core::Worker::BaseThread * const p_thread ) : _thread( p_thread ) {}

	nlohmann::json Serializer::serialize( const VTXApp & p_app ) const
	{
		return nlohmann::json { { "SCENE", serialize( p_app.getScene() ) } };
	}

	nlohmann::json Serializer::serialize( const Application::Scene & p_scene ) const
	{
		nlohmann::json jsonArrayMolecules = nlohmann::json::array();
		for ( const Application::Scene::PairMoleculePtrFloat & pair : p_scene.getMolecules() )
		{
			nlohmann::json json
				= { { "INDEX", p_scene.getItemPosition( *pair.first ) }, { "MOLECULE", serialize( *pair.first ) } };

			jsonArrayMolecules.emplace_back( json );
		}

		nlohmann::json jsonArrayLabels = nlohmann::json::array();
		for ( const Component::Object3D::Label * const label : p_scene.getLabels() )
		{
			nlohmann::json json = { { "INDEX", p_scene.getItemPosition( *label ) }, { "LABEL", serialize( *label ) } };
			jsonArrayLabels.emplace_back( json );
		}

		nlohmann::json jsonArrayPaths = nlohmann::json::array();
		for ( const Component::Video::Path * const path : p_scene.getPaths() )
		{
			nlohmann::json json = { { "INDEX", p_scene.getItemPosition( *path ) }, { "PATH", serialize( *path ) } };
			jsonArrayPaths.emplace_back( json );
		}

		return { { "CAMERA_POSITION", serialize( p_scene.getCamera().getPosition() ) },
				 { "CAMERA_ROTATION", serialize( p_scene.getCamera().getRotation() ) },
				 { "MOLECULE_COUNT", p_scene.getMolecules().size() },
				 { "MOLECULES", jsonArrayMolecules },
				 { "PATHS", jsonArrayPaths },
				 { "LABELS", jsonArrayLabels } };
	}

	nlohmann::json Serializer::serialize( const Component::Chemistry::Molecule & p_molecule ) const
	{
		const FilePath moleculePath = VTXApp::get().getScenePathData().getFilepath( &p_molecule );

		const Writer::ChemfilesWriter * const writer
			= VTXApp::get().getScenePathData().getData( &p_molecule ).getWriter();

		return { { "TRANSFORM", serialize( p_molecule.getTransform() ) },
				 { "PATH", moleculePath.string() },
				 { "REPRESENTATIONS", _serializeMoleculeRepresentations( p_molecule, writer ) },
				 { "VISIBILITIES", _serializeMoleculeVisibilities( p_molecule, writer ) },
				 { "NAME", p_molecule.getName() },
				 { "PDB_ID", p_molecule.getPdbIdCode() },
				 { "DISPLAY_NAME", p_molecule.getDisplayName() },
				 { "COLOR", serialize( p_molecule.getColor() ) },
				 { "CURRENT_FRAME", p_molecule.getFrame() },
				 { "AUTO_ROTATE_ORIENTATION", serialize( p_molecule.getAutoRotationVector() ) },
				 { "AUTO_ROTATE_PLAYING_STATE", p_molecule.isAutoRotationPlaying() },
				 { "TRAJECTORY_FPS", p_molecule.getFPS() },
				 { "TRAJECTORY_PLAYMODE", magic_enum::enum_name( p_molecule.getPlayMode() ) },
				 { "TRAJECTORY_ISPLAYING", p_molecule.isPlaying() },
				 { "PERSISTENT_SCENE_ID", p_molecule.getPersistentSceneID() } };
	}

	nlohmann::json Serializer::serialize( const Component::Object3D::Label & p_label ) const
	{
		const App::VTX_ID & labelTypeID = p_label.getTypeId();

		// if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DISTANCE )
		//	return serialize( dynamic_cast<const Model::Measurement::Distance &>( p_label ) );
		// else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE )
		//	return serialize( dynamic_cast<const Model::Measurement::DistanceToCycle &>( p_label ) );
		// else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_ANGLE )
		//	return serialize( dynamic_cast<const Model::Measurement::Angle &>( p_label ) );
		// else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
		//	return serialize( dynamic_cast<const Model::Measurement::DihedralAngle &>( p_label ) );
		// else

		return {};
	}

	nlohmann::json Serializer::serialize( const Component::Video::Path & p_path ) const
	{
		nlohmann::json jsonArray = nlohmann::json::array();
		for ( const Component::Object3D::Viewpoint * const viewpoint : p_path.getViewpoints() )
		{
			jsonArray.emplace_back( serialize( *viewpoint ) );
		}

		return { { "NAME", p_path.getName() },
				 { "MODE_DURATION", magic_enum::enum_name( p_path.getDurationMode() ) },
				 { "MODE_INTERPOLATION", magic_enum::enum_name( p_path.getInterpolationMode() ) },
				 { "DURATION", p_path.getDuration() },
				 { "IS_LOOPING", p_path.isLooping() },
				 { "VIEWPOINTS", jsonArray },
				 { "PERSISTENT_SCENE_ID", p_path.getPersistentSceneID() } };
	}

	nlohmann::json Serializer::serialize( const Component::Object3D::Viewpoint & p_viewpoint ) const
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
		const Application::Representation::InstantiatedRepresentation & p_representation ) const
	{
		nlohmann::json json = nlohmann::json();

		const Application::Representation::RepresentationPreset * const linkedRep
			= p_representation.getLinkedRepresentation();
		const int presetIndex
			= Application::Representation::RepresentationLibrary::get().getRepresentationIndex( linkedRep );

		json[ "REPRESENTATION_PRESET_INDEX" ] = presetIndex;

		if ( p_representation.isMemberOverrided( Application::Representation::MEMBER_ENUM::SPHERE_RADIUS_ADD ) )
			json[ "SPHERE_RADIUS_ADD" ] = p_representation.getSphereData().radiusAdd;
		if ( p_representation.isMemberOverrided( Application::Representation::MEMBER_ENUM::SPHERE_RADIUS_FIXED ) )
			json[ "SPHERE_RADIUS_FIXED" ] = p_representation.getSphereData().radiusFixed;
		if ( p_representation.isMemberOverrided( Application::Representation::MEMBER_ENUM::CYLINDER_RADIUS ) )
			json[ "CYLINDER_RADIUS" ] = p_representation.getCylinderData().radius;
		if ( p_representation.isMemberOverrided(
				 Application::Representation::MEMBER_ENUM::CYLINDER_COLOR_BLENDING_MODE ) )
			json[ "CYLINDER_COLOR_BLENDING_MODE" ]
				= magic_enum::enum_name( p_representation.getCylinderData().colorBlendingMode );
		if ( p_representation.isMemberOverrided( Application::Representation::MEMBER_ENUM::RIBBON_COLOR_MODE ) )
			json[ "RIBBON_COLOR_MODE" ] = magic_enum::enum_name( p_representation.getRibbonData().colorMode );
		if ( p_representation.isMemberOverrided(
				 Application::Representation::MEMBER_ENUM::RIBBON_COLOR_BLENDING_MODE ) )
			json[ "RIBBON_COLOR_BLENDING_MODE" ]
				= magic_enum::enum_name( p_representation.getRibbonData().colorBlendingMode );
		if ( p_representation.isMemberOverrided( Application::Representation::MEMBER_ENUM::COLOR_MODE ) )
			json[ "COLOR_MODE" ] = magic_enum::enum_name( p_representation.getColorMode() );
		if ( p_representation.isMemberOverrided( Application::Representation::MEMBER_ENUM::COLOR ) )
			json[ "COLOR" ] = serialize( p_representation.getColor() );

		return json;
	}

	nlohmann::json Serializer::serialize(
		const Application::Representation::RepresentationPreset & p_representation ) const
	{
		nlohmann::json json = { { "QUICK_ACCESS", p_representation.hasQuickAccess() },
								{ "TYPE", magic_enum::enum_name( p_representation.getRepresentationType() ) },
								{ "COLOR_MODE", magic_enum::enum_name( p_representation.getData().getColorMode() ) },
								{ "COLOR", serialize( p_representation.getColor() ) } };

		if ( p_representation.getData().hasToDrawSphere() )
		{
			json[ "SPHERE_RADIUS" ] = p_representation.getData().getSphereRadius();
		}
		if ( p_representation.getData().hasToDrawCylinder() )
		{
			json[ "CYLINDER_RADIUS" ] = p_representation.getData().getCylinderRadius();
			json[ "CYLINDER_COLOR_BLENDING_MODE" ]
				= magic_enum::enum_name( p_representation.getData().getCylinderColorBlendingMode() );
		}
		if ( p_representation.getData().hasToDrawRibbon() )
		{
			json[ "RIBBON_COLOR_MODE" ] = magic_enum::enum_name( p_representation.getData().getRibbonColorMode() );
			json[ "RIBBON_COLOR_BLENDING_MODE" ]
				= magic_enum::enum_name( p_representation.getData().getRibbonColorBlendingMode() );
		}

		return json;
	}

	nlohmann::json Serializer::serialize( const Application::RenderEffect::RenderEffectPreset & p_preset ) const
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
		};
	}

	nlohmann::json Serializer::serialize( const Util::Color::Rgba & p_color ) const
	{
		return { { "R", p_color.getR() }, { "G", p_color.getG() }, { "B", p_color.getB() } };
	}

	nlohmann::json Serializer::serialize( const Internal::Math::Transform & p_transform ) const
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

	nlohmann::json Serializer::serialize( const Application::Setting & p_setting ) const
	{
		const std::string & defaultRepresentationName
			= Application::Representation::RepresentationLibrary::get()
				  .getRepresentation( p_setting.getDefaultRepresentationIndex() )
				  ->getName();

		const std::string & defaultRenderEffectPresetName
			= Application::RenderEffect::RenderEffectLibrary::get()
				  .getPreset( p_setting.getDefaultRenderEffectPresetIndex() )
				  ->getName();

		std::vector<std::string> defaultRepresentationNamePerCategory = std::vector<std::string>();
		defaultRepresentationNamePerCategory.resize( int( Internal::ChemDB::Category::TYPE::COUNT ) );

		for ( int i = 0; i < int( Internal::ChemDB::Category::TYPE::COUNT ); i++ )
		{
			const int representationIndex
				= p_setting.getDefaultRepresentationIndexPerCategory( Internal::ChemDB::Category::TYPE( i ) );
			const Application::Representation::RepresentationPreset * representation
				= Application::Representation::RepresentationLibrary::get().getRepresentation( representationIndex );

			if ( representation == nullptr )
				representation = Application::Representation::RepresentationLibrary::get().getDefaultRepresentation();

			defaultRepresentationNamePerCategory[ i ] = representation->getName();
		}

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

			{ "CAMERA_FOV", p_setting.getCameraFOV() },
			{ "CAMERA_NEAR_CLIP", p_setting.getCameraNearClip() },
			{ "CAMERA_FAR_CLIP", p_setting.getCameraFarClip() },
			{ "CAMERA_AA", p_setting.getAA() },
			{ "CAMERA_PERSPECTIVE_PROJECTION", p_setting.getCameraPerspective() },

			{ "CONTROLLER_TRANSLATION_SPEED", p_setting.getTranslationSpeed() },
			{ "CONTROLLER_ACCELERATION_FACTOR", p_setting.getAccelerationSpeedFactor() },
			{ "CONTROLLER_DECELERATION_FACTOR", p_setting.getDecelerationSpeedFactor() },
			{ "CONTROLLER_ROTATION_SPEED", p_setting.getRotationSpeed() },
			{ "CONTROLLER_Y_AXIS_INVERTED", p_setting.getYAxisInverted() },

			{ "ACTIVE_CONTROLLER_ELASTICITY", p_setting.getControllerElasticityActive() },
			{ "CONTROLLER_ELASTICITY_FACTOR", p_setting.getControllerElasticityFactor() },

			{ "DEFAULT_TRAJECTORY_SPEED", p_setting.getDefaultTrajectorySpeed() },
			{ "DEFAULT_TRAJECTORY_PLAY_MODE", magic_enum::enum_name( p_setting.getDefaultTrajectoryPlayMode() ) },
			{ "CHECK_VTX_UPDATE_AT_LAUNCH", p_setting.getCheckVTXUpdateAtLaunch() },
			{ "PORTABLE_SAVE_ACTIVATED", p_setting.isPortableSaveActivated() },
			{ "SELECTION_GRANULARITY", magic_enum::enum_name( p_setting.getSelectionGranularity() ) },

			{ "DEFAULT_REPRESENTATION_PER_CATEGORY", defaultRepresentationNamePerCategory },
		};
	}

	nlohmann::json Serializer::serializeAtomReference( const Component::Chemistry::Atom & p_atom ) const
	{
		return nlohmann::json(
			{ { "M", p_atom.getMoleculePtr()->getPersistentSceneID() }, { "A", p_atom.getIndex() } } );
	}

	void Serializer::deserialize( const nlohmann::json &			   p_json,
								  const std::tuple<uint, uint, uint> & p_version,
								  VTXApp &							   p_app ) const
	{
		deserialize( p_json.at( "SCENE" ), p_version, p_app.getScene() );
	}

	void Serializer::deserialize( const nlohmann::json &			   p_json,
								  const std::tuple<uint, uint, uint> & p_version,
								  Application::Scene &				   p_scene ) const
	{
		Vec3f cameraPos;
		if ( p_json.contains( "CAMERA_POSITION" ) )
			deserialize( p_json.at( "CAMERA_POSITION" ), cameraPos );

		Quatf cameraRot;
		if ( p_json.contains( "CAMERA_ROTATION" ) )
			deserialize( p_json.at( "CAMERA_ROTATION" ), cameraRot );

		std::vector<Component::Chemistry::Molecule *> molecules		  = std::vector<Component::Chemistry::Molecule *>();
		std::map<int, Core::Scene::BaseSceneItem *>	  itemPositionMap = std::map<int, Core::Scene::BaseSceneItem *>();

		int lastItemIndex = -1;

		if ( p_json.contains( "MOLECULES" ) )
		{
			for ( const nlohmann::json & jsonMolecule : p_json.at( "MOLECULES" ) )
			{
				if ( jsonMolecule.contains( "MOLECULE" ) )
				{
					Component::Chemistry::Molecule * const molecule
						= VTX::MVC_MANAGER().instantiateModel<Component::Chemistry::Molecule>();

					try
					{
						deserialize( jsonMolecule.at( "MOLECULE" ), p_version, *molecule );
					}
					catch ( std::exception e )
					{
						VTX::MVC_MANAGER().deleteModel( molecule );
						throw e;
					}

					if ( jsonMolecule.contains( "INDEX" ) )
					{
						const int itemPosition			= _get<int>( jsonMolecule, "INDEX" );
						lastItemIndex					= lastItemIndex > itemPosition ? lastItemIndex : itemPosition;
						itemPositionMap[ itemPosition ] = molecule;
					}

					p_scene.addMolecule( molecule );
				}
			}
		}

		if ( p_json.contains( "PATHS" ) )
		{
			for ( const nlohmann::json & jsonPath : p_json.at( "PATHS" ) )
			{
				Component::Video::Path * path;
				if ( jsonPath.contains( "PATH" ) )
				{
					path = tryDeserializeModel<Component::Video::Path>( jsonPath.at( "PATH" ) );
				}
				else
				{
					path = nullptr;
				}

				if ( path != nullptr )
				{
					if ( jsonPath.contains( "INDEX" ) )
					{
						const int itemPosition = _get<int>( jsonPath, "INDEX" );

						lastItemIndex					= lastItemIndex > itemPosition ? lastItemIndex : itemPosition;
						itemPositionMap[ itemPosition ] = path;
					}

					p_scene.addPath( path );
				}
			}
		}
		if ( p_json.contains( "LABELS" ) )
		{
			for ( const nlohmann::json & jsonLabel : p_json.at( "LABELS" ) )
			{
				if ( jsonLabel.contains( "LABEL" ) )
				{
					Component::Object3D::Label * label		   = nullptr;
					const nlohmann::json &		 jsonLabelData = jsonLabel.at( "LABEL" );

					if ( jsonLabelData.contains( "TYPE_ID" ) )
					{
						App::VTX_ID typeID = jsonLabelData.at( "TYPE_ID" ).get<VTX_ID>();

						// if ( typeID == ID::Model::MODEL_MEASUREMENT_DISTANCE )
						//	label = tryDeserializeModel<Model::Measurement::Distance>( jsonLabelData );
						//// else if ( typeID == ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE )
						////	label = tryDeserializeModel<Model::Measurement::DistanceToCycle>(
						////		jsonLabelData.at( "LABEL" ) );
						// else if ( typeID == ID::Model::MODEL_MEASUREMENT_ANGLE )
						//	label = tryDeserializeModel<Model::Measurement::Angle>( jsonLabelData );
						// else if ( typeID == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
						//	label = tryDeserializeModel<Model::Measurement::DihedralAngle>( jsonLabelData );
					}

					if ( label != nullptr )
					{
						if ( jsonLabel.contains( "INDEX" ) )
						{
							const int itemPosition = _get<int>( jsonLabel, "INDEX" );
							lastItemIndex		   = lastItemIndex > itemPosition ? lastItemIndex : itemPosition;
							itemPositionMap[ itemPosition ] = label;
						}
						p_scene.addLabel( label );
					}
				}
			}
		}

		for ( int i = 0; i <= lastItemIndex; i++ )
		{
			std::map<int, Core::Scene::BaseSceneItem *>::iterator it = itemPositionMap.find( i );
			if ( it != itemPositionMap.end() )
			{
				p_scene.changeModelPosition( *( it->second ), it->first );
			}
		}

		// TODO Manage this
		// VTXApp::get()
		//	.getStateMachine()
		//	.getState<State::Visualization>( ID::State::VISUALIZATION )
		//	->resetCameraController();

		p_scene.getCamera().setPosition( cameraPos );
		p_scene.getCamera().setRotation( cameraRot );
	}

	void Serializer::deserialize( const nlohmann::json &			   p_json,
								  const std::tuple<uint, uint, uint> & p_version,
								  Component::Chemistry::Molecule &	   p_molecule ) const
	{
		if ( p_json.contains( "TRANSFORM" ) )
		{
			Internal::Math::Transform transform;
			deserialize( p_json.at( "TRANSFORM" ), transform );
			p_molecule.applyTransform( transform );
		}

		FilePath molPath = _get<FilePath>( p_json, "PATH" );

		if ( molPath.is_relative() )
		{
			const FilePath sceneFolder
				= Internal::IO::Filesystem::getSceneSaveDirectory( VTXApp::get().getScenePathData().getCurrentPath() );
			molPath = sceneFolder / molPath;
		}

		try
		{
			Internal::IO::Reader::LibChemfiles reader = Internal::IO::Reader::LibChemfiles( _thread );
			reader.readFile( molPath, p_molecule );
			VTXApp::get().getScenePathData().registerLoading( &p_molecule, molPath );
		}
		catch ( const std::exception & p_exception )
		{
			_logWarning( "Error when loading " + molPath.string() + " : " + p_exception.what() );
			throw p_exception;
		}

		if ( p_json.contains( "REPRESENTATIONS" ) )
			_deserializeMoleculeRepresentations( p_json.at( "REPRESENTATIONS" ), p_version, p_molecule );
		if ( p_json.contains( "VISIBILITIES" ) )
			_deserializeMoleculeVisibilities( p_json.at( "VISIBILITIES" ), p_molecule );

		p_molecule.setName( _get<std::string>( p_json, "NAME" ) );
		p_molecule.setPdbIdCode( _get<std::string>( p_json, "PDB_ID" ) );
		p_molecule.setDisplayName( _get<std::string>( p_json, "DISPLAY_NAME" ) );

		if ( p_json.contains( "COLOR" ) )
		{
			Util::Color::Rgba color;
			deserialize( p_json.at( "COLOR" ), color );

			p_molecule.setColor( color );
		}

		if ( p_json.contains( "AUTO_ROTATE_ORIENTATION" ) )
		{
			Vec3f autoRotationOrientation;
			deserialize( p_json.at( "AUTO_ROTATE_ORIENTATION" ), autoRotationOrientation );
			p_molecule.setAutoRotationVector( autoRotationOrientation );
		}

		if ( p_json.contains( "AUTO_ROTATE_PLAYING_STATE" ) )
			p_molecule.setAutoRotationPlaying( _get<bool>( p_json, "AUTO_ROTATE_PLAYING_STATE" ) );

		_deserializeMoleculeRepresentations( p_json.at( "REPRESENTATIONS" ), p_version, p_molecule );

		p_molecule.setFrame( _get<uint>( p_json, "CURRENT_FRAME" ) );
		p_molecule.setFPS( _get<uint>( p_json, "TRAJECTORY_FPS" ) );
		p_molecule.setPlayMode( _getEnum<Component::Chemistry::PlayMode>( p_json, "TRAJECTORY_PLAYMODE" ) );
		p_molecule.setIsPlaying( _get<bool>( p_json, "TRAJECTORY_ISPLAYING" ) );
		p_molecule.setPersistentSceneID( _get<int>( p_json, "PERSISTENT_SCENE_ID" ) );
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Component::Video::Path & p_path ) const
	{
		p_path.setName( _get<std::string>( p_json, "NAME" ) );

		p_path.setDurationMode( _getEnum<Component::Video::PATH_DURATION_MODE>(
			p_json, "MODE_DURATION", Application::Setting::DEFAULT_PATH_DURATION_MODE ) );

		p_path.setInterpolationMode( _getEnum<Component::Video::PATH_INTERPOLATION_MODE>(
			p_json, "MODE_DURATION", Application::Setting::DEFAULT_PATH_INTERPOLATION_MODE ) );

		p_path.setDuration( _get<float>( p_json, "DURATION", Application::Setting::PATH_DURATION_DEFAULT ) );
		p_path.setIsLooping( _get<bool>( p_json, "IS_LOOPING" ) );
		p_path.setPersistentSceneID( _get<int>( p_json, "PERSISTENT_SCENE_ID" ) );

		if ( p_json.contains( "VIEWPOINTS" ) )
		{
			for ( const nlohmann::json & jsonViewpoint : p_json.at( "VIEWPOINTS" ) )
			{
				Component::Object3D::Viewpoint * const viewpoint
					= VTX::MVC_MANAGER().instantiateModel<Component::Object3D::Viewpoint>( &p_path );
				deserialize( jsonViewpoint, *viewpoint );
				p_path.addViewpoint( viewpoint );
			}
		}

		p_path.refreshAllDurations();
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Component::Object3D::Viewpoint & p_viewpoint ) const
	{
		p_viewpoint.setName( _get<std::string>( p_json, "NAME" ) );
		p_viewpoint.setController(
			_get<VTX_ID>( p_json, "CONTROLLER", Application::Setting::CONTROLLER_MODE_DEFAULT ) );
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

	void Serializer::deserialize( const nlohmann::json &									p_json,
								  const std::tuple<uint, uint, uint> &						p_version,
								  Application::Representation::InstantiatedRepresentation & p_representation ) const
	{
		const int representationPresetIndex = p_json.at( "REPRESENTATION_PRESET_INDEX" ).get<int>();
		const Application::Representation::RepresentationPreset * const sourceRepresentation
			= Application::Representation::RepresentationLibrary::get().getRepresentation( representationPresetIndex );

		p_representation.setLinkedRepresentation( sourceRepresentation );

		if ( p_json.contains( "SPHERE_RADIUS_ADD" ) && !p_representation.getSphereData().isRadiusFixed )
		{
			p_representation.setSphereRadius( p_json.at( "SPHERE_RADIUS_ADD" ).get<float>() );
		}
		if ( p_json.contains( "SPHERE_RADIUS_FIXED" ) && p_representation.getSphereData().isRadiusFixed )
		{
			p_representation.setSphereRadius( p_json.at( "SPHERE_RADIUS_FIXED" ).get<float>() );
		}
		if ( p_json.contains( "CYLINDER_RADIUS" ) )
		{
			p_representation.setCylinderRadius( p_json.at( "CYLINDER_RADIUS" ).get<float>() );
		}
		if ( p_json.contains( "CYLINDER_COLOR_BLENDING_MODE" ) )
		{
			p_representation.setCylinderColorBlendingMode( _getEnum<Generic::COLOR_BLENDING_MODE>(
				p_json, "CYLINDER_COLOR_BLENDING_MODE", Application::Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT ) );
		}
		if ( p_json.contains( "RIBBON_COLOR_MODE" ) )
		{
			p_representation.setRibbonColorMode( _getEnum<Generic::SECONDARY_STRUCTURE_COLOR_MODE>(
				p_json, "RIBBON_COLOR_MODE", Application::Setting::SS_COLOR_MODE_DEFAULT ) );
		}
		if ( p_json.contains( "RIBBON_COLOR_BLENDING_MODE" ) )
		{
			p_representation.setRibbonColorBlendingMode( _getEnum<Generic::COLOR_BLENDING_MODE>(
				p_json, "RIBBON_COLOR_BLENDING_MODE", Application::Setting::SS_COLOR_BLENDING_MODE_DEFAULT ) );
		}
		if ( p_json.contains( "COLOR_MODE" ) )
		{
			p_representation.setColorMode(
				_getEnum<Generic::COLOR_MODE>( p_json, "COLOR_MODE", Application::Setting::COLOR_MODE_DEFAULT ) );
		}
		if ( p_json.contains( "COLOR" ) )
		{
			Util::Color::Rgba color;
			deserialize( p_json.at( "COLOR" ), color );
			p_representation.setColor( color );
		}

		_migrate( p_json, p_version, p_representation );
	}

	void Serializer::deserialize( const nlohmann::json &							  p_json,
								  const std::tuple<uint, uint, uint> &				  p_version,
								  Application::Representation::RepresentationPreset & p_representation ) const
	{
		Util::Color::Rgba color;
		if ( p_json.contains( "COLOR" ) )
		{
			deserialize( p_json.at( "COLOR" ), color );
			p_representation.setColor( color );
		}

		p_representation.setQuickAccess( _get<bool>( p_json, "QUICK_ACCESS", false ) );

		p_representation.changeRepresentationType(
			_getEnum<Application::Representation::REPRESENTATION_ENUM>(
				p_json, "TYPE", Application::Setting::DEFAULT_REPRESENTATION_TYPE ),
			false );
		if ( p_representation.getData().hasToDrawSphere() )
		{
			p_representation.getData().setSphereRadius(
				_get<float>( p_json, "SPHERE_RADIUS", Application::Setting::ATOMS_RADIUS_DEFAULT ) );
		}
		if ( p_representation.getData().hasToDrawCylinder() )
		{
			p_representation.getData().setCylinderRadius(
				_get<float>( p_json, "CYLINDER_RADIUS", Application::Setting::BONDS_RADIUS_DEFAULT ) );
			p_representation.getData().setCylinderColorBlendingMode( _getEnum<Generic::COLOR_BLENDING_MODE>(
				p_json, "CYLINDER_COLOR_BLENDING_MODE", Application::Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT ) );
		}
		if ( p_representation.getData().hasToDrawRibbon() )
		{
			p_representation.getData().setRibbonColorMode( _getEnum<Generic::SECONDARY_STRUCTURE_COLOR_MODE>(
				p_json, "RIBBON_COLOR_MODE", Application::Setting::SS_COLOR_MODE_DEFAULT ) );
			p_representation.getData().setRibbonColorBlendingMode( _getEnum<Generic::COLOR_BLENDING_MODE>(
				p_json, "RIBBON_COLOR_BLENDING_MODE", Application::Setting::SS_COLOR_BLENDING_MODE_DEFAULT ) );
		}
		p_representation.getData().setColorMode(
			_getEnum<Generic::COLOR_MODE>( p_json, "COLOR_MODE", Application::Setting::COLOR_MODE_DEFAULT ) );

		_migrate( p_json, p_version, p_representation );
	}
	void Serializer::deserialize( const nlohmann::json &						  p_json,
								  const std::tuple<uint, uint, uint> &			  p_version,
								  Application::RenderEffect::RenderEffectPreset & p_preset ) const
	{
		Util::Color::Rgba color;

		p_preset.setQuickAccess( _get<bool>( p_json, "QUICK_ACCESS", false ) );
		p_preset.setShading(
			_getEnum<Render::Renderer::SHADING>( p_json, "SHADING", Application::Setting::SHADING_DEFAULT ) );
		p_preset.enableSSAO( _get<bool>( p_json, "SSAO", Application::Setting::ACTIVE_AO_DEFAULT ) );
		p_preset.setSSAOIntensity( _get<int>( p_json, "SSAO_INTENSITY", Application::Setting::AO_INTENSITY_DEFAULT ) );
		p_preset.setSSAOBlurSize( _get<int>( p_json, "SSAO_BLUR_SIZE", Application::Setting::AO_BLUR_SIZE_DEFAULT ) );
		p_preset.enableOutline( _get<bool>( p_json, "OUTLINE", Application::Setting::ACTIVE_OUTLINE_DEFAULT ) );
		p_preset.setOutlineThickness(
			_get<int>( p_json, "OUTLINE_THICKNESS", Application::Setting::OUTLINE_THICKNESS_DEFAULT ) );
		p_preset.setOutlineSensivity(
			_get<float>( p_json, "OUTLINE_SENSIVITY", Application::Setting::OUTLINE_SENSIVITY_DEFAULT ) );

		if ( p_json.contains( "OUTLINE_COLOR" ) )
			deserialize( p_json.at( "OUTLINE_COLOR" ), color );
		else
			color = Application::Setting::OUTLINE_COLOR_DEFAULT;
		p_preset.setOutlineColor( color );

		p_preset.enableFog( _get<bool>( p_json, "FOG", Application::Setting::ACTIVE_FOG_DEFAULT ) );
		p_preset.setFogNear( _get<float>( p_json, "FOG_NEAR", Application::Setting::FOG_NEAR_DEFAULT ) );
		p_preset.setFogFar( _get<float>( p_json, "FOG_FAR", Application::Setting::FOG_FAR_DEFAULT ) );
		p_preset.setFogDensity( _get<float>( p_json, "FOG_DENSITY", Application::Setting::FOG_DENSITY_DEFAULT ) );

		if ( p_json.contains( "FOG_COLOR" ) )
			deserialize( p_json.at( "FOG_COLOR" ), color );
		else
			color = Application::Setting::FOG_COLOR_DEFAULT;
		p_preset.setFogColor( color );

		if ( p_json.contains( "BACKGROUND_COLOR" ) )
			deserialize( p_json.at( "BACKGROUND_COLOR" ), color );
		else
			color = Application::Setting::BACKGROUND_COLOR_DEFAULT;
		p_preset.setBackgroundColor( color );

		if ( p_json.contains( "CAMERA_LIGHT_COLOR" ) )
			deserialize( p_json.at( "CAMERA_LIGHT_COLOR" ), color );
		else
			color = Application::Setting::LIGHT_COLOR_DEFAULT;
		p_preset.setCameraLightColor( color );
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Util::Color::Rgba & p_color ) const
	{
		p_color.setR( _get<float>( p_json, "R" ) );
		p_color.setG( _get<float>( p_json, "G" ) );
		p_color.setB( _get<float>( p_json, "B" ) );
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Internal::Math::Transform & p_transform ) const
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

	void Serializer::deserialize( const nlohmann::json &			   p_json,
								  const std::tuple<uint, uint, uint> & p_version,
								  Application::Setting &			   p_setting ) const
	{
		const Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE symbolDisplayMode
			= _get<Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE>(
				p_json, "SYMBOL_DISPLAY_MODE", Application::Setting::SYMBOL_DISPLAY_MODE_DEFAULT );
		p_setting.setSymbolDisplayMode( Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE(
			int( symbolDisplayMode ) % int( Internal::ChemDB::Residue::SYMBOL_DISPLAY_MODE::COUNT ) ) );

		p_setting.setWindowFullscreen(
			_get<bool>( p_json, "WINDOW_FULLSCREEN", Application::Setting::WINDOW_FULLSCREEN_DEFAULT ) );

		p_setting.setActivateRenderer(
			_get<bool>( p_json, "ACTIVE_RENDERER", Application::Setting::ACTIVE_RENDERER_DEFAULT ) );
		p_setting.setForceRenderer(
			_get<bool>( p_json, "FORCE_RENDERER", Application::Setting::FORCE_RENDERER_DEFAULT ) );

		p_setting.setTmpRepresentationDefaultName( _get<std::string>( p_json, "REPRESENTATION" ) );
		p_setting.setTmpRenderEffectPresetDefaultName( _get<std::string>( p_json, "RENDER_EFFECT_DEFAULT" ) );

		p_setting.setVSync( _get<bool>( p_json, "ACTIVE_VSYNC", Application::Setting::ACTIVE_VSYNC_DEFAULT ) );

		p_setting.setSnapshotFormat( _getEnum<Internal::IO::Serialization::ImageExport::Format>(
			p_json, "SNAPSHOT_FORMAT", Application::Setting::SNAPSHOT_FORMAT_DEFAULT ) );
		p_setting.setSnapshotBackgroundOpacity(
			_get<float>( p_json, "BACKGROUND_OPACITY", Application::Setting::BACKGROUND_OPACITY_DEFAULT ) );
		p_setting.setSnapshotQuality(
			_get<float>( p_json, "SNAPSHOT_QUALITY", Application::Setting::SNAPSHOT_QUALITY_DEFAULT ) );
		p_setting.setSnapshotResolution( _getEnum<Internal::IO::Serialization::ImageExport::RESOLUTION>(
			p_json, "BACKGROUND_RESOLUTION", Application::Setting::SNAPSHOT_RESOLUTION_DEFAULT ) );

		p_setting.setCameraFOV( _get<float>( p_json, "CAMERA_FOV", Application::Setting::CAMERA_FOV_DEFAULT ) );
		p_setting.setCameraNearClip(
			_get<float>( p_json, "CAMERA_NEAR_CLIP", Application::Setting::CAMERA_NEAR_DEFAULT ) );
		p_setting.setCameraFarClip(
			_get<float>( p_json, "CAMERA_FAR_CLIP", Application::Setting::CAMERA_FAR_DEFAULT ) );
		p_setting.setAA( _get<bool>( p_json, "CAMERA_AA", Application::Setting::ACTIVE_AA_DEFAULT ) );
		p_setting.setCameraPerspectiveProjection(
			_get<bool>( p_json, "CAMERA_PERSPECTIVE_PROJECTION", Application::Setting::CAMERA_PERSPECTIVE_DEFAULT ) );

		p_setting.setTranslationSpeed( _get<float>(
			p_json, "CONTROLLER_TRANSLATION_SPEED", Application::Setting::CONTROLLER_TRANSLATION_SPEED_DEFAULT ) );
		p_setting.setAccelerationSpeedFactor( _get<float>(
			p_json, "CONTROLLER_ACCELERATION_FACTOR", Application::Setting::CONTROLLER_ACCELERATION_FACTOR_DEFAULT ) );
		p_setting.setDecelerationSpeedFactor( _get<float>(
			p_json, "CONTROLLER_DECELERATION_FACTOR", Application::Setting::CONTROLLER_DECELERATION_FACTOR_DEFAULT ) );
		p_setting.setRotationSpeed( _get<float>(
			p_json, "CONTROLLER_ROTATION_SPEED", Application::Setting::CONTROLLER_ROTATION_SPEED_DEFAULT ) );
		p_setting.setYAxisInverted(
			_get<bool>( p_json, "CONTROLLER_Y_AXIS_INVERTED", Application::Setting::CONTROLLER_Y_AXIS_INVERTED ) );

		p_setting.setControllerElasticityActive( _get<bool>(
			p_json, "ACTIVE_CONTROLLER_ELASTICITY", Application::Setting::CONTROLLER_ELASTICITY_ACTIVE_DEFAULT ) );
		p_setting.setControllerElasticityFactor( _get<float>(
			p_json, "CONTROLLER_ELASTICITY_FACTOR", Application::Setting::CONTROLLER_ELASTICITY_FACTOR_DEFAULT ) );

		p_setting.setDefaultTrajectorySpeed(
			_get<int>( p_json, "DEFAULT_TRAJECTORY_SPEED", Application::Setting::DEFAULT_TRAJECTORY_SPEED ) );
		p_setting.setDefaultTrajectoryPlayMode( _getEnum<Component::Chemistry::PlayMode>(
			p_json, "DEFAULT_TRAJECTORY_PLAY_MODE", Application::Setting::DEFAULT_TRAJECTORY_PLAY_MODE ) );

		p_setting.setCheckVTXUpdateAtLaunch(
			_get<bool>( p_json, "CHECK_VTX_UPDATE_AT_LAUNCH", Application::Setting::CHECK_VTX_UPDATE_DEFAULT ) );

		p_setting.activatePortableSave(
			_get<bool>( p_json, "PORTABLE_SAVE_ACTIVATED", Application::Setting::PORTABLE_SAVE_ACTIVATED_DEFAULT ) );

		p_setting.setSelectionGranularity(
			_getEnum( p_json, "SELECTION_GRANULARITY", Application::Setting::SELECTION_GRANULARITY_DEFAULT ) );

		const std::vector<std::string> representationNamePerCategory
			= _get<std::vector<std::string>>( p_json,
											  "DEFAULT_REPRESENTATION_PER_CATEGORY",
											  Application::Setting::DEFAULT_REPRESENTATION_PER_CATEGORY_NAME );

		const int categoryCount = int( Internal::ChemDB::Category::TYPE::COUNT );
		if ( representationNamePerCategory.size() == categoryCount )
		{
			for ( int i = 0; i < categoryCount; i++ )
			{
				p_setting.setTmpDefaultRepresentationNamePerCategory( Internal::ChemDB::Category::TYPE( i ),
																	  representationNamePerCategory[ i ] );
			}
		}
	}

	const Component::Chemistry::Atom * Serializer::deserializeAtomReference( const nlohmann::json & p_json ) const
	{
		if ( !p_json.contains( "M" ) || !p_json.contains( "A" ) )
			return nullptr;

		const int moleculePersistentSceneID = p_json.at( "M" ).get<int>();

		const Application::Scene::MapMoleculePtrFloat & sceneMolecules = VTXApp::get().getScene().getMolecules();

		const Component::Chemistry::Molecule * linkedMolecule = nullptr;
		for ( const Application::Scene::PairMoleculePtrFloat & pair : sceneMolecules )
		{
			if ( pair.first->getPersistentSceneID() == moleculePersistentSceneID )
			{
				linkedMolecule = pair.first;
				break;
			}
		}

		if ( linkedMolecule == nullptr )
			return nullptr;

		const uint atomIndex = p_json.at( "A" ).get<uint>();
		return linkedMolecule->getAtom( atomIndex );
	}

	nlohmann::json Serializer::_serializeMoleculeRepresentations( const Component::Chemistry::Molecule & p_molecule,
																  const Writer::ChemfilesWriter * p_writer ) const
	{
		nlohmann::json jsonArrayRepresentations = nlohmann::json::array();
		nlohmann::json jsonRepresentation		= { { "TARGET_TYPE", App::ID::Model::MODEL_MOLECULE },
													{ "INDEX", 0 },
													{ "REPRESENTATION", serialize( *p_molecule.getRepresentation() ) } };
		jsonArrayRepresentations.emplace_back( jsonRepresentation );

		for ( const Component::Chemistry::Chain * const chain : p_molecule.getChains() )
		{
			if ( chain == nullptr )
				continue;

			if ( chain->hasCustomRepresentation() )
			{
				jsonRepresentation = { { "TARGET_TYPE", App::ID::Model::MODEL_CHAIN },
									   { "INDEX", chain->getIndex() },
									   { "REPRESENTATION", serialize( *chain->getRepresentation() ) } };
				jsonArrayRepresentations.emplace_back( jsonRepresentation );
			}

			for ( uint residueIndex = 0; residueIndex < chain->getResidueCount(); residueIndex++ )
			{
				const Component::Chemistry::Residue * const residue
					= p_molecule.getResidue( chain->getIndexFirstResidue() + residueIndex );

				if ( residue == nullptr )
					continue;

				if ( residue->hasCustomRepresentation() )
				{
					const uint newResidueIndex
						= p_writer != nullptr ? p_writer->getNewResidueIndex( *residue ) : residue->getIndex();
					jsonRepresentation = { { "TARGET_TYPE", App::ID::Model::MODEL_RESIDUE },
										   { "INDEX", newResidueIndex },
										   { "REPRESENTATION", serialize( *residue->getRepresentation() ) } };

					jsonArrayRepresentations.emplace_back( jsonRepresentation );
				}
			}
		}

		return jsonArrayRepresentations;
	}

	void Serializer::_migrate( const nlohmann::json &									 p_json,
							   const std::tuple<uint, uint, uint> &						 p_version,
							   Application::Representation::InstantiatedRepresentation & p_representation ) const
	{
		// 0.2.0 -> 0.3.0
		// SS_COLOR_MODE -> RIBBON_COLOR_MODE
		if ( std::get<1>( p_version ) < 3 )
		{
			if ( p_representation.hasToDrawRibbon() )
			{
				p_representation.setRibbonColorMode( _getEnum<Generic::SECONDARY_STRUCTURE_COLOR_MODE>(
					p_json, "SS_COLOR_MODE", Application::Setting::SS_COLOR_MODE_DEFAULT ) );
			}
		}
	}

	void Serializer::_migrate( const nlohmann::json &							   p_json,
							   const std::tuple<uint, uint, uint> &				   p_version,
							   Application::Representation::RepresentationPreset & p_representation ) const
	{
		// 0.2.0 -> 0.3.0
		if ( std::get<1>( p_version ) < 3 )
		{
			if ( p_representation.getData().hasToDrawRibbon() )
			{
				p_representation.getData().setRibbonColorMode( _getEnum<Generic::SECONDARY_STRUCTURE_COLOR_MODE>(
					p_json, "SS_COLOR_MODE", Application::Setting::SS_COLOR_MODE_DEFAULT ) );
			}
		}
	}

	nlohmann::json Serializer::_serializeMoleculeVisibilities( const Component::Chemistry::Molecule & p_molecule,
															   const Writer::ChemfilesWriter *		  p_writer ) const
	{
		nlohmann::json jsonChainVisibilitiesArray	= nlohmann::json::array();
		nlohmann::json jsonResidueVisibilitiesArray = nlohmann::json::array();
		nlohmann::json jsonAtomVisibilitiesArray	= nlohmann::json::array();

		for ( const Component::Chemistry::Chain * const chain : p_molecule.getChains() )
		{
			if ( chain == nullptr )
				continue;

			if ( !chain->isVisible() )
			{
				const uint newChainIndex
					= p_writer != nullptr ? p_writer->getNewChainIndex( *chain ) : chain->getIndex();
				jsonChainVisibilitiesArray.emplace_back( newChainIndex );
			}
		}

		for ( const Component::Chemistry::Residue * const residue : p_molecule.getResidues() )
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

		for ( const Component::Chemistry::Atom * const atom : p_molecule.getAtoms() )
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

	void Serializer::_deserializeMoleculeRepresentations( const nlohmann::json &			   p_json,
														  const std::tuple<uint, uint, uint> & p_version,
														  Component::Chemistry::Molecule &	   p_molecule ) const
	{
		for ( const nlohmann::json & jsonRepresentations : p_json )
		{
			if ( !jsonRepresentations.contains( "TARGET_TYPE" ) || !jsonRepresentations.contains( "INDEX" ) )
				continue;

			const App::VTX_ID type	= jsonRepresentations.at( "TARGET_TYPE" ).get<VTX_ID>();
			const uint		  index = jsonRepresentations.at( "INDEX" ).get<uint>();

			const bool dataValid = ( type == App::ID::Model::MODEL_MOLECULE )
								   || ( type == App::ID::Model::MODEL_CHAIN && index < p_molecule.getChainCount() )
								   || ( type == App::ID::Model::MODEL_RESIDUE && index < p_molecule.getResidueCount() );

			// Currently prevent app from crash when lodading out of range (because of deletion)
			if ( !dataValid )
				continue;

			Application::Representation::InstantiatedRepresentation * const representation
				= VTX::MVC_MANAGER().instantiateModel<Application::Representation::InstantiatedRepresentation>();
			deserialize( jsonRepresentations.at( "REPRESENTATION" ), p_version, *representation );

			if ( type == App::ID::Model::MODEL_MOLECULE )
			{
				Application::Representation::RepresentationManager::get().assignRepresentation(
					representation, p_molecule, false, false );
			}
			else if ( type == App::ID::Model::MODEL_CHAIN )
			{
				Application::Representation::RepresentationManager::get().assignRepresentation(
					representation, *p_molecule.getChain( index ), false, false );
			}
			else if ( type == App::ID::Model::MODEL_RESIDUE )
			{
				Application::Representation::RepresentationManager::get().assignRepresentation(
					representation, *p_molecule.getResidue( index ), false, false );
			}
		}
	}
	void Serializer::_deserializeMoleculeVisibilities( const nlohmann::json &			p_json,
													   Component::Chemistry::Molecule & p_molecule ) const
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

	// TODO Reimplement this wihtout Qt
	void Serializer::_logError( const std::string & p_msg ) const
	{
		// if ( _thread != nullptr )
		//	_thread->logError( p_msg );
		// else
		//	VTX_ERROR( p_msg );
	}
	// TODO Reimplement this wihtout Qt
	void Serializer::_logWarning( const std::string & p_msg ) const
	{
		// if ( _thread != nullptr )
		//	_thread->logWarning( p_msg );
		// else
		//	VTX_WARNING( p_msg );
	}
	// TODO Reimplement this wihtout Qt
	void Serializer::_logDebug( const std::string & p_msg ) const
	{
		// if ( _thread != nullptr )
		//	_thread->logDebug( p_msg );
		// else
		//	VTX_DEBUG( p_msg );
	}
	// TODO Reimplement this wihtout Qt
	void Serializer::_logInfo( const std::string & p_msg ) const
	{
		// if ( _thread != nullptr )
		//	_thread->logInfo( p_msg );
		// else
		//	VTX_INFO( p_msg );
	}

} // namespace VTX::App::Internal::IO
