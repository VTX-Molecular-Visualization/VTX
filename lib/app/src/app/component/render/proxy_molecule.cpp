#include "app/component/render/proxy_molecule.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/vtx_app.hpp"
#include <core/chemdb/atom.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::App::Component::Render
{
	ProxyMolecule::ProxyMolecule() {}

	void ProxyMolecule::init()
	{
		Component::Chemistry::Molecule & molComp
			= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( *this );
		VTX::Core::Struct::Molecule & molStruct = molComp._moleculeStruct;

		Component::Scene::Transform & transformComp
			= MAIN_REGISTRY().getComponent<Component::Scene::Transform>( *this );

		const std::vector<uchar> atomColors	   = generateAtomColors( molStruct );
		const std::vector<float> atomRadii	   = generateAtomRadii( molStruct );
		const std::vector<uint>	 atomIds	   = generateAtomUids( molStruct );
		const std::vector<uchar> residueColors = generateResidueColors( molStruct );
		const std::vector<uint>	 residueIds	   = generateResidueUids( molStruct );

		_proxyPtr = std::make_unique<VTX::Renderer::Proxy::Molecule>( VTX::Renderer::Proxy::Molecule {
			&transformComp.getTransform().get(),

			&molStruct.trajectory.getCurrentFrame(),
			&molStruct.bondPairAtomIndexes,
			&molStruct.atomNames,
			reinterpret_cast<const std::vector<uchar> *>( &molStruct.residueSecondaryStructureTypes ),
			&molStruct.residueFirstAtomIndexes,
			&molStruct.residueAtomCounts,
			&molStruct.chainFirstResidues,
			&molStruct.chainResidueCounts,

			atomColors,
			atomRadii,
			atomIds,
			residueColors,
			residueIds } );
	}

	std::vector<uchar> ProxyMolecule::generateAtomColors( const VTX::Core::Struct::Molecule & p_molStruct ) const
	{
		std::vector<uchar> atomColors;
		atomColors.resize( p_molStruct.getAtomCount(), 0 );

		return atomColors;
	}
	std::vector<float> ProxyMolecule::generateAtomRadii( const VTX::Core::Struct::Molecule & p_molStruct ) const
	{
		std::vector<float> atomRadii;

		atomRadii.resize( p_molStruct.getAtomCount(), 0.f );
		std::generate(
			atomRadii.begin(),
			atomRadii.end(),
			[ this, &p_molStruct, i = 0 ]() mutable
			{ return VTX::Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( p_molStruct.atomSymbols[ i ] ) ]; }
		);

		return atomRadii;
	}
	std::vector<uint> ProxyMolecule::generateAtomUids( const Component::Chemistry::Molecule & p_molComp ) const
	{
		std::vector<uint> atomUids;
		const uint		  offset = uint( p_molComp._atomUidRange.getFirst() );

		atomUids.resize( p_molComp.getAtoms().size(), Core::UID::INVALID_UID );
		std::generate( atomUids.begin(), atomUids.end(), [ this, offset, i = 0 ]() mutable { return offset + i++; } );

		return atomUids;
	}
	std::vector<uchar> ProxyMolecule::generateResidueColors( const VTX::Core::Struct::Molecule & p_molStruct ) const
	{
		std::vector<uchar> residueColors;
		residueColors.resize( p_molStruct.getResidueCount(), 0 );

		return residueColors;
	}
	std::vector<uint> ProxyMolecule::generateResidueUids( const Component::Chemistry::Molecule & p_molComp ) const
	{
		std::vector<uint> residueUids;
		const uint		  offset = uint( p_molComp._residueUidRange.getFirst() );

		residueUids.resize( p_molComp.getResidues().size(), Core::UID::INVALID_UID );
		std::generate(
			residueUids.begin(), residueUids.end(), [ this, offset, i = 0 ]() mutable { return offset + i++; }
		);

		return residueUids;
	}
} // namespace VTX::App::Component::Render
