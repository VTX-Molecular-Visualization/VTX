#include "model_molecule.hpp"
#include "../ui/enum_component_type.hpp"
#include "../view/view_ball_and_stick.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		void ModelMolecule::_addViews()
		{
			_addView( std::shared_ptr<View::BaseView<BaseModel>>(
				(View::BaseView<BaseModel> *)( new View::ViewBallAndStick() ) ) );
			_addView( std::reinterpret_pointer_cast<View::BaseView<BaseModel>>(
				( VTXApp::get().getUIComponentByType( UI::COMPONENT_TYPE::SCENE ) ) ) );
		}

		void ModelMolecule::printInfos() const
		{
			VTX_INFO( "Molecule: " + _name );
			VTX_INFO( "Chains: " + std::to_string( _chains.size() ) + " / Residues: "
					  + std::to_string( _residues.size() ) + " / Atoms: " + std::to_string( _atoms.size() )
					  + " / Bonds: " + std::to_string( _bonds.size() ) );
		}
	} // namespace Model
} // namespace VTX
