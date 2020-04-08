#include "residue.hpp"
#include "view/ui/residue.hpp"

namespace VTX
{
	namespace Model
	{
		void Residue::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() )
			{ addItem( (View::BaseView<BaseModel> *)Generic::create<Residue, View::UI::Residue>( this ) ); }
			else
			{
				Generic::destroy( removeItem( ID::View::UI_RESIDUE ) );
			}
		}

		void Residue::setVisible( const bool p_visible )
		{
			Generic::BaseVisible::setVisible( p_visible );
			_moleculePtr->refreshVisibility();
		}

		const Atom * const Residue::findFirstAtomByName( const std::string & p_name ) const
		{
			for ( uint i = 0; i < _atomCount; ++i )
			{
				const Atom & atom = _moleculePtr->getAtom( _idFirstAtom + i );
				if ( atom.getName() == p_name ) { return &atom; }
			}

			return nullptr;
		}

		const std::string Residue::SYMBOL_NAME[ (int)RESIDUE_SYMBOL::COUNT ] = {
			"Unknown",		 // UNKWNON,
			"Alanine",		 // ALA,
			"Arginine",		 // ARG,
			"Asparagine",	 // ASN,
			"Aspartate",	 // ASP,
			"Cysteine",		 // CYS,
			"Glutamine",	 // GLN,
			"Glutamate",	 // GLU,
			"Glycine",		 // GLY,
			"Histidine",	 // HIS,
			"Isoleucine",	 // ILE,
			"Leucine",		 // LEU,
			"Lysine",		 // LYS,
			"Methionine",	 // MET,
			"Phenylalanine", // PHE,
			"Proline",		 // PRO,
			"Serine",		 // SER,
			"Threonine",	 // THR,
			"Tryptophan",	 // TRP,
			"Tyrosine",		 // TYR,
			"Valine",		 // VAL,
			"?",			 // ASX,
			"?",			 // GLX
		};

		const std::string Residue::SYMBOL_SHORT[ (int)RESIDUE_SYMBOL::COUNT ] = {
			"UNKNOWN", // UNKWNON
			"A",	   // ALA,
			"R",	   // ARG,
			"N",	   // ASN,
			"D",	   // ASP,
			"C",	   // CYS,
			"Q",	   // GLN,
			"E",	   // GLU,
			"G",	   // GLY,
			"H",	   // HIS,
			"I",	   // ILE,
			"L",	   // LEU,
			"K",	   // LYS,
			"M",	   // MET,
			"F",	   // PHE,
			"P",	   // PRO,
			"S",	   // SER,
			"T",	   // THR,
			"W",	   // TRP,
			"Y",	   // TYR,
			"V",	   // VAL,
			"?",	   // ASX,
			"?",	   // GLX
		};
	} // namespace Model
} // namespace VTX
