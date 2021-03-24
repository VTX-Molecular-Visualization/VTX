#include "residue.hpp"
#include "atom.hpp"
#include "chain.hpp"
#include "molecule.hpp"

namespace VTX
{
	namespace Model
	{
		Molecule * const Residue::getMoleculePtr() const { return _chainPtr->getMoleculePtr(); };
		void			 Residue::setChainPtr( Chain * const p_chain )
		{
			_chainPtr = p_chain;

			setParent( _chainPtr );
			setRepresentableMolecule( p_chain->getMoleculePtr() );
		}

		void Residue::setAtomCount( const uint p_count )
		{
			_atomCount	   = p_count;
			_realAtomCount = p_count;
		}
		void Residue::removeToAtoms( const uint p_atomIndex )
		{
			const Model::Molecule * const moleculePtr = getMoleculePtr();
			if ( p_atomIndex == _indexFirstAtom )
			{
				while ( _atomCount > 0 && moleculePtr->getAtom( _indexFirstAtom ) == nullptr )
				{
					_indexFirstAtom++;
					_atomCount--;
				}
			}
			else
			{
				uint lastResidueIndex = _indexFirstAtom + _atomCount - 1;
				if ( lastResidueIndex == p_atomIndex )
				{
					while ( _atomCount > 0 && moleculePtr->getAtom( lastResidueIndex ) == nullptr )
					{
						_atomCount--;
						lastResidueIndex--;
					}
				}
			}

			_realAtomCount--;
		}

		const Atom * const Residue::findFirstAtomByName( const std::string & p_name ) const
		{
			for ( uint i = 0; i < _atomCount; ++i )
			{
				const Atom * const atom = getMoleculePtr()->getAtom( _indexFirstAtom + i );
				if ( atom != nullptr && atom->getName() == p_name )
				{
					return atom;
				}
			}

			return nullptr;
		}

		void Residue::setVisible( const bool p_visible )
		{
			if ( isVisible() != p_visible )
			{
				BaseVisible ::setVisible( p_visible );
				_notifyViews( new Event::VTXEventValue<uint>( Event::Model::RESIDUE_VISIBILITY, _index ) );
				getMoleculePtr()->propagateEventToViews(
					new Event::VTXEventValue<uint>( Event::Model::RESIDUE_VISIBILITY, _index ) );
			}
		}

		const Math::AABB Residue::getAABB() const
		{
			Math::AABB aabb = Math::AABB();

			for ( uint i = 0; i < _atomCount; ++i )
			{
				const Atom * const atom = getMoleculePtr()->getAtom( _indexFirstAtom + i );

				if ( atom == nullptr )
					continue;

				aabb.extend( atom->getAABB() );
			}

			return aabb;
		}

		const Math::AABB Residue::getWorldAABB() const
		{
			Math::AABB aabb = getAABB();
			aabb.translate( getMoleculePtr()->getTransform().getTranslationVector() );
			return aabb;
		}

		void Residue::applyRepresentation(
			Generic::BaseRepresentable::InstantiatedRepresentation * const p_representation,
			const bool													   p_recompute )
		{
			BaseRepresentable::applyRepresentation( p_representation, p_recompute );
			_notifyViews( new Event::VTXEvent( Event::Model::REPRESENTATION_CHANGE ) );
		}
		void Residue::removeRepresentation()
		{
			BaseRepresentable::removeRepresentation();
			_notifyViews( new Event::VTXEvent( Event::Model::REPRESENTATION_CHANGE ) );
		}

		const std::string Residue::SYMBOL_STR[ (int)SYMBOL::COUNT ] = {
			"---", // UNKNOWN
			"ALA", // ALA
			"ARG", // ARG
			"ASN", // ASN
			"ASP", // ASP
			"CYS", // CYS
			"GLN", // GLN
			"GLU", // GLU
			"GLY", // GLY
			"HIS", // HIS
			"ILE", // ILE
			"LEU", // LEU
			"LYS", // LYS
			"MET", // MET
			"PHE", // PHE
			"PRO", // PRO
			"SER", // SER
			"THR", // THR
			"TRP", // TRP
			"TYR", // TYR
			"VAL", // VAL
			"ASX", // ASX
			"GLX", // GLX
			"WAT", // WAT
			"HOH", // HOH
			"HEM", // HEM
			"PO4", // PO4
			"MEL", // MEL
		};

		const std::string Residue::SYMBOL_SHORT_STR[ (int)SYMBOL::COUNT ] = {
			"-", // UNKNOWN
			"A", // ALA
			"R", // ARG
			"N", // ASN
			"D", // ASP
			"C", // CYS
			"Q", // GLN
			"E", // GLU
			"G", // GLY
			"H", // HIS
			"I", // ILE
			"L", // LEU
			"K", // LYS
			"M", // MET
			"F", // PHE
			"P", // PRO
			"S", // SER
			"T", // THR
			"W", // TRP
			"Y", // TYR
			"V", // VAL
			"?", // ASX
			"?", // GLX
			"O", // WAT
			"O", // HOH
			"?", // HEM
			"?", // PO4
			"?", // MEL
		};

		const std::string Residue::SYMBOL_NAME[ (int)SYMBOL::COUNT ] = {
			"Unknown",		   // UNKNOWN
			"Alanine",		   // ALA
			"Arginine",		   // ARG
			"Asparagine",	   // ASN
			"Aspartate",	   // ASP
			"Cysteine",		   // CYS
			"Glutamine",	   // GLN
			"Glutamate",	   // GLU
			"Glycine",		   // GLY
			"Histidine",	   // HIS
			"Isoleucine",	   // ILE
			"Leucine",		   // LEU
			"Lysine",		   // LYS
			"Methionine",	   // MET
			"Phenylalanine",   // PHE
			"Proline",		   // PRO
			"Serine",		   // SER
			"Threonine",	   // THR
			"Tryptophan",	   // TRP
			"Tyrosine",		   // TYR
			"Valine",		   // VAL
			"?",			   // ASX
			"?",			   // GLX
			"Water",		   // WAT
			"Water",		   // HOH
			"Heme",			   // HEM
			"Phosphate group", // PO4
			"Melagatran",	   // MEL
		};

		// http://jmol.sourceforge.net/jscolors/#Jmolcolors : Protein "amino" colors
		const Color::Rgb Residue::SYMBOL_COLOR[ (int)SYMBOL::COUNT ] = {
			{ 190, 160, 110 }, // UNKNOWN
			{ 200, 200, 200 }, // ALA
			{ 20, 90, 255 },   // ARG
			{ 0, 220, 220 },   // ASN
			{ 230, 10, 10 },   // ASP
			{ 230, 230, 0 },   // CYS
			{ 0, 220, 220 },   // GLN
			{ 230, 10, 10 },   // GLU
			{ 235, 235, 235 }, // GLY
			{ 130, 130, 210 }, // HIS
			{ 15, 130, 15 },   // ILE
			{ 15, 130, 15 },   // LEU
			{ 20, 90, 255 },   // LYS
			{ 230, 230, 0 },   // MET
			{ 50, 50, 170 },   // PHE
			{ 220, 150, 130 }, // PRO
			{ 250, 150, 0 },   // SER
			{ 250, 150, 0 },   // THR
			{ 180, 90, 180 },  // TRP
			{ 50, 50, 170 },   // TYR
			{ 15, 130, 15 },   // VAL
			{ 255, 105, 180 }, // ASX
			{ 255, 105, 180 }, // GLX
			{ 255, 105, 180 }, // WAT
			{ 255, 105, 180 }, // HOH
			{ 255, 105, 180 }, // HEM
			{ 255, 105, 180 }, // PO4
			{ 255, 105, 180 }, // MEL
		};
	} // namespace Model
} // namespace VTX
