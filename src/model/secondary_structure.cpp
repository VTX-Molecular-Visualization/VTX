#include "secondary_structure.hpp"
#include "id.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "view/d3/ribbon.hpp"

namespace VTX
{
	namespace Model
	{
		const Color::Rgb SecondaryStructure::COLORS_JMOL[ uint( VALUE::COUNT ) ] = { Color::Rgb( 1.f, 0.f, 0.5f ),	 // HELIX_ALPHA_RIGHT
																					 Color::Rgb( 1.f, 0.f, 0.5f ),	 // HELIX_ALPHA_LEFT
																					 Color::Rgb( 0.62f, 0.f, 0.5f ), // HELIX_3_10_RIGHT
																					 Color::Rgb( 0.62f, 0.f, 0.5f ), // HELIX_3_10_LEFT
																					 Color::Rgb( 0.37f, 0.f, 0.5f ), // HELIX_PI
																					 Color::Rgb( 1.f, 0.78f, 0.f ),	 // STRAND
																					 Color::Rgb( 0.37f, 0.5f, 1.f ), // TURN
																					 Color::Rgb::WHITE };			 // COIL

		SecondaryStructure::SecondaryStructure( Molecule * const p_molecule ) : BaseModel3D( ID::Model::MODEL_SECONDARY_STRUCTURE ), _molecule( p_molecule )
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
					if ( controlPointPositions.size() > 1 )
					{
						_flipTest( direction, directionLast );
					}
					_controlPointDirections.emplace_back( direction );

					// Compute control point normal.
					if ( controlPointPositions.size() > 1 )
					{
						Vec3f directionCA = positionCA - controlPointPositions[ controlPointPositions.size() - 2 ];
						Vec3f normal	  = Util::Math::normalize( Util::Math::cross( direction, directionCA ) );
						_controlPointNormals.emplace_back( normal );

						// Copy second to first.
						if ( controlPointPositions.size() == 2 )
						{
							_controlPointNormals[ _controlPointNormals.size() - 2 ] = _controlPointNormals[ _controlPointNormals.size() - 1 ];
						}
					}
					else
					{
						_controlPointNormals.emplace_back( VEC3F_ZERO );
					}

					// Add secondary structure type.
					_controlPointSecondaryStructures.emplace_back( ushort( residue.getSecondaryStructure() ) );

					// Add color.
					switch ( _colorMode )
					{
					case COLOR_MODE::JMOL: _controlPointColors.emplace_back( COLORS_JMOL[ uint( residue.getSecondaryStructure() ) ] ); break;
					case COLOR_MODE::PROTEIN: _controlPointColors.emplace_back( residue.getMoleculePtr()->getColor() ); break;
					case COLOR_MODE::CHAIN: _controlPointColors.emplace_back( residue.getChainPtr()->getColor() ); break;
					case COLOR_MODE::RESIDUE: _controlPointColors.emplace_back( residue.getColor() ); break;
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
					_controlPointPositions.insert( _controlPointPositions.end(), controlPointPositions.begin(), controlPointPositions.end() );
				}
			}

			// Reverse indices to render the other side.
			// std::vector<uint> indicesReverse = _indices;
			// std::reverse( indicesReverse.begin(), indicesReverse.end() );
			//_indices.insert( _indices.end(), indicesReverse.begin(), indicesReverse.end() );

			_controlPointPositions.shrink_to_fit();
			_controlPointDirections.shrink_to_fit();
			_controlPointNormals.shrink_to_fit();
			_controlPointSecondaryStructures.shrink_to_fit();
			_controlPointColors.shrink_to_fit();
			_indices.shrink_to_fit();

			chrono.stop();
			VTX_INFO( "Secondary structure created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SecondaryStructure::instantiateDefaultViews() {};

		SecondaryStructure::~SecondaryStructure() {}

		void SecondaryStructure::_init()
		{
			_buffer->setControlPointPositions( _controlPointPositions );
			_buffer->setControlPointDirections( _controlPointDirections );
			_buffer->setControlPointNormals( _controlPointNormals );
			_buffer->setControlPointSecondaryStructure( _controlPointSecondaryStructures );
			_buffer->setControlPointColors( _controlPointColors );
			_buffer->setIndices( _indices );
		}

		void SecondaryStructure::_instanciate3DViews() {_addRenderable(MVC::MvcManager::get().addViewOnModel( this, ID::View::D3_RIBBON_PATCH, new View::D3::Ribbon( this )) ); }

		void SecondaryStructure::setCurrentFrame()
		{
			// TODO: refacto.
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

			_controlPointPositions.shrink_to_fit();
			_controlPointDirections.shrink_to_fit();

			_buffer->setControlPointPositions( _controlPointPositions );
			_buffer->setControlPointDirections( _controlPointDirections );
		}

		void SecondaryStructure::_fillBufferColors()
		{
			_controlPointColors.clear();

			for ( uint chainIdx = 0; chainIdx < _molecule->getChainCount(); ++chainIdx )
			{
				const Chain & chain		   = _molecule->getChain( chainIdx );
				uint		  residueCount = chain.getResidueCount();

				if ( residueCount < 4 )
				{
					continue;
				}

				uint idxFirstResidue = chain.getIndexFirstResidue();
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					const Residue &			  residue = _molecule->getResidue( idxFirstResidue + residueIdx );
					const Model::Atom * const CA	  = residue.findFirstAtomByName( "CA" );
					const Model::Atom * const O		  = residue.findFirstAtomByName( "O" );

					if ( CA == nullptr || O == nullptr )
					{
						continue;
					}

					switch ( _colorMode )
					{
					case COLOR_MODE::JMOL: _controlPointColors.emplace_back( COLORS_JMOL[ uint( residue.getSecondaryStructure() ) ] ); break;
					case COLOR_MODE::PROTEIN: _controlPointColors.emplace_back( residue.getMoleculePtr()->getColor() ); break;
					case COLOR_MODE::CHAIN: _controlPointColors.emplace_back( residue.getChainPtr()->getColor() ); break;
					case COLOR_MODE::RESIDUE: _controlPointColors.emplace_back( residue.getColor() ); break;
					default: _controlPointColors.emplace_back( Color::Rgb::WHITE ); break;
					}
				}
			}

			_controlPointColors.shrink_to_fit();

			_buffer->setControlPointColors( _controlPointColors );
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
			VTX_INFO( "Control points: " + std::to_string( _controlPointPositions.size() ) + " / Indices: " + std::to_string( _indices.size() ) );
			VTX_DEBUG( "Sizeof secondary structure: " + std::to_string( sizeof( *this ) ) );
		}

	} // namespace Model
} // namespace VTX
