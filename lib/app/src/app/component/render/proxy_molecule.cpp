#include "app/component/render/proxy_molecule.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/component/scene/transform_component.hpp"
#include <core/chemdb/atom.hpp>
#include <renderer/facade.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Render
{
	ProxyMolecule::ProxyMolecule() {}
	ProxyMolecule::~ProxyMolecule()
	{
		if ( _proxyPtr != nullptr )
		{
			_proxyPtr->onRemove();
		}
	}
	void ProxyMolecule::setup( Renderer::Facade & p_renderer )
	{
		_addInRenderer( p_renderer );
		_setupCallbacks();
	}

	void ProxyMolecule::_addInRenderer( Renderer::Facade & p_renderer )
	{
		Component::Chemistry::Molecule & molComp
			= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( *this );
		VTX::Core::Struct::Molecule & molStruct = molComp._moleculeStruct;

		Component::Scene::Transform & transformComp
			= MAIN_REGISTRY().getComponent<Component::Scene::Transform>( *this );

		const std::vector<uchar> atomColors	   = _generateAtomColors( molStruct );
		const std::vector<float> atomRadii	   = _generateAtomRadii( molStruct );
		const std::vector<uint>	 atomIds	   = _generateAtomUids( molComp );
		const std::vector<uchar> residueColors = _generateResidueColors( molStruct );
		const std::vector<uint>	 residueIds	   = _generateResidueUids( molComp );

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

		p_renderer.addProxyMolecule( *_proxyPtr );
	}
	void ProxyMolecule::_setupCallbacks()
	{
		Component::Chemistry::Molecule & molecule
			= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( *this );

		molecule.onVisibilityChange +=
			[ this ](
				Component::Chemistry::AtomIndexRangeList p_rangeList, App::Core::VISIBILITY_APPLY_MODE p_applyMode
			) { _applyOnVisibility( p_rangeList, p_applyMode ); };
	}

	std::vector<uchar> ProxyMolecule::_generateAtomColors( const VTX::Core::Struct::Molecule & p_molStruct ) const
	{
		std::vector<uchar> atomColors;
		atomColors.resize( p_molStruct.getAtomCount(), 0 );

		return atomColors;
	}
	std::vector<float> ProxyMolecule::_generateAtomRadii( const VTX::Core::Struct::Molecule & p_molStruct ) const
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
	std::vector<uint> ProxyMolecule::_generateAtomUids( const Component::Chemistry::Molecule & p_molComp ) const
	{
		std::vector<uint> atomUids;
		const uint		  offset = uint( p_molComp._atomUidRange.getFirst() );

		atomUids.resize( p_molComp.getAtoms().size(), Core::UID::INVALID_UID );
		std::generate( atomUids.begin(), atomUids.end(), [ this, offset, i = 0 ]() mutable { return offset + i++; } );

		return atomUids;
	}
	std::vector<uchar> ProxyMolecule::_generateResidueColors( const VTX::Core::Struct::Molecule & p_molStruct ) const
	{
		std::vector<uchar> residueColors;
		residueColors.resize( p_molStruct.getResidueCount(), 0 );

		return residueColors;
	}
	std::vector<uint> ProxyMolecule::_generateResidueUids( const Component::Chemistry::Molecule & p_molComp ) const
	{
		std::vector<uint> residueUids;
		const uint		  offset = uint( p_molComp._residueUidRange.getFirst() );

		residueUids.resize( p_molComp.getResidues().size(), Core::UID::INVALID_UID );
		std::generate(
			residueUids.begin(), residueUids.end(), [ this, offset, i = 0 ]() mutable { return offset + i++; }
		);

		return residueUids;
	}

	void ProxyMolecule::_applyOnVisibility(
		const Component::Chemistry::AtomIndexRangeList & p_rangeList,
		const App::Core::VISIBILITY_APPLY_MODE			 p_applyMode
	)
	{
		switch ( p_applyMode )
		{
		case App::Core::VISIBILITY_APPLY_MODE::SHOW: _proxyPtr->onAtomVisibilities( p_rangeList, true ); break;

		case App::Core::VISIBILITY_APPLY_MODE::HIDE: _proxyPtr->onAtomVisibilities( p_rangeList, false ); break;

		case App::Core::VISIBILITY_APPLY_MODE::SET:
			_proxyPtr->onVisible( false );
			_proxyPtr->onAtomVisibilities( p_rangeList, true );
			break;

		default:
			throw VTXException(
				"Unhandled Component::Chemistry::VISIBILITY_APPLY_MODE in ProxyMolecule::onVisibilityChange "
				"callback."
			);
		}
	}

} // namespace VTX::App::Component::Render
