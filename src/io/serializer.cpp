#include "serializer.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "io/reader/lib_chemfiles.hpp"
#include "io/writer/writer_chemfiles.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "model/viewpoint.hpp"
#include "mvc/mvc_manager.hpp"

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

			return {
				{ "CAMERA_POSITION", serialize( p_scene.getCamera().getPosition() ) },
				{ "CAMERA_ROTATION", serialize( p_scene.getCamera().getRotation() ) },
				/*{ "MOLECULES", jsonArrayMolecules },
				{ "PATHS", jsonArrayPaths }*/
			};
		}

		nlohmann::json Serializer::serialize( const Model::Molecule & p_molecule ) const
		{
			Writer::ChemfilesWriter chemfileWriter = Writer::ChemfilesWriter();
			std::string				buffer		   = std::string();
			chemfileWriter.writeBuffer( buffer, p_molecule );

			return { { "MOLECULE_DATA", buffer } };
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

		nlohmann::json Serializer::serialize( const Color::Rgb & p_color ) const
		{
			return { { "R", p_color.getR() }, { "G", p_color.getG() }, { "B", p_color.getB() } };
		}

		template<int L, typename T, glm::qualifier Q>
		nlohmann::json Serializer::serialize( const glm::vec<L, T, Q> & p_vec ) const
		{
			return nlohmann::json( { { "X", p_vec.x }, { "Y", p_vec.y }, { "Z", p_vec.z } } );
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
					 { "BACKGROUND_OPACITY", p_setting.backgroundOpacity },
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
					 { "OUTLINE_THICKNESS", p_setting.outlineThickness },
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

			p_scene.getCamera().setPosition( cameraPos );
			p_scene.getCamera().setRotation( cameraRot );

			// for ( const nlohmann::json & jsonMolecule : p_json.at( "MOLECULES" ) )
			//{
			//	Model::Molecule * const molecule = MVC::MvcManager::get().instantiateModel<Model::Molecule>();
			//	deserialize( jsonMolecule, *molecule );
			//	p_scene.addMolecule( molecule );
			//}

			// for ( const nlohmann::json & jsonPath : p_json.at( "PATHS" ) )
			//{
			//	Model::Path * const path = MVC::MvcManager::get().instantiateModel<Model::Path>();
			//	deserialize( jsonPath, *path );
			//	p_scene.addPath( path );
			//}
		}

		void Serializer::deserialize( const nlohmann::json & p_json, Model::Molecule & p_molecule ) const
		{
			const std::string		 buffer = p_json.at( "MOLECULE_DATA" ).get<std::string>();
			IO::Reader::LibChemfiles reader = IO::Reader::LibChemfiles( nullptr );
			reader.readBuffer( buffer, "mol.pdb", p_molecule );
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

		void Serializer::deserialize( const nlohmann::json & p_json, Color::Rgb & p_color ) const
		{
			p_color.setR( p_json.at( "R" ).get<float>() );
			p_color.setG( p_json.at( "G" ).get<float>() );
			p_color.setB( p_json.at( "B" ).get<float>() );
		}

		template<int L, typename T, glm::qualifier Q>
		void Serializer::deserialize( const nlohmann::json & p_json, glm::vec<L, T, Q> & p_vec ) const
		{
			p_vec.x = p_json.at( "X" ).get<T>();
			p_vec.y = p_json.at( "Y" ).get<T>();
			p_vec.z = p_json.at( "Z" ).get<T>();
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
			VTX_ACTION(
				new Action::Setting::ChangeBackgroundOpacity( p_json.at( "BACKGROUND_OPACITY" ).get<float>() ) );
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
			VTX_ACTION( new Action::Setting::ChangeOutlineThickness( p_json.at( "OUTLINE_THICKNESS" ).get<float>() ) );
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

	} // namespace IO
} // namespace VTX
