#include "residue.hpp"
#include "atom.hpp"
#include "bond.hpp"
#include "chain.hpp"
#include "molecule.hpp"
#include <algorithm>

namespace VTX
{
	namespace Model
	{
		bool Residue::IsStandard( const std::string & p_residueSymbol )
		{
			std::string residueSymbol = p_residueSymbol;
			std::transform( residueSymbol.begin(),
							residueSymbol.end(),
							residueSymbol.begin(),
							[]( unsigned char c ) { return std::toupper( c ); } );

			return std::find( std::begin( SYMBOL_STR ), std::end( SYMBOL_STR ), residueSymbol )
				   != std::end( SYMBOL_STR );
		}

		Molecule * const Residue::getMoleculePtr() const { return _chainPtr->getMoleculePtr(); };
		void			 Residue::setChainPtr( Chain * const p_chain )
		{
			_chainPtr = p_chain;

			setParent( _chainPtr );
			setRepresentableMolecule( p_chain->getMoleculePtr() );
		}

		const std::string & Residue::getSymbolStr() const
		{
			return _symbol < SYMBOL_COUNT
					   ? SYMBOL_STR[ _symbol ]
					   : getMoleculePtr()->getUnknownResidueSymbols()[ _symbol - SYMBOL_COUNT ]->symbolStr;
		}
		const std::string & Residue::getSymbolName() const
		{
			return _symbol < SYMBOL_COUNT
					   ? SYMBOL_NAME[ _symbol ]
					   : getMoleculePtr()->getUnknownResidueSymbols()[ _symbol - SYMBOL_COUNT ]->symbolName;
		}

		const std::string & Residue::getSymbolShort() const
		{
			return _symbol < SYMBOL_COUNT ? SYMBOL_SHORT_STR[ _symbol ] : SYMBOL_SHORT_STR[ int( SYMBOL::UNKNOWN ) ];
		}
		void Residue::setSymbol( const SYMBOL & p_symbol ) { setSymbol( int( p_symbol ) ); }
		void Residue::setSymbol( const int p_symbolValue )
		{
			_symbol = p_symbolValue;
			BaseModel::setDefaultName( &getSymbolName() );
		};

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

		const uint Residue::findBondIndex( const uint p_firstAtomIndex, const uint p_secondAtomIndex ) const
		{
			const uint lastBondIndex = getIndexFirstBond() + getBondCount() - 1;

			// Start from the end because this function will mostly be call to find external bonds that are at the end
			// of bond range.
			for ( uint i = lastBondIndex; i >= getIndexFirstBond(); i-- )
			{
				const Bond * const bond = getMoleculePtr()->getBond( i );

				if ( bond == nullptr )
					continue;

				if ( bond->getIndexFirstAtom() == p_firstAtomIndex && bond->getIndexSecondAtom() == p_secondAtomIndex )
					return i;
			}

			return INVALID_ID;
		}

		const Atom * const Residue::getAlphaCarbon() const { return findFirstAtomByName( "CA" ); }

		void Residue::setVisible( const bool p_visible )
		{
			const bool previousVisibleState = isVisible();

			BaseVisible::setVisible( p_visible );

			if ( previousVisibleState != p_visible )
			{
				_notifyViews( new Event::VTXEventValue<uint>( Event::Model::RESIDUE_VISIBILITY, _index ) );
				getMoleculePtr()->propagateEventToViews(
					new Event::VTXEventValue<uint>( Event::Model::RESIDUE_VISIBILITY, _index ) );
			}
		}

		void Residue::setVisible( const bool p_visible, const bool p_notify )
		{
			const bool previousVisibleState = isVisible();

			BaseVisible::setVisible( p_visible );

			if ( previousVisibleState != p_visible )
			{
				if ( p_notify )
				{
					_notifyViews( new Event::VTXEventValue<uint>( Event::Model::RESIDUE_VISIBILITY, _index ) );
					getMoleculePtr()->propagateEventToViews(
						new Event::VTXEventValue<uint>( Event::Model::RESIDUE_VISIBILITY, _index ) );
				}
			}
		}

		Object3D::Helper::AABB Residue::getAABB() const
		{
			Object3D::Helper::AABB aabb = Object3D::Helper::AABB();

			for ( uint i = 0; i < _atomCount; ++i )
			{
				const Atom * const atom = getMoleculePtr()->getAtom( _indexFirstAtom + i );

				if ( atom == nullptr )
					continue;

				aabb.extend( atom->getAABB() );
			}

			return aabb;
		}

		Object3D::Helper::AABB Residue::getWorldAABB() const
		{
			const Object3D::Helper::AABB aabb	   = getAABB();
			const Math::Transform &		 transform = getMoleculePtr()->getTransform();

			Object3D::Helper::AABB worldAabb   = Object3D::Helper::AABB();
			std::vector<Vec3f>	   aabbSummits = aabb.getSummits();

			for ( const Vec3f & summit : aabbSummits )
			{
				const Vec4f worldSummit = transform.get() * Vec4f( summit, 1 );
				worldAabb.extend( worldSummit );
			}

			return worldAabb;
		}

		void Residue::_onRepresentationChange()
		{
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
			"SEC", // SEC
			"PYL", // PYL
			"ASX", // ASX
			"GLX", // GLX
			"C",   // C
			"G",   // G
			"A",   // A
			"U",   // U
			"I",   // I
			"DC",  // DC
			"DG",  // DG
			"DA",  // DA
			"DU",  // DU
			"DT",  // DT
			"DI",  // DI
			"WAT", // WAT
			"HOH", // HOH
		};

		const std::string Residue::SYMBOL_SHORT_STR[ (int)SYMBOL::COUNT ] = {
			"-",  // UNKNOWN
			"A",  // ALA
			"R",  // ARG
			"N",  // ASN
			"D",  // ASP
			"C",  // CYS
			"Q",  // GLN
			"E",  // GLU
			"G",  // GLY
			"H",  // HIS
			"I",  // ILE
			"L",  // LEU
			"K",  // LYS
			"M",  // MET
			"F",  // PHE
			"P",  // PRO
			"S",  // SER
			"T",  // THR
			"W",  // TRP
			"Y",  // TYR
			"V",  // VAL
			"U",  // SEC
			"O",  // PYL
			"?",  // ASX
			"?",  // GLX
			"C",  // C
			"G",  // G
			"A",  // A
			"U",  // U
			"I",  // I
			"DC", // DC
			"DG", // DG
			"DA", // DA
			"DU", // DU
			"DT", // DT
			"DI", // DI
			"O",  // WAT
			"O",  // HOH
		};

		const std::string Residue::SYMBOL_NAME[ (int)SYMBOL::COUNT ] = {
			"Unknown",		  // UNKNOWN
			"Alanine",		  // ALA
			"Arginine",		  // ARG
			"Asparagine",	  // ASN
			"Aspartate",	  // ASP
			"Cysteine",		  // CYS
			"Glutamine",	  // GLN
			"Glutamate",	  // GLU
			"Glycine",		  // GLY
			"Histidine",	  // HIS
			"Isoleucine",	  // ILE
			"Leucine",		  // LEU
			"Lysine",		  // LYS
			"Methionine",	  // MET
			"Phenylalanine",  // PHE
			"Proline",		  // PRO
			"Serine",		  // SER
			"Threonine",	  // THR
			"Tryptophan",	  // TRP
			"Tyrosine",		  // TYR
			"Valine",		  // VAL
			"Selenocysteine", // SEC
			"Pyrrolysine",	  // PYL
			"?",			  // ASX
			"?",			  // GLX
			"Cytosine",		  // C
			"Guanine",		  // G
			"Adenine",		  // A
			"Uracil",		  // U
			"Inosine",		  // I
			"Cytosine",		  // DC
			"Guanine",		  // DG
			"Adenine",		  // DA
			"Uracil",		  // DU
			"Thymine",		  // DT
			"Inosine",		  // DI
			"Water",		  // WAT
			"Water",		  // HOH
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
			{ 190, 160, 110 }, // SEC
			{ 190, 160, 110 }, // PYL
			{ 255, 105, 180 }, // ASX
			{ 255, 105, 180 }, // GLX
			{ 255, 140, 75 },  // C
			{ 255, 112, 112 }, // G
			{ 160, 160, 255 }, // A
			{ 255, 128, 128 }, // U
			{ 128, 255, 255 }, // I
			{ 255, 140, 75 },  // DC
			{ 255, 112, 112 }, // DG
			{ 160, 160, 255 }, // DA
			{ 255, 128, 128 }, // DU
			{ 160, 255, 160 }, // DT
			{ 128, 255, 255 }, // DI
			{ 255, 105, 180 }, // WAT
			{ 255, 105, 180 }, // HOH
		};

		const Color::Rgb Residue::getResidueColor( const Model::Residue & p_residue )
		{
			return p_residue.isStandardResidue() ? SYMBOL_COLOR[ p_residue._symbol ]
												 : SYMBOL_COLOR[ int( SYMBOL::UNKNOWN ) ];
		}

	} // namespace Model
} // namespace VTX
