#include "app/component/render/proxy_molecule.hpp"
#include "app/application/selection/molecule_data.hpp"
#include "app/application/selection/selection.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/scene/transform_component.hpp"
#include <core/chemdb/atom.hpp>
#include <core/chemdb/color.hpp>
#include <io/util/secondary_structure.hpp>
#include <renderer/facade.hpp>
#include <util/algorithm/range.hpp>
#include <util/exceptions.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Render
{
	ProxyMolecule::ProxyMolecule() {}
	ProxyMolecule::~ProxyMolecule() { _removeFromRenderer(); }
	void ProxyMolecule::setup( Renderer::Facade & p_renderer )
	{
		_addInRenderer( p_renderer );
		_setupCallbacks();

		ECS_REGISTRY().connectSignal<Component::Chemistry::Molecule, &ProxyMolecule::_removeFromRenderer>(
			Core::ECS::SIGNAL::DESTROY, this
		);
	}
	void ProxyMolecule::_removeFromRenderer() { _proxy->onRemove(); }

	void ProxyMolecule::_addInRenderer( Renderer::Facade & p_renderer )
	{
		Component::Chemistry::Molecule & molComp = ECS_REGISTRY().getComponent<Component::Chemistry::Molecule>( *this );
		VTX::Core::Struct::Molecule &	 molStruct = molComp._moleculeStruct;

		// TODO: how to handle this?
		IO::Util::SecondaryStructure::computeStride( molComp._moleculeStruct );

		Component::Scene::Transform & transformComp = ECS_REGISTRY().getComponent<Component::Scene::Transform>( *this );

		const std::vector<uchar> atomColors	   = _generateAtomColors( molStruct );
		const std::vector<float> atomRadii	   = _generateAtomRadii( molStruct );
		const std::vector<uint>	 atomIds	   = _generateAtomUids( molComp );
		const std::vector<uchar> residueColors = _generateResidueColors( molStruct );
		const std::vector<uint>	 residueIds	   = _generateResidueUids( molComp );

		_proxy = std::make_unique<VTX::Renderer::Proxy::Molecule>( VTX::Renderer::Proxy::Molecule {
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

		p_renderer.addProxyMolecule( *_proxy );
	}
	void ProxyMolecule::_setupCallbacks()
	{
		_applyVisibilityCallbacks();
		_applySelectionCallbacks();
		_applyAtomPositionCallbacks();
	}

	std::vector<uchar> ProxyMolecule::_generateAtomColors( const VTX::Core::Struct::Molecule & p_molStruct ) const
	{
		std::vector<uchar> atomColors;

		atomColors.resize( p_molStruct.getAtomCount(), 0 );
		std::generate(
			atomColors.begin(),
			atomColors.end(),
			[ &p_molStruct, i = 0 ]() mutable
			{ return VTX::Core::ChemDB::Color::getColorIndex( p_molStruct.atomSymbols[ i++ ] ); }
		);

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

		std::generate(
			residueColors.begin(),
			residueColors.end(),
			[ this, &p_molStruct, i = 0 ]() mutable
			{ return VTX::Core::ChemDB::Color::getColorIndex( p_molStruct.residueSecondaryStructureTypes[ i++ ] ); }
		);

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
		case App::Core::VISIBILITY_APPLY_MODE::SHOW: _proxy->onAtomVisibilities( p_rangeList, true ); break;

		case App::Core::VISIBILITY_APPLY_MODE::HIDE: _proxy->onAtomVisibilities( p_rangeList, false ); break;

		case App::Core::VISIBILITY_APPLY_MODE::SET:
			_proxy->onVisible( false );
			_proxy->onAtomVisibilities( p_rangeList, true );
			break;

		default:
			throw VTXException(
				"Unhandled Component::Chemistry::VISIBILITY_APPLY_MODE in ProxyMolecule::onVisibilityChange "
				"callback."
			);
		}
	}

	void ProxyMolecule::_applyVisibilityCallbacks()
	{
		Component::Chemistry::Molecule & molecule
			= ECS_REGISTRY().getComponent<Component::Chemistry::Molecule>( *this );

		molecule.onVisibilityChange += [ this ](
										   const Component::Chemistry::AtomIndexRangeList & p_rangeList,
										   App::Core::VISIBILITY_APPLY_MODE					p_applyMode
									   )
		{
			Component::Chemistry::Molecule & molecule
				= ECS_REGISTRY().getComponent<Component::Chemistry::Molecule>( *this );

			const Component::Chemistry::AtomIndexRangeList activeAtoms
				= Util::Algorithm::Range::intersect( p_rangeList, molecule.getActiveAtoms() );

			_applyOnVisibility( activeAtoms, p_applyMode );
		};

		molecule.onAtomRemoved += [ this ]( const Component::Chemistry::AtomIndexRangeList & p_rangeList )
		{ _applyOnVisibility( p_rangeList, App::Core::VISIBILITY_APPLY_MODE::HIDE ); };
	}
	void ProxyMolecule::_applySelectionCallbacks()
	{
		Component::Scene::Selectable & selectableComponent
			= ECS_REGISTRY().getComponent<Component::Scene::Selectable>( *this );

		selectableComponent.onSelect += [ this ]( const Application::Selection::SelectionData & p_selectionData )
		{
			const Application::Selection::MoleculeData & castedSelectionData
				= dynamic_cast<const Application::Selection::MoleculeData &>( p_selectionData );
			_proxy->onAtomSelections( castedSelectionData.getAtomIds(), true );
		};

		selectableComponent.onDeselect += [ this ]( const Application::Selection::SelectionData & p_selectionData )
		{
			const Application::Selection::MoleculeData & castedSelectionData
				= dynamic_cast<const Application::Selection::MoleculeData &>( p_selectionData );
			_proxy->onAtomSelections( castedSelectionData.getAtomIds(), false );
		};
	}
	void ProxyMolecule::_applyAtomPositionCallbacks()
	{
		if ( ECS_REGISTRY().hasComponent<Component::Chemistry::Trajectory>( *this ) )
		{
			Component::Chemistry::Trajectory & trajectoryComponent
				= ECS_REGISTRY().getComponent<Component::Chemistry::Trajectory>( *this );

			trajectoryComponent.onFrameChange += [ this ]( const size_t p_frameIndex )
			{
				Component::Chemistry::Molecule & moleculeComponent
					= ECS_REGISTRY().getComponent<Component::Chemistry::Molecule>( *this );

				_proxy->atomPositions = &moleculeComponent.getTrajectory().getCurrentFrame();
				_proxy->onAtomPositions();
			};
		}
	}

} // namespace VTX::App::Component::Render
