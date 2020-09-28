#include "molecule_structure.hpp"
#include "ui/user_interface.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				MoleculeStructure::MoleculeStructure( Model::Molecule * const p_model, QWidget * p_parent ) : View::BaseView<Model::Molecule>( p_model ), BaseWidget( p_parent )
				{
					const VTX::UI::MainWindow & mainWindow = VTXApp::get().getUI().getMainWindow();
				}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
