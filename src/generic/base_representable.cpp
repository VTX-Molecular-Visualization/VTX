#include "base_representable.hpp"
#include "model/atom.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation_library.hpp"
#include "model/residue.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Generic
	{
		const std::set<const Model::Representation::InstantiatedRepresentation *> & BaseRepresentable::getRepresentations() const
		{
			if ( _representations.size() == 0 )
			{
				if ( _molecule->_representations.size() == 0 )
					return Representation::RepresentationManager::get().getDefaultRepresentationSet();
				else
					return _molecule->getRepresentations();
			}
			else
				return _representations;
		}

		const Model::Representation::InstantiatedRepresentation * const BaseRepresentable::getRepresentation() const
		{
			const std::set<const Model::Representation::InstantiatedRepresentation *> & representations = getRepresentations();

			const Model::Representation::InstantiatedRepresentation * res = *representations.cbegin();

			for ( const Model::Representation::InstantiatedRepresentation * representation : representations )
			{
				if ( representation->getPriority() > res->getPriority() )
					res = representation;
			}

			return res;
		}

		void BaseRepresentable::computeRepresentationTargets()
		{
			_molecule->_representationTargets.clear();

			const Model::SecondaryStructure & secondaryStructure		   = _molecule->getSecondaryStructure();
			const std::map<uint, uint>		  residueToControlPointIndices = secondaryStructure.getResidueToControlPointIndice();

			for ( Model::Residue * const residue : _molecule->getResidues() )
			{
				// Skip hidden items.
				if ( !_isResidueVisible( *residue ) )
					continue;

				const Model::Representation::InstantiatedRepresentation * const representation = residue->getRepresentation();

				if ( _molecule->_representationTargets.find( representation ) == _molecule->_representationTargets.end() )
					_molecule->_representationTargets.emplace( representation, VTX::Representation::RepresentationTarget() );

				VTX::Representation::RepresentationTarget & representationTargets = _molecule->_representationTargets.at( representation );
				const VTX::Representation::FlagDataTargeted dataFlag			  = representation->getFlagDataTargeted();

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

		void BaseRepresentable::computeColorBuffer()
		{
			std::vector<Color::Rgb> p_colorBuffer = _molecule->getBufferAtomColors();

			for ( Model::Residue * const residue : _molecule->getResidues() )
			{
				// Skip hidden items.
				if ( !_isResidueVisible( *residue ) )
					continue;

				const Model::Representation::InstantiatedRepresentation * const currentRepresentation = residue->getRepresentation();

				for ( uint i = residue->getIndexFirstAtom(); i < residue->getIndexFirstAtom() + residue->getAtomCount(); i++ )
				{
					const Model::Atom & atom = _molecule->getAtom( i );
					switch ( currentRepresentation->getColorMode() )
					{
					case Generic::COLOR_MODE::ATOM:
						if ( atom.getSymbol() == Model::Atom::SYMBOL::A_C )
							p_colorBuffer[ i ] = atom.getChainPtr()->getColor();
						else
							p_colorBuffer[ i ] = atom.getColor();
						break;
					case Generic::COLOR_MODE::RESIDUE: p_colorBuffer[ i ] = atom.getResiduePtr()->getColor(); break;
					case Generic::COLOR_MODE::CHAIN: p_colorBuffer[ i ] = atom.getChainPtr()->getColor(); break;
					case Generic::COLOR_MODE::PROTEIN: p_colorBuffer[ i ] = currentRepresentation->getColor(); break;

					default: break;
					}
				}
			}

			_molecule->getBuffer()->setAtomColors( p_colorBuffer );
		}

		bool BaseRepresentable::_isResidueVisible( const Model::Residue & p_residue ) const
		{
			const Model::Molecule * const molecule = p_residue.getMoleculePtr();

			// Skip hidden items.
			if ( p_residue.isVisible() == false || p_residue.getChainPtr()->isVisible() == false || molecule->isVisible() == false )
				return false;

			const Model::Atom::TYPE atomType = p_residue.getAtomType();

			if ( molecule->showSolvent() == false && atomType == Model::Atom::TYPE::SOLVENT )
				return false;

			if ( molecule->showIon() == false && atomType == Model::Atom::TYPE::ION )
				return false;

			return true;
		}

	} // namespace Generic
} // namespace VTX
