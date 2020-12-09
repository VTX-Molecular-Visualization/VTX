#include "base_representable.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation_library.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Generic
	{
		const std::set<const Model::Representation::BaseRepresentation *> & BaseRepresentable::getRepresentations() const
		{
			if ( _representations.size() == 0 )
				return Representation::RepresentationManager::get().getDefaultRepresentationSet();
			else
				return _representations;
		}

		void BaseRepresentable::computeRepresentationTargets()
		{
			_molecule->_representationTargets.clear();

			const Model::SecondaryStructure & secondaryStructure		   = _molecule->getSecondaryStructure();
			const std::map<uint, uint>		  residueToControlPointIndices = secondaryStructure.getResidueToControlPointIndice();

			for ( Model::Residue * const residue : _molecule->getResidues() )
			{
				// Skip hidden items.
				if ( residue->isVisible() == false || residue->getChainPtr()->isVisible() == false || residue->getMoleculePtr()->isVisible() == false )
					continue;

				if ( _molecule->showSolvent() == false && residue->getAtomType() == Model::Atom::TYPE::SOLVENT )
					continue;

				if ( _molecule->showIon() == false && residue->getAtomType() == Model::Atom::TYPE::ION )
					continue;

				const std::set<const Model::Representation::BaseRepresentation *> * currentRepresentations;
				if ( residue->_representations.size() > 0 )
					currentRepresentations = &( residue->_representations );
				else
					currentRepresentations = &( _molecule->getRepresentations() );

				for ( const Model::Representation::BaseRepresentation * const representation : *currentRepresentations )
				{
					if ( _molecule->_representationTargets.find( representation ) == _molecule->_representationTargets.end() )
						_molecule->_representationTargets.emplace( representation, VTX::Representation::RepresentationTarget() );

					VTX::Representation::RepresentationTarget & representationTargets = _molecule->_representationTargets.at( representation );
					const VTX::Representation::FlagDataTargeted		dataFlag			  = representation->getFlagDataTargeted();

					if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::ATOM ) )
					{
						const std::pair<uint, uint> rangeAtoms = std::pair( residue->getIndexFirstAtom(), residue->getAtomCount() );
						representationTargets.appendAtoms( rangeAtoms );
					}
					if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::BOND ) )
					{
						const std::pair<uint, uint> rangeBonds = std::pair( residue->getIndiceFirstBond(), residue->getBondIndiceCount() );
						representationTargets.appendBonds( rangeBonds, residue->getIndexExtraBondStart(), residue->getIndexExtraBondEnd() );
					}
					if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::RIBBON ) )
					{
						std::pair<uint, uint> rangeRibbons = std::pair( 0, 0 );
						if ( residueToControlPointIndices.find( residue->getIndex() ) != residueToControlPointIndices.end() )
							rangeRibbons = std::pair( residueToControlPointIndices.at( residue->getIndex() ), 4 );

						representationTargets.appendRibbons( rangeRibbons );
					}
				}
			}
		}
	} // namespace Generic
} // namespace VTX
