#include "model_molecule.hpp"
#include "../util/type.hpp"
#include "../view/view_3d_ball_and_stick.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		void ModelMolecule::_addViews()
		{
			_addView( std::shared_ptr<View::BaseView<BaseModel>>(
				(View::BaseView<BaseModel> *)( new View::View3DBallAndStick() ) ) );

			_addView( Util::Type::componentToView<ModelMolecule>( UI::COMPONENT_TYPE::VIEW_MOLECULE ) );
		}

		void ModelMolecule::printInfos() const
		{
			VTX_INFO( "Molecule: " + _name );
			VTX_INFO( "Chains: " + std::to_string( _chains.size() ) + " / Residues: "
					  + std::to_string( _residues.size() ) + " / Atoms: " + std::to_string( _atoms.size() )
					  + " / Bonds: " + std::to_string( _bonds.size() ) );
		}

		void ModelMolecule::setSelectedChain( const uint p_id )
		{
			if ( _selectedChain != nullptr ) { _selectedChain->setSelected( false ); }
			try
			{
				_selectedChain = &getChain( p_id );
				_selectedChain->setSelected( true );
				VTX_INFO( "Chain " + std::to_string( _selectedChain->getId() ) + " selected" );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "Failed to select chain" );
				_selectedChain = nullptr;
			}
		}
		void ModelMolecule::setSelectedResidue( const uint p_id )
		{
			if ( _selectedResidue != nullptr ) { _selectedResidue->setSelected( false ); }
			try
			{
				_selectedResidue = &getResidue( p_id );
				_selectedResidue->setSelected( true );
				setSelectedChain( _selectedResidue->getChainPtr()->getId() );
				VTX_INFO( "Residue " + std::to_string( _selectedResidue->getId() ) + " selected" );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "Failed to select residue" );
				_selectedResidue = nullptr;
			}
		}
		void ModelMolecule::setSelectedAtom( const uint p_id )
		{
			if ( _selectedAtom != nullptr ) { _selectedAtom->setSelected( false ); }
			try
			{
				_selectedAtom = &getAtom( p_id );
				_selectedAtom->setSelected( true );
				setSelectedResidue( _selectedAtom->getResiduePtr()->getId() );
				VTX_INFO( "Atom " + std::to_string( _selectedAtom->getId() ) + " selected" );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "Failed to select atom" );
				_selectedAtom = nullptr;
			}
		}
	} // namespace Model
} // namespace VTX
