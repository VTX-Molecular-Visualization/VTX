#include "model_molecule.hpp"
#include "../util/type.hpp"
#include "../view/view_3d_molecule_cylinder.hpp"
#include "../view/view_3d_molecule_sphere.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		ModelMolecule::~ModelMolecule()
		{
			_current3DViews.clear();

			glDeleteBuffers( 1, &_vao );
			glDeleteBuffers( 1, &_atomPositionsVBO );
			glDeleteBuffers( 1, &_atomRadiusVBO );
			glDeleteBuffers( 1, &_atomColorsVBO );
			glDeleteBuffers( 1, &_bondsIBO );
		}

		void ModelMolecule::_addViews()
		{
			_addView( Util::Type::componentToView<ModelMolecule>( UI::COMPONENT_NAME::VIEW_MOLECULE ) );
			setup3DViews();
		}

		void ModelMolecule::setup3DViews()
		{
			// Remove actual views.
			for ( BaseView3DMolecule * const view : _current3DViews )
			{
				std::string name = ( (View::BaseView3DMolecule *)view )->getNameStr();
				try
				{
					_views.at( name ).reset();
					_views.erase( name );
				}
				catch ( const std::exception )
				{
					VTX_ERROR( "Trying to delete a non existing view: " + name );
				}
			}
			_current3DViews.clear();

			// Create views.
			switch ( Setting::Rendering::reprensation )
			{
				// TODO: refacto in specific method.
			case View::MOLECULE_REPRESENTATION::BALL_AND_STICK:
			{
				// Sphere.
				View::View3DMoleculeSphere * viewSphere = new View::View3DMoleculeSphere( 0.3f );
				_addView( std::shared_ptr<View::BaseView<BaseModel>>( (View::BaseView<BaseModel> *)( viewSphere ) ) );
				_current3DViews.emplace_back( (BaseView3DMolecule *)viewSphere );
				break;
				// Cylinder.
				View::View3DMoleculeCylinder * viewCylinder = new View::View3DMoleculeCylinder();
				_addView( std::shared_ptr<View::BaseView<BaseModel>>( (View::BaseView<BaseModel> *)( viewCylinder ) ) );
				_current3DViews.emplace_back( (BaseView3DMolecule *)viewCylinder );
			}
			case View::MOLECULE_REPRESENTATION::VAN_DER_WAALS:
			{
				// Sphere.
				View::View3DMoleculeSphere * view = new View::View3DMoleculeSphere( 1.0f );
				_addView( std::shared_ptr<View::BaseView<BaseModel>>( (View::BaseView<BaseModel> *)( view ) ) );
				_current3DViews.emplace_back( (BaseView3DMolecule *)view );
				break;
			}
			default: VTX_ERROR( "Unknown representation" ); break;
			}
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
