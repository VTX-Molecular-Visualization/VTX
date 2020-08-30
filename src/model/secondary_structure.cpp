#include "secondary_structure.hpp"
#include "tool/chrono.hpp"
#include "view/d3/ribbon.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		SecondaryStructure::SecondaryStructure( Molecule * const p_molecule ) : _molecule( p_molecule )
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating secondary structure..." );

			// The current frame.
			const Molecule::AtomPositionsFrame & positions = p_molecule->getAtomPositionFrame( p_molecule->getFrame() );

			// TODO: //?
			// Loop over chains (1 chain = 1 ribbon).
			for ( uint chainIdx = 0; chainIdx < p_molecule->getChainCount(); ++chainIdx )
			{
				const Chain & chain		   = p_molecule->getChain( chainIdx );
				uint		  residueCount = chain.getResidueCount();

				// Not enought residues.
				if ( residueCount < 4 )
				{
					VTX_DEBUG( "Chain residue count < 4" );
					continue;
				}

				uint idxFirstResidue   = chain.getIndexFirstResidue();
				uint validResidueCount = 0;
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					const Residue & residue = p_molecule->getResidue( idxFirstResidue + residueIdx );

					// Use backbone to compute spline data.
					// Find alpha carbon.
					const Model::Atom * const CA = residue.findFirstAtomByName( "CA" );
					// Find oxygen.
					const Model::Atom * const O = residue.findFirstAtomByName( "O" );

					// Not an amine acid (water, heme, or phosphate groupment).
					if ( CA == nullptr || O == nullptr )
					{
						// What to do, skip residue, skip all the chain or split the chain into multiple ribbons?
						continue;
					}

					const Vec3f & positionCA = positions[ CA->getIndex() ];
					const Vec3f & positionO	 = positions[ O->getIndex() ];

					// Add control point position.
					_controlPointPositions.emplace_back( positionCA );

					// Compute control point direction.
					Vec3f direction = Util::Math::normalize( positionO - positionCA );

					// TODO: Flip test, something like if dot product > 0 then direction *= -1
					_controlPointDirections.emplace_back( direction );

					// Add secondary structure type.
					_controlPointSecondaryStructures.emplace_back( uint( residue.getSecondaryStructure() ) );

					// Add indices.
					if ( validResidueCount >= 4 )
					{
						_indices.emplace_back( validResidueCount - 4 );
						_indices.emplace_back( validResidueCount - 3 );
						_indices.emplace_back( validResidueCount - 2 );
						_indices.emplace_back( validResidueCount - 1 );
					}

					validResidueCount++;
				}
			}

			chrono.stop();
			VTX_INFO( "Secondary structure created in " + std::to_string( chrono.elapsedTime() ) + "s" );

			addItem( (View::BaseView<BaseModel> *)new View::D3::Ribbon( this ) );
		}

		SecondaryStructure::~SecondaryStructure()
		{
			glBindVertexArray( _vao );
			glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			glDisableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );
			glBindBuffer( GL_ARRAY_BUFFER, _vboDirections );
			glDisableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );
			glBindBuffer( GL_ARRAY_BUFFER, _vboSecondaryStructures );
			glDisableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );
			glBindBuffer( GL_ARRAY_BUFFER, _vboVisibilities );
			glDisableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_VISIBILITY );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindVertexArray( 0 );

			if ( _vboPositions != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_vboPositions );
			if ( _vboDirections != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_vboDirections );
			if ( _vboSecondaryStructures != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_vboSecondaryStructures );
			if ( _vboVisibilities != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_vboVisibilities );
			if ( _ibo != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_ibo );
			if ( _vao != GL_INVALID_VALUE )
				glDeleteVertexArrays( 1, &_vao );
		}

		void SecondaryStructure::init()
		{
			// VBO.
			glGenBuffers( 1, &_vboPositions );
			glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			glBufferData( GL_ARRAY_BUFFER,
						  _controlPointPositions.size() * sizeof( Vec3f ),
						  _controlPointPositions.data(),
						  GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_vboDirections );
			glBindBuffer( GL_ARRAY_BUFFER, _vboDirections );
			glBufferData( GL_ARRAY_BUFFER,
						  _controlPointDirections.size() * sizeof( Vec3f ),
						  _controlPointDirections.data(),
						  GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_vboSecondaryStructures );
			glBindBuffer( GL_ARRAY_BUFFER, _vboSecondaryStructures );
			glBufferData( GL_ARRAY_BUFFER,
						  _controlPointSecondaryStructures.size() * sizeof( uint ),
						  _controlPointSecondaryStructures.data(),
						  GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_vboVisibilities );
			glBindBuffer( GL_ARRAY_BUFFER, _vboVisibilities );
			glBufferData( GL_ARRAY_BUFFER,
						  _controlPointVisibilities.size() * sizeof( uint ),
						  _controlPointVisibilities.data(),
						  GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			// IBO.
			glGenBuffers( 1, &_ibo );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof( uint32_t ), _indices.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			// VAO.
			glGenVertexArrays( 1, &_vao );
			glBindVertexArray( _vao );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );

			glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );
			glVertexAttribPointer(
				ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );

			glBindBuffer( GL_ARRAY_BUFFER, _vboDirections );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );
			glVertexAttribPointer(
				ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );

			glBindBuffer( GL_ARRAY_BUFFER, _vboSecondaryStructures );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );
			glVertexAttribPointer( ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE,
								   1,
								   GL_UNSIGNED_INT,
								   GL_FALSE,
								   sizeof( uint ),
								   0 );

			glBindBuffer( GL_ARRAY_BUFFER, _vboVisibilities );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_VISIBILITY );
			glVertexAttribPointer(
				ATTRIBUTE_LOCATION::CONTROL_POINT_VISIBILITY, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof( uint ), 0 );

			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			glBindVertexArray( 0 );
		}

		void SecondaryStructure::bindBuffers()
		{
			glBindVertexArray( _vao );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
		}

		void SecondaryStructure::unbindBuffers()
		{
			glBindVertexArray( 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		void SecondaryStructure::print() const
		{
			VTX_INFO( "Control points: " + std::to_string( _controlPointPositions.size() ) );
			VTX_INFO( "Indices: " + std::to_string( _indices.size() ) );

			VTX_DEBUG( "Sizeof secondary structure: " + std::to_string( sizeof( *this ) ) );
		}

	} // namespace Model
} // namespace VTX
