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
			glDeleteBuffers( 1, &_vao );
			glDeleteBuffers( 1, &_atomPositionsVBO );
			glDeleteBuffers( 1, &_atomRadiusVBO );
			glDeleteBuffers( 1, &_atomColorsVBO );
			glDeleteBuffers( 1, &_bondsIBO );
		}

		void ModelMolecule::init()
		{
			// Add views.
			BaseModel::init();

			// Create GL buffers.
			_createBuffers();

			// Set color mode.
			setColorMode();

			// Set default representation.
			setRepresentation();
		}

		void ModelMolecule::_addViews()
		{
			_addView( Util::Type::componentToView<ModelMolecule>( UI::COMPONENT_NAME::VIEW_MOLECULE ) );
			_addView( std::shared_ptr<View::BaseView<BaseModel>>(
				(View::BaseView<BaseModel> *)( new View::View3DMoleculeSphere() ) ) );
			_addView( std::shared_ptr<View::BaseView<BaseModel>>(
				(View::BaseView<BaseModel> *)( new View::View3DMoleculeCylinder() ) ) );
		}

		void ModelMolecule::setRepresentation() { _notifyViews( Event::EVENT_MODEL::CHANGE_REPRESENTATION ); }

		void ModelMolecule::setColorMode()
		{
			_atomColors.clear();

			switch ( Setting::Rendering::colorMode )
			{
			case View::MOLECULE_COLOR_MODE::ATOM:
				for ( ModelAtom atom : _atoms )
				{
					_atomColors.emplace_back( atom.getColor() );
				}
				break;
			case View::MOLECULE_COLOR_MODE::RESIDUE:
				for ( ModelAtom atom : _atoms )
				{
					_atomColors.emplace_back( atom.getResiduePtr()->getColor() );
				}
				break;
			case View::MOLECULE_COLOR_MODE::CHAIN:
				for ( ModelAtom atom : _atoms )
				{
					_atomColors.emplace_back( atom.getChainPtr()->getColor() );
				}
				break;
			default: break;
			}

			glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( Vec3f ) * _atomColors.size(), _atomColors.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
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

		void ModelMolecule::_createBuffers()
		{
			glGenBuffers( 1, &_atomPositionsVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			glBufferData(
				GL_ARRAY_BUFFER, sizeof( Vec3f ) * _atomPositions.size(), _atomPositions.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_atomColorsVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( Vec3f ) * _atomColors.size(), _atomColors.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_atomRadiusVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * _atomRadius.size(), _atomRadius.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			// ibo
			glGenBuffers( 1, &_bondsIBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( uint ) * _bonds.size(), _bonds.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			// vao
			glGenVertexArrays( 1, &_vao );
			glBindVertexArray( _vao );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );

			glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_POSITION );
			glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_COLOR );
			glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_RADIUS );
			glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_RADIUS, 1, GL_FLOAT, GL_FALSE, sizeof( float ), 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			glBindVertexArray( 0 );
		}

		void ModelMolecule::bindBuffers()
		{
			glBindVertexArray( _vao );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );
		}

		void ModelMolecule::unbindBuffers()
		{
			glBindVertexArray( 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		void ModelMolecule::draw()
		{
			bindBuffers();
			_notifyViews( Event::EVENT_MODEL::DRAW );
			unbindBuffers();
		}
	} // namespace Model
} // namespace VTX
