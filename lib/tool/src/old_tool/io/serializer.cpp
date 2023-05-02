#include "tool/old_tool/io/serializer.hpp"
#include "tool/old_tool/model/measurement/angle.hpp"
#include "tool/old_tool/model/measurement/dihedral_angle.hpp"
#include "tool/old_tool/model/measurement/distance.hpp"
#include "tool/old_tool/model/measurement/distance_to_cycle.hpp"
#include <app/component/object3d/label.hpp>

namespace VTX::IO
{
	// nlohmann::json Serializer::serialize( const App::Component::Object3D::Label & p_label ) const
	//{
	//	const ID::VTX_ID & labelTypeID = p_label.getTypeId();

	//	if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DISTANCE )
	//		return serialize( dynamic_cast<const Model::Measurement::Distance &>( p_label ) );
	//	else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE )
	//		return serialize( dynamic_cast<const Model::Measurement::DistanceToCycle &>( p_label ) );
	//	else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_ANGLE )
	//		return serialize( dynamic_cast<const Model::Measurement::Angle &>( p_label ) );
	//	else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
	//		return serialize( dynamic_cast<const Model::Measurement::DihedralAngle &>( p_label ) );
	//	else
	//		return {};
	//}

	nlohmann::json Serializer::serialize( const Model::Measurement::Distance & p_distanceLabel ) const
	{
		return {};
		// return { { "TYPE_ID", p_distanceLabel.getTypeId() },
		//		 { "NAME", p_distanceLabel.getDefaultName() },
		//		 { "ENABLED", p_distanceLabel.isEnable() },
		//		 { "COLOR", serialize( p_distanceLabel.getColor() ) },
		//		 { "ATOM_1_ID", serializeAtomReference( p_distanceLabel.getFirstAtom() ) },
		//		 { "ATOM_2_ID", serializeAtomReference( p_distanceLabel.getSecondAtom() ) },
		//		 { "AUTO_NAMING", p_distanceLabel.hasAutoNaming() },
		//		 { "PERSISTENT_SCENE_ID", p_distanceLabel.getPersistentSceneID() } };
	}
	nlohmann::json Serializer::serialize( const Model::Measurement::DistanceToCycle & ) const { return {}; }
	nlohmann::json Serializer::serialize( const Model::Measurement::Angle & p_angleLabel ) const
	{
		return {};
		// return { { "TYPE_ID", p_angleLabel.getTypeId() },
		//		 { "NAME", p_angleLabel.getDefaultName() },
		//		 { "ENABLED", p_angleLabel.isEnable() },
		//		 { "COLOR", serialize( p_angleLabel.getColor() ) },
		//		 { "ATOM_1_ID", serializeAtomReference( p_angleLabel.getFirstAtom() ) },
		//		 { "ATOM_2_ID", serializeAtomReference( p_angleLabel.getSecondAtom() ) },
		//		 { "ATOM_3_ID", serializeAtomReference( p_angleLabel.getThirdAtom() ) },
		//		 { "AUTO_NAMING", p_angleLabel.hasAutoNaming() },
		//		 { "PERSISTENT_SCENE_ID", p_angleLabel.getPersistentSceneID() } };
	}
	nlohmann::json Serializer::serialize( const Model::Measurement::DihedralAngle & p_angleLabel ) const
	{
		return {};
		// return { { "TYPE_ID", p_angleLabel.getTypeId() },
		//		 { "NAME", p_angleLabel.getDefaultName() },
		//		 { "ENABLED", p_angleLabel.isEnable() },
		//		 { "COLOR", serialize( p_angleLabel.getColor() ) },
		//		 { "ATOM_1_ID", serializeAtomReference( p_angleLabel.getFirstAtom() ) },
		//		 { "ATOM_2_ID", serializeAtomReference( p_angleLabel.getSecondAtom() ) },
		//		 { "ATOM_3_ID", serializeAtomReference( p_angleLabel.getThirdAtom() ) },
		//		 { "ATOM_4_ID", serializeAtomReference( p_angleLabel.getFourthAtom() ) },
		//		 { "AUTO_NAMING", p_angleLabel.hasAutoNaming() },
		//		 { "PERSISTENT_SCENE_ID", p_angleLabel.getPersistentSceneID() } };
	}

	void Serializer::deserialize( const nlohmann::json & p_json, Model::Measurement::Distance & p_distanceLabel ) const
	{
		// if ( p_json.contains( "AUTO_NAMING" ) )
		//	p_distanceLabel.setAutoNaming( p_json.at( "AUTO_NAMING" ).get<bool>() );

		// if ( !p_distanceLabel.hasAutoNaming() && p_json.contains( "NAME" ) )
		//	p_distanceLabel.setName( p_json.at( "NAME" ).get<std::string>() );

		// if ( p_json.contains( "ENABLED" ) )
		//	p_distanceLabel.setEnable( p_json.at( "ENABLED" ).get<bool>() );

		// if ( p_json.contains( "COLOR" ) )
		//{
		//	Util::Color::Rgba color;
		//	deserialize( p_json.at( "COLOR" ), color );
		//	p_distanceLabel.setColor( color );
		// }

		// if ( p_json.contains( "ATOM_1_ID" ) && p_json.contains( "ATOM_2_ID" ) )
		//{
		//	const App::Component::Chemistry::Atom * atom1 = deserializeAtomReference( p_json.at( "ATOM_1_ID" ) );
		//	const App::Component::Chemistry::Atom * atom2 = deserializeAtomReference( p_json.at( "ATOM_2_ID" ) );

		//	if ( atom1 != nullptr && atom2 != nullptr )
		//		p_distanceLabel.setAtoms( *atom1, *atom2 );
		//}

		// if ( p_json.contains( "PERSISTENT_SCENE_ID" ) )
		//	p_distanceLabel.setPersistentSceneID( _get<int>( p_json, "PERSISTENT_SCENE_ID" ) );
	}
	void Serializer::deserialize( const nlohmann::json &, Model::Measurement::DistanceToCycle & ) const {}
	void Serializer::deserialize( const nlohmann::json & p_json, Model::Measurement::Angle & p_angleLabel ) const
	{
		// if ( p_json.contains( "AUTO_NAMING" ) )
		//	p_angleLabel.setAutoNaming( p_json.at( "AUTO_NAMING" ).get<bool>() );

		// if ( !p_angleLabel.hasAutoNaming() && p_json.contains( "NAME" ) )
		//	p_angleLabel.setName( p_json.at( "NAME" ).get<std::string>() );

		// if ( p_json.contains( "ENABLED" ) )
		//	p_angleLabel.setEnable( p_json.at( "ENABLED" ).get<bool>() );

		// if ( p_json.contains( "COLOR" ) )
		//{
		//	Util::Color::Rgba color;
		//	deserialize( p_json.at( "COLOR" ), color );
		//	p_angleLabel.setColor( color );
		// }

		// if ( p_json.contains( "ATOM_1_ID" ) && p_json.contains( "ATOM_2_ID" ) && p_json.contains( "ATOM_3_ID" ) )
		//{
		//	const App::Component::Chemistry::Atom * atom1 = deserializeAtomReference( p_json.at( "ATOM_1_ID" ) );
		//	const App::Component::Chemistry::Atom * atom2 = deserializeAtomReference( p_json.at( "ATOM_2_ID" ) );
		//	const App::Component::Chemistry::Atom * atom3 = deserializeAtomReference( p_json.at( "ATOM_3_ID" ) );

		//	if ( atom1 != nullptr && atom2 != nullptr && atom3 != nullptr )
		//		p_angleLabel.setAtoms( *atom1, *atom2, *atom3 );
		//}

		// if ( p_json.contains( "PERSISTENT_SCENE_ID" ) )
		//	p_angleLabel.setPersistentSceneID( _get<int>( p_json, "PERSISTENT_SCENE_ID" ) );
	}
	void Serializer::deserialize( const nlohmann::json &			  p_json,
								  Model::Measurement::DihedralAngle & p_angleLabel ) const
	{
		// if ( p_json.contains( "AUTO_NAMING" ) )
		//	p_angleLabel.setAutoNaming( p_json.at( "AUTO_NAMING" ).get<bool>() );

		// if ( !p_angleLabel.hasAutoNaming() && p_json.contains( "NAME" ) )
		//	p_angleLabel.setName( p_json.at( "NAME" ).get<std::string>() );

		// if ( p_json.contains( "ENABLED" ) )
		//	p_angleLabel.setEnable( p_json.at( "ENABLED" ).get<bool>() );

		// if ( p_json.contains( "COLOR" ) )
		//{
		//	Util::Color::Rgba color;
		//	deserialize( p_json.at( "COLOR" ), color );
		//	p_angleLabel.setColor( color );
		// }

		// if ( p_json.contains( "ATOM_1_ID" ) && p_json.contains( "ATOM_2_ID" ) && p_json.contains( "ATOM_3_ID" )
		//	 && p_json.contains( "ATOM_4_ID" ) )
		//{
		//	const App::Component::Chemistry::Atom * atom1 = deserializeAtomReference( p_json.at( "ATOM_1_ID" ) );
		//	const App::Component::Chemistry::Atom * atom2 = deserializeAtomReference( p_json.at( "ATOM_2_ID" ) );
		//	const App::Component::Chemistry::Atom * atom3 = deserializeAtomReference( p_json.at( "ATOM_3_ID" ) );
		//	const App::Component::Chemistry::Atom * atom4 = deserializeAtomReference( p_json.at( "ATOM_4_ID" ) );

		//	if ( atom1 != nullptr && atom2 != nullptr && atom3 != nullptr && atom4 != nullptr )
		//		p_angleLabel.setAtoms( *atom1, *atom2, *atom3, *atom4 );
		//}

		// if ( p_json.contains( "PERSISTENT_SCENE_ID" ) )
		//	p_angleLabel.setPersistentSceneID( _get<int>( p_json, "PERSISTENT_SCENE_ID" ) );
	}

} // namespace VTX::IO
