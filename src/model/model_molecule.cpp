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
			_addView( Util::Type::componentToView<ModelMolecule>( UI::COMPONENT_NAME::VIEW_MOLECULE ) );
		}

		std::shared_ptr<BaseView3DMolecule> ModelMolecule::createAndGetView3DByRepresentation()
		{
			View::View3DBallAndStick * view = new View::View3DBallAndStick();

			_addView( std::shared_ptr<View::BaseView<BaseModel>>( (View::BaseView<BaseModel> *)( view ) ) );

			return std::shared_ptr<BaseView3DMolecule>( (BaseView3DMolecule *)view );
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
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "Failed to select atom" );
				_selectedAtom = nullptr;
			}
		}

		void ModelMolecule::resetSelectedChain()
		{
			if ( _selectedChain != nullptr )
			{
				_selectedChain->setSelected( false );
				_selectedChain = nullptr;
			}

			resetSelectedResidue();
		}

		void ModelMolecule::resetSelectedResidue()
		{
			if ( _selectedResidue != nullptr )
			{
				_selectedResidue->setSelected( false );
				_selectedResidue = nullptr;
			}

			resetSelectedAtom();
		}

		void ModelMolecule::resetSelectedAtom()
		{
			if ( _selectedAtom != nullptr )
			{
				_selectedAtom->setSelected( false );
				_selectedAtom = nullptr;
			}
		}
	} // namespace Model
} // namespace VTX
