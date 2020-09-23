#include "molecule_structure.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				MoleculeStructure::MoleculeStructure( QWidget * p_parent ) :
					QTreeWidget( p_parent ), _ui( new Ui_MoleculeStructure() )
				{
					_ui->setupUi( this );
				}

				MoleculeStructure::~MoleculeStructure() { delete _ui; }

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
