#include "model_molecule.hpp"
#include "../view/view_ball_and_stick.hpp"

namespace VTX
{
	namespace Model
	{
		void ModelMolecule::_addViews()
		{
			_addView( (View::BaseView<BaseModel> *)( new View::ViewBallAndStick() ) );
			//_addView( new UI::ComponentScene() );
		}

		void ModelMolecule::printInfos()
		{
			VTX_INFO( "Molecule: " + _name );
			VTX_INFO( "Chains: " + std::to_string( _chains.size() ) + " / Residues: "
					  + std::to_string( _residues.size() ) + " / Atoms: " + std::to_string( _atoms.size() )
					  + " / Bonds: " + std::to_string( _bonds.size() ) );
		}
	} // namespace Model
} // namespace VTX
