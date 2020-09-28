#include "molecule_structure.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				MoleculeStructure::MoleculeStructure( Model::Molecule * const p_model, QWidget * p_parent ) : View::BaseView<Model::Molecule>( p_model ), BaseWidget( p_parent ) {}

				MoleculeStructure::~MoleculeStructure() {}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
