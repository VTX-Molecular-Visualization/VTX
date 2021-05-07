#include "serializer.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "generic/base_colorable.hpp"
#include "io/reader/lib_chemfiles.hpp"
#include "model/chain.hpp"
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

namespace VTX
{
	namespace IO
	{
		nlohmann::json Serializer::serialize( const VTXApp & p_app ) const
		{
			return nlohmann::json { { "SCENE", serialize( p_app.getScene() ) } };
		}

		nlohmann::json Serializer::serialize( const Object3D::Scene & p_scene ) const
		{
			nlohmann::json jsonArrayMolecules = nlohmann::json::array();
			for ( const Object3D::Scene::PairMoleculePtrFloat & pair : p_scene.getMolecules() )
			{
				jsonArrayMolecules.emplace_back( serialize( *pair.first ) );
			}

			nlohmann::json jsonArrayPaths = nlohmann::json::array();
			for ( const Model::Path * const path : p_scene.getPaths() )
			{
				jsonArrayPaths.emplace_back( serialize( *path ) );
			}

			return { { "CAMERA_POSITION", serialize( p_scene.getCamera().getPosition() ) },
					 { "CAMERA_ROTATION", serialize( p_scene.getCamera().getRotation() ) },
					 { "MOLECULES", jsonArrayMolecules },
					 { "PATHS", jsonArrayPaths } };
		}

		nlohmann::json Serializer::serialize( const Model::Molecule & p_molecule ) const
		{
			Writer::ChemfilesWriter chemfileWriter = Writer::ChemfilesWriter();
			std::string				buffer		   = std::string();
			chemfileWriter.writeBuffer( buffer, p_molecule );

			return { { "TRANSFORM", serialize( p_molecule.getTransform() ) },
					 { "DATA", buffer },
					 { "REPRESENTATIONS", _serializeMoleculeRepresentations( p_molecule, chemfileWriter ) },
					 { "VISIBILITIES", _serializeMoleculeVisibilities( p_molecule, chemfileWriter ) },
					 { "NAME", p_molecule.getName() },
					 { "PDB_ID", p_molecule.getPdbIdCode() } };
		}

		nlohmann::json Serializer::serialize( const Model::Path & p_path ) const
		{
			nlohmann::json jsonArray = nlohmann::json::array();
			for ( const Model::Viewpoint * const viewpoint : p_path.getViewpoints() )
			{
				jsonArray.emplace_back( serialize( *viewpoint ) );
			}

			return { { "MODE_DURATION", p_path.getDurationMode() },
					 { "MODE_INTERPOLATION", p_path.getInterpolationMode() },
					 { "DURATION", p_path.getDuration() },
					 { "IS_LOOPING", p_path.isLooping() },
					 { "VIEWPOINTS", jsonArray } };
		}

		nlohmann::json Serializer::serialize( const Model::Viewpoint & p_viewpoint ) const
		{
			nlohmann::json jsonArray = nlohmann::json::array();
			for ( std::string action : p_viewpoint.getActions() )
			{
				std::replace( action.begin(), action.end(), ' ', '-' );
				jsonArray.emplace_back( action );
			}

			return { { "DURATION", p_viewpoint.getDuration() },
					 { "POSITION", serialize( p_viewpoint.getPosition() ) },
					 { "TARGET", serialize( p_viewpoint.getTarget() ) },
					 { "ROTATION", serialize( p_viewpoint.getRotation() ) },
					 { "DISTANCE", p_viewpoint.getDistance() },
					 { "CONTROLLER", p_viewpoint.getController() },
					 { "ACTIONS", jsonArray } };
		}

		nlohmann::json Serializer::serialize(
			const Model::Representation::InstantiatedRepresentation & p_representation ) const
		{
			nlohmann::json json = nlohmann::json();

			const Model::Representation::Representation * const linkedRep = p_representation.getLinkedRepresentation();
			const int											presetIndex
				= Model::Representation::RepresentationLibrary::get().getRepresentationIndex( linkedRep );

			json[ "REPRESENTATION_PRESET_INDEX" ] = presetIndex;

			if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_ADD ) )
				json[ "SPHERE_RADIUS_ADD" ] = p_representation.getSphereData()._radiusAdd;
			if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED ) )
				json[ "SPHERE_RADIUS_FIXED" ] = p_representation.getSphereData()._radiusFixed;
			if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::CYLINDER_RADIUS ) )
				json[ "CYLINDER_RADIUS" ] = p_representation.getCylinderData()._radius;
			if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::COLOR_MODE ) )
				json[ "COLOR_MODE" ] = p_representation.getColorMode();
			if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::SS_COLOR_MODE ) )
				json[ "SS_COLOR_MODE" ] = p_representation.getSecondaryStructureColorMode();
			if ( p_representation.isMemberOverrided( Model::Representation::MEMBER_FLAG::COLOR ) )
				json[ "COLOR" ] = serialize( p_representation.getColor() );

			return json;
		}

		nlohmann::json Serializer::serialize( const Model::Representation::Representation & p_representation ) const
		{
			return {
				{ "NAME", p_representation.getName() },
				{ "COLOR", serialize( p_representation.getColor() ) },
				{ "QUICK_ACCESS", p_representation.hasQuickAccess() },
				{ "TYPE", p_representation.getRepresentationType() },
				{ "SPHERE_RADIUS", p_representation.getData().getSphereRadius() },
				{ "CYLINDER_RADIUS", p_representation.getData().getCylinderRadius() },
				{ "COLOR_MODE", p_representation.getData().getColorMode() },
				{ "SS_COLOR_MODE", p_representation.getData().getSecondaryStructureColorMode() },
			};
		}
		nlohmann::json Serializer::serialize( const Model::Renderer::RenderEffectPreset & p_preset ) const
		{
			return {
				{ "NAME", p_preset.getName() },
				{ "QUICK_ACCESS", p_preset.hasQuickAccess() },
				{ "SHADING", p_preset.getShading() },
				{ "SSAO", p_preset.isSSAOEnabled() },
				{ "SSAO_INTENSITY", p_preset.getSSAOIntensity() },
				{ "SSAO_BLUR_SIZE", p_preset.getSSAOBlurSize() },
				{ "OUTLINE", p_preset.isOutlineEnabled() },
				{ "OUTLINE_THICKNESS", p_preset.getOutlineThickness() },
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
			return { { "SYMBOL_DISPLAY_MODE", p_setting.symbolDisplayMode },

					 { "ACTIVE_RENDERER", p_setting.activeRenderer },
					 { "BACKGROUND_COLOR", serialize( p_setting.backgroundColor ) },
					 { "REPRESENTATION", p_setting.representation },
					 { "ATOMS_RADIUS", p_setting.atomsRadius },
					 { "BONDS_RADIUS", p_setting.bondsRadius },
					 { "COLOR_MODE", p_setting.colorMode },
					 { "SHADING", p_setting.shading },
					 { "ACTIVE_VSYNC", p_setting.activeVSync },
					 { "ACTIVE_AO", p_setting.activeAO },
					 { "AO_INTENSITY", p_setting.aoIntensity },
					 { "AO_BLUR_SIZE", p_setting.aoBlurSize },
					 { "ACTIVE_OUTLINE", p_setting.activeOutline },
					 { "OUTLINE_COLOR", serialize( p_setting.outlineColor ) },
					 { "ACTIVE_FOG", p_setting.activeFog },
					 { "FOG_NEAR", p_setting.fogNear },
					 { "FOG_FAR", p_setting.fogFar },
					 { "FOG_DENSITY", p_setting.fogDensity },
					 { "FOG_COLOR", serialize( p_setting.fogColor ) },
					 { "ACTIVE_AA", p_setting.activeAA },
					 { "LIGHT_COLOR", serialize( p_setting.lightColor ) },

					 { "CAMERA_NEAR", p_setting.cameraNear },
					 { "CAMERA_FAR", p_setting.cameraFar },
					 { "CAMERA_FOV", p_setting.cameraFov },
					 { "CAMERA_PERSPECTIVE", p_setting.cameraPerspective },

					 { "CONTROLLER_TRANSLATION_SPEED", p_setting.translationSpeed },
					 { "CONTROLLER_TRANSLATION_FACTOR", p_setting.translationFactorSpeed },
					 { "CONTROLLER_ROTATION_SPEED", p_setting.rotationSpeed },
					 { "CONTROLLER_Y_AXIS_INVERTED", p_setting.yAxisInverted },

					 { "AUTO_ROTATE_SPEED", serialize( p_setting.autoRotationSpeed ) } };
		}

		void Serializer::deserialize( const nlohmann::json & p_json, VTXApp & p_app ) const
		{
			deserialize( p_json.at( "SCENE" ), p_app.getScene() );
		}

		void Serializer::deserialize( const nlohmann::json & p_json, Object3D::Scene & p_scene ) const
		{
			Vec3f cameraPos;
			deserialize( p_json.at( "CAMERA_POSITION" ), cameraPos );
			Quatf cameraRot;
			deserialize( p_json.at( "CAMERA_ROTATION" ), cameraRot );

			for ( const nlohmann::json & jsonMolecule : p_json.at( "MOLECULES" ) )
			{
				Model::Molecule * const molecule = MVC::MvcManager::get().instantiateModel<Model::Molecule>();
				deserialize( jsonMolecule, *molecule );

				VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_CREATED, molecule ) );
				p_scene.addMolecule( molecule );
			}

			for ( const nlohmann::json & jsonPath : p_json.at( "PATHS" ) )
			{
				Model::Path * const path = MVC::MvcManager::get().instantiateModel<Model::Path>();
				deserialize( jsonPath, *path );
				p_scene.addPath( path );
			}

			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->resetCameraController();
			p_scene.getCamera().setPosition( cameraPos );
			p_scene.getCamera().setRotation( cameraRot );
		}

		void Serializer::deserialize( const nlohmann::json & p_json, Model::Molecule & p_molecule ) const
		{
			Math::Transform transform;
			deserialize( p_json.at( "TRANSFORM" ), transform );
			p_molecule.applyTransform( transform );

			const std::string		 buffer = p_json.at( "DATA" ).get<std::string>();
			IO::Reader::LibChemfiles reader = IO::Reader::LibChemfiles( nullptr );
			reader.readBuffer( buffer, "mol.pdb", p_molecule );

			_deserializeMoleculeRepresentations( p_json.at( "REPRESENTATIONS" ), p_molecule );
			_deserializeMoleculeVisibilities( p_json.at( "VISIBILITIES" ), p_molecule );

			p_molecule.setName( p_json.at( "NAME" ).get<std::string>() );
			p_molecule.setPdbIdCode( p_json.at( "PDB_ID" ).get<std::string>() );
		}

		void Serializer::deserialize( const nlohmann::json & p_json, Model::Path & p_path ) const
		{
			p_path.setDurationMode( p_json.at( "MODE_DURATION" ).get<Model::Path::DURATION_MODE>() );
			p_path.setInterpolationMode( p_json.at( "MODE_INTERPOLATION" ).get<Model::Path::INTERPOLATION_MODE>() );
			p_path.setDuration( p_json.at( "DURATION" ).get<float>() );
			p_path.setIsLooping( p_json.at( "IS_LOOPING" ).get<bool>() );

			for ( const nlohmann::json & jsonViewpoint : p_json.at( "VIEWPOINTS" ) )
			{
				Model::Viewpoint * const viewpoint
					= MVC::MvcManager::get().instantiateModel<Model::Viewpoint>( &p_path );
				deserialize( jsonViewpoint, *viewpoint );
				p_path.addViewpoint( viewpoint );
			}

			p_path.refreshAllDurations();
		}

		void Serializer::deserialize( const nlohmann::json & p_json, Model::Viewpoint & p_viewpoint ) const
		{
			p_viewpoint.setDuration( p_json.at( "DURATION" ).get<float>() );
			Vec3f position;
			deserialize( p_json.at( "POSITION" ), position );
			p_viewpoint.setPosition( position );
			Vec3f target;
			deserialize( p_json.at( "TARGET" ), target );
			p_viewpoint.setTarget( target );
			Quatf rotation;
			deserialize( p_json.at( "ROTATION" ), rotation );
			p_viewpoint.setRotation( rotation );
			p_viewpoint.setDistance( p_json.at( "DISTANCE" ).get<float>() );
			p_viewpoint.setController( p_json.at( "CONTROLLER" ).get<ID::VTX_ID>() );

			for ( const nlohmann::json & jsonAction : p_json.at( "ACTIONS" ) )
			{
				std::string action = jsonAction.get<std::string>();
				std::replace( action.begin(), action.end(), '-', ' ' );
				p_viewpoint.addAction( action );
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
				p_representation.setColorMode( p_json.at( "COLOR_MODE" ).get<Generic::COLOR_MODE>() );
			}
			if ( p_json.contains( "SS_COLOR_MODE" ) )
			{
				p_representation.setSecondaryStructureColorMode(
					p_json.at( "SS_COLOR_MODE" ).get<Generic::SECONDARY_STRUCTURE_COLOR_MODE>() );
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
			p_representation.setName( p_json.at( "NAME" ).get<std::string>() );
			Color::Rgb color;
			deserialize( p_json.at( "COLOR" ), color );
			p_representation.setColor( color );
			p_representation.setQuickAccess( p_json.at( "QUICK_ACCESS" ).get<bool>() );

			p_representation.changeRepresentationType( p_json.at( "TYPE" ).get<Generic::REPRESENTATION>(), false );
			p_representation.getData().setSphereRadius( p_json.at( "SPHERE_RADIUS" ).get<float>() );
			p_representation.getData().setCylinderRadius( p_json.at( "CYLINDER_RADIUS" ).get<float>() );
			p_representation.getData().setColorMode( p_json.at( "COLOR_MODE" ).get<Generic::COLOR_MODE>() );
			p_representation.getData().setSecondaryStructureColorMode(
				p_json.at( "SS_COLOR_MODE" ).get<Generic::SECONDARY_STRUCTURE_COLOR_MODE>() );
		}
		void Serializer::deserialize( const nlohmann::json &				p_json,
									  Model::Renderer::RenderEffectPreset & p_preset ) const
		{
			Color::Rgb color;

			p_preset.setName( p_json.at( "NAME" ).get<std::string>() );
			p_preset.setQuickAccess( p_json.at( "QUICK_ACCESS" ).get<bool>() );
			p_preset.setShading( p_json.at( "SHADING" ).get<Renderer::SHADING>() );
			p_preset.enableSSAO( p_json.at( "SSAO" ).get<bool>() );
			p_preset.setSSAOIntensity( p_json.at( "SSAO_INTENSITY" ).get<int>() );
			p_preset.setSSAOBlurSize( p_json.at( "SSAO_BLUR_SIZE" ).get<int>() );
			p_preset.enableOutline( p_json.at( "OUTLINE" ).get<bool>() );
			p_preset.setOutlineThickness( p_json.at( "OUTLINE_THICKNESS" ).get<float>() );
			deserialize( p_json.at( "OUTLINE_COLOR" ), color );
			p_preset.setOutlineColor( color );
			p_preset.enableFog( p_json.at( "FOG" ).get<bool>() );
			p_preset.setFogNear( p_json.at( "FOG_NEAR" ).get<float>() );
			p_preset.setFogFar( p_json.at( "FOG_FAR" ).get<float>() );
			p_preset.setFogDensity( p_json.at( "FOG_DENSITY" ).get<float>() );
			deserialize( p_json.at( "FOG_COLOR" ), color );
			p_preset.setFogColor( color );
			deserialize( p_json.at( "BACKGROUND_COLOR" ), color );
			p_preset.setBackgroundColor( color );
			deserialize( p_json.at( "CAMERA_LIGHT_COLOR" ), color );
			p_preset.setCameraLightColor( color );
			p_preset.setCameraFOV( p_json.at( "CAMERA_FOV" ).get<float>() );
			p_preset.setCameraNearClip( p_json.at( "CAMERA_NEAR_CLIP" ).get<float>() );
			p_preset.setCameraFarClip( p_json.at( "CAMERA_FAR_CLIP" ).get<float>() );
			p_preset.setAA( p_json.at( "CAMERA_AA" ).get<bool>() );
			p_preset.setPerspectiveProjection( p_json.at( "CAMERA_PERSPECTIVE_PROJECTION" ).get<bool>() );
		}

		void Serializer::deserialize( const nlohmann::json & p_json, Color::Rgb & p_color ) const
		{
			p_color.setR( p_json.at( "R" ).get<float>() );
			p_color.setG( p_json.at( "G" ).get<float>() );
			p_color.setB( p_json.at( "B" ).get<float>() );
		}

		void Serializer::deserialize( const nlohmann::json & p_json, Math::Transform & p_transform ) const
		{
			Vec3f position;
			deserialize( p_json.at( "POSITION" ), position );
			Vec3f euler;
			deserialize( p_json.at( "ROTATION" ), euler );
			Vec3f scale;
			deserialize( p_json.at( "SCALE" ), scale );

			p_transform.setTranslation( position );
			p_transform.setRotation( euler );
			p_transform.setScale( scale );
		}

		template<typename T, glm::qualifier Q>
		void Serializer::deserialize( const nlohmann::json & p_json, glm::vec<2, T, Q> & p_vec ) const
		{
			p_vec.x = p_json.at( "X" ).get<T>();
			p_vec.y = p_json.at( "Y" ).get<T>();
		}
		template<typename T, glm::qualifier Q>
		void Serializer::deserialize( const nlohmann::json & p_json, glm::vec<3, T, Q> & p_vec ) const
		{
			p_vec.x = p_json.at( "X" ).get<T>();
			p_vec.y = p_json.at( "Y" ).get<T>();
			p_vec.z = p_json.at( "Z" ).get<T>();
		}
		template<typename T, glm::qualifier Q>
		void Serializer::deserialize( const nlohmann::json & p_json, glm::vec<4, T, Q> & p_vec ) const
		{
			p_vec.x = p_json.at( "X" ).get<T>();
			p_vec.y = p_json.at( "Y" ).get<T>();
			p_vec.z = p_json.at( "Z" ).get<T>();
			p_vec.w = p_json.at( "W" ).get<T>();
		}

		template<typename T, glm::qualifier Q>
		void Serializer::deserialize( const nlohmann::json & p_json, glm::qua<T, Q> & p_quat ) const
		{
			p_quat.x = p_json.at( "X" ).get<T>();
			p_quat.y = p_json.at( "Y" ).get<T>();
			p_quat.z = p_json.at( "Z" ).get<T>();
			p_quat.w = p_json.at( "W" ).get<T>();
		}

		void Serializer::deserialize( const nlohmann::json & p_json, Setting & p_setting ) const
		{
			VTX_ACTION( new Action::Setting::ChangeDisplayMode(
				p_json.at( "SYMBOL_DISPLAY_MODE" ).get<Style::SYMBOL_DISPLAY_MODE>() ) );

			VTX_ACTION( new Action::Setting::ActiveRenderer( p_json.at( "ACTIVE_RENDERER" ).get<bool>() ) );
			Color::Rgb bgColor = Color::Rgb();
			deserialize( p_json.at( "BACKGROUND_COLOR" ), bgColor );
			VTX_ACTION( new Action::Setting::ChangeBackgroundColor( bgColor ) );
			VTX_ACTION( new Action::Setting::ChangeRepresentation( p_json.at( "REPRESENTATION" ).get<int>() ) );
			VTX_ACTION( new Action::Setting::ChangeAtomsRadius( p_json.at( "ATOMS_RADIUS" ).get<float>() ) );
			VTX_ACTION( new Action::Setting::ChangeBondsRadius( p_json.at( "BONDS_RADIUS" ).get<float>() ) );
			VTX_ACTION( new Action::Setting::ChangeColorMode( p_json.at( "COLOR_MODE" ).get<Generic::COLOR_MODE>() ) );
			VTX_ACTION( new Action::Setting::ChangeShading( p_json.at( "SHADING" ).get<Renderer::SHADING>() ) );
			VTX_ACTION( new Action::Setting::ActiveVerticalSync( p_json.at( "ACTIVE_VSYNC" ).get<bool>() ) );
			VTX_ACTION( new Action::Setting::ActiveAO( p_json.at( "ACTIVE_AO" ).get<bool>() ) );
			VTX_ACTION( new Action::Setting::ChangeAOIntensity( p_json.at( "AO_INTENSITY" ).get<int>() ) );
			VTX_ACTION( new Action::Setting::ChangeAOBlurSize( p_json.at( "AO_BLUR_SIZE" ).get<int>() ) );
			VTX_ACTION( new Action::Setting::ActiveOutline( p_json.at( "ACTIVE_OUTLINE" ).get<bool>() ) );
			Color::Rgb outlinColor = Color::Rgb();
			deserialize( p_json.at( "OUTLINE_COLOR" ), outlinColor );
			VTX_ACTION( new Action::Setting::ChangeOutlineColor( outlinColor ) );
			VTX_ACTION( new Action::Setting::ActiveFog( p_json.at( "ACTIVE_FOG" ).get<bool>() ) );
			VTX_ACTION( new Action::Setting::ChangeFogNear( p_json.at( "FOG_NEAR" ).get<float>() ) );
			VTX_ACTION( new Action::Setting::ChangeFogFar( p_json.at( "FOG_FAR" ).get<float>() ) );
			VTX_ACTION( new Action::Setting::ChangeFogDensity( p_json.at( "FOG_DENSITY" ).get<float>() ) );
			Color::Rgb fogColor = Color::Rgb();
			deserialize( p_json.at( "FOG_COLOR" ), fogColor );
			VTX_ACTION( new Action::Setting::ChangeFogColor( fogColor ) );
			VTX_ACTION( new Action::Setting::ActiveAA( p_json.at( "ACTIVE_AA" ).get<bool>() ) );
			Color::Rgb lightColor = Color::Rgb();
			deserialize( p_json.at( "LIGHT_COLOR" ), lightColor );
			VTX_ACTION( new Action::Setting::ChangeLightColor( lightColor ) );

			VTX_ACTION( new Action::Setting::ChangeCameraClip( p_json.at( "CAMERA_NEAR" ),
															   p_json.at( "CAMERA_FAR" ).get<float>() ) );
			VTX_ACTION( new Action::Setting::ChangeCameraFov( p_json.at( "CAMERA_FOV" ).get<float>() ) );
			VTX_ACTION( new Action::Setting::ChangeCameraProjection( p_json.at( "CAMERA_PERSPECTIVE" ).get<bool>() ) );

			VTX_ACTION( new Action::Setting::ChangeTranslationSpeed(
				p_json.at( "CONTROLLER_TRANSLATION_SPEED" ).get<float>() ) );
			VTX_ACTION( new Action::Setting::ChangeTranslationFactorSpeed(
				p_json.at( "CONTROLLER_TRANSLATION_FACTOR" ).get<float>() ) );
			VTX_ACTION(
				new Action::Setting::ChangeRotationSpeed( p_json.at( "CONTROLLER_ROTATION_SPEED" ).get<float>() ) );
			VTX_ACTION(
				new Action::Setting::ActiveYAxisInversion( p_json.at( "CONTROLLER_Y_AXIS_INVERTED" ).get<bool>() ) );
			Vec3f autoRotateSpeed;
			deserialize( p_json.at( "AUTO_ROTATE_SPEED" ), autoRotateSpeed );
			VTX_ACTION( new Action::Setting::ChangeAutoRotateSpeed( autoRotateSpeed ) );
		}

		nlohmann::json Serializer::_serializeMoleculeRepresentations( const Model::Molecule &		  p_molecule,
																	  const Writer::ChemfilesWriter & p_writer ) const
		{
			nlohmann::json jsonArrayRepresentations = nlohmann::json::array();
			nlohmann::json jsonRepresentation		= { { "TARGET_TYPE", ID::Model::MODEL_MOLECULE },
													{ "INDEX", 0 },
													{ "REPRESENTATION", serialize( *p_molecule.getRepresentation() ) } };
			jsonArrayRepresentations.emplace_back( jsonRepresentation );

			for ( const Model::Chain * const chain : p_molecule.getChains() )
			{
				if ( chain == nullptr )
					continue;

				const Model::Representation::InstantiatedRepresentation * const chainCustomRep
					= chain->hasCustomRepresentation() ? chain->getRepresentation() : nullptr;

				for ( uint residueIndex = 0; residueIndex < chain->getResidueCount(); residueIndex++ )
				{
					const Model::Residue * const residue
						= p_molecule.getResidue( chain->getIndexFirstResidue() + residueIndex );

					if ( residue == nullptr )
						continue;

					if ( residue->hasCustomRepresentation() )
					{
						const uint newResidueIndex = p_writer.getNewResidueIndex( *residue );
						jsonRepresentation		   = { { "TARGET_TYPE", ID::Model::MODEL_RESIDUE },
											   { "INDEX", newResidueIndex },
											   { "REPRESENTATION", serialize( *residue->getRepresentation() ) } };

						jsonArrayRepresentations.emplace_back( jsonRepresentation );
					}
					else if ( chainCustomRep != nullptr )
					{
						const uint newResidueIndex = p_writer.getNewResidueIndex( *residue );
						jsonRepresentation		   = { { "TARGET_TYPE", ID::Model::MODEL_RESIDUE },
											   { "INDEX", newResidueIndex },
											   { "REPRESENTATION", serialize( *chainCustomRep ) } };

						jsonArrayRepresentations.emplace_back( jsonRepresentation );
					}
				}
			}

			return jsonArrayRepresentations;
		}
		nlohmann::json Serializer::_serializeMoleculeVisibilities( const Model::Molecule &		   p_molecule,
																   const Writer::ChemfilesWriter & p_writer ) const
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
					if ( p_writer.isChainMerged( *chain ) )
					{
						for ( uint residueIndex = chain->getIndexFirstResidue();
							  residueIndex <= chain->getIndexLastResidue();
							  residueIndex++ )
						{
							const Model::Residue * const residue = p_molecule.getResidue( residueIndex );

							if ( residue == nullptr )
								continue;

							jsonResidueVisibilitiesArray.emplace_back( p_writer.getNewResidueIndex( *residue ) );
						}
					}
					else
					{
						jsonChainVisibilitiesArray.emplace_back( chain->getIndex() );
					}
				}
			}

			for ( const Model::Residue * const residue : p_molecule.getResidues() )
			{
				if ( residue == nullptr )
					continue;

				if ( !residue->isVisible() )
					jsonResidueVisibilitiesArray.emplace_back( p_writer.getNewResidueIndex( *residue ) );
			}

			for ( const Model::Atom * const atom : p_molecule.getAtoms() )
			{
				if ( atom == nullptr )
					continue;

				if ( !atom->isVisible() )
					jsonAtomVisibilitiesArray.emplace_back( p_writer.getNewAtomIndex( *atom ) );
			}

			return { { "CHAINS", jsonChainVisibilitiesArray },
					 { "RESIDUES", jsonResidueVisibilitiesArray },
					 { "ATOMS", jsonAtomVisibilitiesArray } };
		}

		void Serializer::_deserializeMoleculeRepresentations( const nlohmann::json & p_json,
															  Model::Molecule &		 p_molecule ) const
		{
			for ( const nlohmann::json & jsonRepresentations : p_json )
			{
				const ID::VTX_ID type  = jsonRepresentations.at( "TARGET_TYPE" ).get<ID::VTX_ID>();
				const uint		 index = jsonRepresentations.at( "INDEX" ).get<uint>();

				const bool dataValid = ( type == ID::Model::MODEL_MOLECULE )
									   || ( type == ID::Model::MODEL_CHAIN && index < p_molecule.getChainCount() )
									   || ( type == ID::Model::MODEL_RESIDUE && index < p_molecule.getResidueCount() );

				// Currently prevent app from crash when lodading out of range (because of deletion)
				if ( !dataValid )
					continue;

				Model::Representation::InstantiatedRepresentation * const representation
					= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>();
				deserialize( jsonRepresentations.at( "REPRESENTATION" ), *representation );

				if ( type == ID::Model::MODEL_MOLECULE )
				{
					Representation::RepresentationManager::get().assignRepresentation(
						representation, p_molecule, false, false );
				}
				else if ( type == ID::Model::MODEL_CHAIN )
				{
					Representation::RepresentationManager::get().assignRepresentation(
						representation, *p_molecule.getChain( index ), false, false );
				}
				else if ( type == ID::Model::MODEL_RESIDUE )
				{
					Representation::RepresentationManager::get().assignRepresentation(
						representation, *p_molecule.getResidue( index ), false, false );
				}
			}
		}
		void Serializer::_deserializeMoleculeVisibilities( const nlohmann::json & p_json,
														   Model::Molecule &	  p_molecule ) const
		{
			for ( const uint invisibleChainIndex : p_json.at( "CHAINS" ) )
			{
				p_molecule.getChain( invisibleChainIndex )->setVisible( false );
			}
			for ( const uint invisibleResidueIndex : p_json.at( "RESIDUES" ) )
			{
				p_molecule.getResidue( invisibleResidueIndex )->setVisible( false );
			}
			for ( const uint invisibleAtomIndex : p_json.at( "ATOMS" ) )
			{
				p_molecule.getAtom( invisibleAtomIndex )->setVisible( false );
			}
		}
	} // namespace IO
} // namespace VTX
