#include "secondary_structure.hpp"
#include "tool/chrono.hpp"
#include "view/d3/ribbon.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		const Color::Rgb
			SecondaryStructure::SECONDARY_STRUCTURE_COLORS_JMOL[ uint( Model::Residue::SECONDARY_STRUCTURE::COUNT ) ]
			= { Color::Rgb( 1.f, 0.f, 0.5f ),  Color::Rgb( 0.62f, 0.f, 0.5f ), Color::Rgb( 0.37f, 0.f, 0.5f ),
				Color::Rgb( 1.f, 0.78f, 0.f ), Color::Rgb( 0.37f, 0.5f, 1.f ), Color::Rgb::WHITE };

		SecondaryStructure::SecondaryStructure( Molecule * const p_molecule ) : _molecule( p_molecule )
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating secondary structure..." );

			const Molecule::AtomPositionsFrame & positions = p_molecule->getAtomPositionFrame( p_molecule->getFrame() );

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

				// Store chain control points.
				std::vector<Vec3f> controlPointPositions = std::vector<Vec3f>();
				std::vector<uint>  residueIndex			 = std::vector<uint>();

				Vec3f directionLast;
				uint  idxFirstResidue = chain.getIndexFirstResidue();
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					const Residue & residue = p_molecule->getResidue( idxFirstResidue + residueIdx );

					// Use backbone to compute spline data.
					// Find alpha carbon.
					const Model::Atom * const CA = residue.findFirstAtomByName( "CA" );
					// Find oxygen.
					const Model::Atom * const O = residue.findFirstAtomByName( "O" );

					// Not an amine acid (water, heme, or phosphate groupment).
					if ( CA == nullptr )
					{
						// What to do, skip residue, skip all the chain or split the chain into multiple ribbons?
						continue;
					}
					// Missing oxygen atom.
					if ( O == nullptr )
					{
						VTX_DEBUG( "Missing oxygen atom in amine acid" );
						continue;
					}

					const Vec3f & positionCA = positions[ CA->getIndex() ];
					const Vec3f & positionO	 = positions[ O->getIndex() ];

					// Add control point position.
					controlPointPositions.emplace_back( positionCA );

					// Store residue index for later.
					residueIndex.emplace_back( residue.getIndex() );

					// Compute control point direction.
					Vec3f direction = Util::Math::normalize( positionO - positionCA );
					if ( controlPointPositions.size() > 0 )
					{
						_flipTest( direction, directionLast );
					}
					_controlPointDirections.emplace_back( direction );

					// Add secondary structure type.
					_controlPointSecondaryStructures.emplace_back( uint( residue.getSecondaryStructure() ) );

					// Add color.
					switch ( _colorMode )
					{
					case COLOR_MODE::JMOL:
						_controlPointColors.emplace_back(
							SECONDARY_STRUCTURE_COLORS_JMOL[ uint( residue.getSecondaryStructure() ) ] );
						break;
					case COLOR_MODE::CHAIN:
						_controlPointColors.emplace_back( residue.getChainPtr()->getColor() );
						break;
					default: _controlPointColors.emplace_back( Color::Rgb::WHITE ); break;
					}
				}

				// Add indices and save mapping.
				if ( controlPointPositions.size() >= 4 )
				{
					uint offset = uint( _controlPointPositions.size() );
					for ( uint i = 1; i < controlPointPositions.size() - 2; ++i )
					{
						_residueToControlPointIndices.emplace( residueIndex[ i ], uint( _indices.size() ) );

						_indices.emplace_back( offset + i - 1 );
						_indices.emplace_back( offset + i );
						_indices.emplace_back( offset + i + 1 );
						_indices.emplace_back( offset + i + 2 );
					}

					// Merge control points.
					_controlPointPositions.insert(
						_controlPointPositions.end(), controlPointPositions.begin(), controlPointPositions.end() );
				}
			}

			std::vector<uint> indicesReverse = _indices;
			std::reverse( indicesReverse.begin(), indicesReverse.end() );
			//_indices.insert( _indices.end(), indicesReverse.begin(), indicesReverse.end() );

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
			glBindBuffer( GL_ARRAY_BUFFER, _vboColors );
			glDisableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR );
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
			if ( _vboColors != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_vboColors );
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

			glGenBuffers( 1, &_vboColors );
			glBindBuffer( GL_ARRAY_BUFFER, _vboColors );
			glBufferData( GL_ARRAY_BUFFER,
						  _controlPointColors.size() * sizeof( Color::Rgb ),
						  _controlPointColors.data(),
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
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );
			glVertexAttribPointer(
				ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ARRAY_BUFFER, _vboDirections );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );
			glVertexAttribPointer(
				ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ARRAY_BUFFER, _vboSecondaryStructures );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );
			glVertexAttribPointer( ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE,
								   1,
								   GL_UNSIGNED_INT,
								   GL_FALSE,
								   sizeof( uint ),
								   0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ARRAY_BUFFER, _vboColors );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR );
			glVertexAttribPointer(
				ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof( Color::Rgb ), 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

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

		void SecondaryStructure::setCurrentFrame()
		{
			const Molecule::AtomPositionsFrame & positions = _molecule->getAtomPositionFrame( _molecule->getFrame() );

			_controlPointPositions.clear();
			_controlPointDirections.clear();

			for ( uint chainIdx = 0; chainIdx < _molecule->getChainCount(); ++chainIdx )
			{
				const Chain & chain		   = _molecule->getChain( chainIdx );
				uint		  residueCount = chain.getResidueCount();

				if ( residueCount < 4 )
				{
					VTX_DEBUG( "Chain residue count < 4" );
					continue;
				}

				uint  validResidueCount = 0;
				Vec3f directionLast;
				uint  idxFirstResidue = chain.getIndexFirstResidue();
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					const Residue &			  residue = _molecule->getResidue( idxFirstResidue + residueIdx );
					const Model::Atom * const CA	  = residue.findFirstAtomByName( "CA" );
					const Model::Atom * const O		  = residue.findFirstAtomByName( "O" );

					if ( CA == nullptr )
					{
						continue;
					}
					if ( O == nullptr )
					{
						VTX_DEBUG( "Missing oxygen atom in amine acid" );
						continue;
					}

					const Vec3f & positionCA = positions[ CA->getIndex() ];
					const Vec3f & positionO	 = positions[ O->getIndex() ];

					_controlPointPositions.emplace_back( positionCA );

					Vec3f direction = Util::Math::normalize( positionO - positionCA );
					if ( validResidueCount > 0 )
					{
						_flipTest( direction, directionLast );
					}
					_controlPointDirections.emplace_back( direction );

					validResidueCount++;
				}
			}

			glNamedBufferSubData(
				_vboPositions, 0, sizeof( Vec3f ) * _controlPointPositions.size(), _controlPointPositions.data() );
			glNamedBufferSubData(
				_vboDirections, 0, sizeof( Vec3f ) * _controlPointDirections.size(), _controlPointDirections.data() );
		}

		void SecondaryStructure::_flipTest( Vec3f & p_direction, Vec3f & p_directionLast ) const
		{
			if ( Util::Math::dot( p_direction, p_directionLast ) < 0.f )
			{
				p_direction = -p_direction;
			}
			p_directionLast = p_direction;
		}

		void SecondaryStructure::print() const
		{
			VTX_INFO( "Control points: " + std::to_string( _controlPointPositions.size() ) );
			VTX_INFO( "Indices: " + std::to_string( _indices.size() ) );

			VTX_DEBUG( "Sizeof secondary structure: " + std::to_string( sizeof( *this ) ) );
		}

	} // namespace Model
} // namespace VTX
