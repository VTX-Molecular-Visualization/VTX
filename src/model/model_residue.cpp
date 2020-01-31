#include "model_residue.hpp"
#include "../util/type.hpp"
#include "../view/view_ui_residue.hpp"

namespace VTX
{
	namespace Model
	{
		void ModelResidue::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() ) { addItem( (View::BaseView<BaseModel> *)( new View::ViewUIResidue( this ) ) ); }
			else
			{
				_deleteView( ID::View::UI_RESIDUE );
			}
		}

		const std::string ModelResidue::SYMBOL_NAME[ (int)RESIDUE_SYMBOL::COUNT ] = {
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
			"Water"			 // HOH,
		};

		const std::string ModelResidue::SYMBOL_SHORT[ (int)RESIDUE_SYMBOL::COUNT ] = {
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
			"HOH",	   // HOH,
		};

		const Vec3f * ModelResidue::SYMBOL_COLOR[ (int)RESIDUE_SYMBOL::COUNT ] = {
			// TODO
		};

	} // namespace Model
} // namespace VTX
