#include "secondary_structure.hpp"
#include "id.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "view/d3/ribbon.hpp"

namespace VTX
{
	namespace Model
	{
		const Color::Rgb SecondaryStructure::COLORS_JMOL[ uint( VALUE::COUNT ) ]
			= { Color::Rgb( 1.f, 0.f, 0.5f ),	// HELIX_ALPHA_RIGHT
				Color::Rgb( 1.f, 0.f, 0.5f ),	// HELIX_ALPHA_LEFT
				Color::Rgb( 0.62f, 0.f, 0.5f ), // HELIX_3_10_RIGHT
				Color::Rgb( 0.62f, 0.f, 0.5f ), // HELIX_3_10_LEFT
				Color::Rgb( 0.37f, 0.f, 0.5f ), // HELIX_PI
				Color::Rgb( 1.f, 0.78f, 0.f ),	// STRAND
				Color::Rgb( 0.37f, 0.5f, 1.f ), // TURN
				Color::Rgb::WHITE };			// COIL

		SecondaryStructure::SecondaryStructure( Molecule * const p_molecule ) :
			BaseModel3D( ID::Model::MODEL_SECONDARY_STRUCTURE ), _molecule( p_molecule )
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating secondary structure..." );

			const Molecule::AtomPositionsFrame & positions = p_molecule->getAtomPositionFrame( p_molecule->getFrame() );

			// Loop over chains (1 chain = 1 ribbon).
			for ( uint chainIdx = 0; chainIdx < p_molecule->getChainCount(); ++chainIdx )
			{
				const Chain * const chain = p_molecule->getChain( chainIdx );
				if ( chain == nullptr )
					continue;

				uint residueCount = chain->getResidueCount();

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
				uint  idxFirstResidue = chain->getIndexFirstResidue();
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					const Residue * const residue = p_molecule->getResidue( idxFirstResidue + residueIdx );

					if ( residue == nullptr )
						continue;

					// Use backbone to compute spline data.
					// Find alpha carbon.
					const Model::Atom * const CA = residue->findFirstAtomByName( "CA" );
					// Find oxygen.
					const Model::Atom * const O = residue->findFirstAtomByName( "O" );

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
					residueIndex.emplace_back( residue->getIndex() );

					// Compute control point direction.
					Vec3f direction = Util::Math::normalize( positionO - positionCA );
					if ( controlPointPositions.size() > 1 )
					{
						_flipTest( direction, directionLast );
					}
					_bufferDirections.emplace_back( direction );

					// Compute control point normal.
					if ( controlPointPositions.size() > 1 )
					{
						Vec3f directionCA = positionCA - controlPointPositions[ controlPointPositions.size() - 2 ];
						Vec3f normal	  = Util::Math::normalize( Util::Math::cross( direction, directionCA ) );
						_bufferNormals.emplace_back( normal );

						// Copy second to first.
						if ( controlPointPositions.size() == 2 )
						{
							_bufferNormals[ _bufferNormals.size() - 2 ] = _bufferNormals[ _bufferNormals.size() - 1 ];
						}
					}
					else
					{
						_bufferNormals.emplace_back( VEC3F_ZERO );
					}

					// Add secondary structure type.
					_bufferSecondaryStructures.emplace_back( ushort( residue->getSecondaryStructure() ) );

					// Add color.
					switch ( _colorMode )
					{
					case COLOR_MODE::JMOL:
						_bufferColors.emplace_back( COLORS_JMOL[ uint( residue->getSecondaryStructure() ) ] );
						break;
					case COLOR_MODE::PROTEIN:
						_bufferColors.emplace_back( residue->getMoleculePtr()->getColor() );
						break;
					case COLOR_MODE::CHAIN: _bufferColors.emplace_back( residue->getChainPtr()->getColor() ); break;
					case COLOR_MODE::RESIDUE: _bufferColors.emplace_back( residue->getColor() ); break;
					default: _bufferColors.emplace_back( Color::Rgb::WHITE ); break;
					}
				}

				// Add indices and save mapping.
				if ( controlPointPositions.size() >= 4 )
				{
					_residueToPositions.emplace( residueIndex[ 0 ], uint( _bufferPositions.size() ) );
					uint offset = uint( _bufferPositions.size() );
					for ( uint i = 1; i < controlPointPositions.size() - 2; ++i )
					{
						_residueToIndices.emplace( residueIndex[ i ], uint( _buffferIndices.size() ) );
						_residueToPositions.emplace( residueIndex[ i ], uint( _bufferPositions.size() + i ) );

						_buffferIndices.emplace_back( offset + i - 1 );
						_buffferIndices.emplace_back( offset + i );
						_buffferIndices.emplace_back( offset + i + 1 );
						_buffferIndices.emplace_back( offset + i + 2 );
					}
					_residueToPositions.emplace( residueIndex[ controlPointPositions.size() - 1 ],
												 uint( _bufferPositions.size() + controlPointPositions.size() - 1 ) );
					_residueToPositions.emplace( residueIndex[ controlPointPositions.size() - 2 ],
												 uint( _bufferPositions.size() + controlPointPositions.size() - 2 ) );

					// Merge control points.
					_bufferPositions.insert(
						_bufferPositions.end(), controlPointPositions.begin(), controlPointPositions.end() );
				}
			}

			// Reverse indices to render the other side.
			// std::vector<uint> indicesReverse = _indices;
			// std::reverse( indicesReverse.begin(), indicesReverse.end() );
			//_indices.insert( _indices.end(), indicesReverse.begin(), indicesReverse.end() );

			_bufferPositions.shrink_to_fit();
			_bufferDirections.shrink_to_fit();
			_bufferNormals.shrink_to_fit();
			_bufferSecondaryStructures.shrink_to_fit();
			_bufferColors.shrink_to_fit();
			_buffferIndices.shrink_to_fit();

			chrono.stop();
			VTX_INFO( "Secondary structure created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SecondaryStructure::_init() {}

		void SecondaryStructure::_fillBuffer()
		{
			_buffer->setControlPointPositions( _bufferPositions );
			_buffer->setControlPointDirections( _bufferDirections );
			_buffer->setControlPointNormals( _bufferNormals );
			_buffer->setControlPointSecondaryStructure( _bufferSecondaryStructures );
			_buffer->setControlPointColors( _bufferColors );
			refreshSelection();
			_buffer->setIndices( _buffferIndices );
		}

		void SecondaryStructure::_computeAABB() {}

		void SecondaryStructure::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::Ribbon>( this, ID::View::D3_RIBBON_PATCH ) );
		}

		void SecondaryStructure::setCurrentFrame()
		{
			// TODO: refacto.
			const Molecule::AtomPositionsFrame & positions = _molecule->getAtomPositionFrame( _molecule->getFrame() );

			_bufferPositions.clear();
			_bufferDirections.clear();

			for ( uint chainIdx = 0; chainIdx < _molecule->getChainCount(); ++chainIdx )
			{
				const Chain * const chain		 = _molecule->getChain( chainIdx );
				uint				residueCount = chain->getResidueCount();

				if ( residueCount < 4 )
				{
					VTX_DEBUG( "Chain residue count < 4" );
					continue;
				}

				uint  validResidueCount = 0;
				Vec3f directionLast;
				uint  idxFirstResidue = chain->getIndexFirstResidue();
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					const Residue * const	  residue = _molecule->getResidue( idxFirstResidue + residueIdx );
					const Model::Atom * const CA	  = residue->findFirstAtomByName( "CA" );
					const Model::Atom * const O		  = residue->findFirstAtomByName( "O" );

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

					_bufferPositions.emplace_back( positionCA );

					Vec3f direction = Util::Math::normalize( positionO - positionCA );
					if ( validResidueCount > 0 )
					{
						_flipTest( direction, directionLast );
					}
					_bufferDirections.emplace_back( direction );

					validResidueCount++;
				}
			}

			_bufferPositions.shrink_to_fit();
			_bufferDirections.shrink_to_fit();

			_buffer->setControlPointPositions( _bufferPositions );
			_buffer->setControlPointDirections( _bufferDirections );
		}

		void SecondaryStructure::_fillBufferColors()
		{
			_bufferColors.clear();

			for ( uint chainIdx = 0; chainIdx < _molecule->getChainCount(); ++chainIdx )
			{
				const Chain * const chain		 = _molecule->getChain( chainIdx );
				uint				residueCount = chain->getResidueCount();

				if ( residueCount < 4 )
				{
					continue;
				}

				uint idxFirstResidue = chain->getIndexFirstResidue();
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					const Residue * const	  residue = _molecule->getResidue( idxFirstResidue + residueIdx );
					const Model::Atom * const CA	  = residue->findFirstAtomByName( "CA" );
					const Model::Atom * const O		  = residue->findFirstAtomByName( "O" );

					if ( CA == nullptr || O == nullptr )
					{
						continue;
					}

					switch ( _colorMode )
					{
					case COLOR_MODE::JMOL:
						_bufferColors.emplace_back( COLORS_JMOL[ uint( residue->getSecondaryStructure() ) ] );
						break;
					case COLOR_MODE::PROTEIN:
						_bufferColors.emplace_back( residue->getMoleculePtr()->getColor() );
						break;
					case COLOR_MODE::CHAIN: _bufferColors.emplace_back( residue->getChainPtr()->getColor() ); break;
					case COLOR_MODE::RESIDUE: _bufferColors.emplace_back( residue->getColor() ); break;
					default: _bufferColors.emplace_back( Color::Rgb::WHITE ); break;
					}
				}
			}

			_bufferColors.shrink_to_fit();

			_buffer->setControlPointColors( _bufferColors );
		}

		void SecondaryStructure::_flipTest( Vec3f & p_direction, Vec3f & p_directionLast ) const
		{
			if ( Util::Math::dot( p_direction, p_directionLast ) < 0.f )
			{
				p_direction = -p_direction;
			}
			p_directionLast = p_direction;
		}

		void SecondaryStructure::_fillBufferSelections( const Model::Selection::MapChainIds * const p_selection )
		{
			_bufferSelections.clear();
			_bufferSelections.resize( _bufferPositions.size(), 0 );

			if ( p_selection != nullptr )
			{
				for ( const std::pair<uint, Model::Selection::MapResidueIds> & pairChain : *p_selection )
				{
					for ( const std::pair<uint, Model::Selection::VecAtomIds> & pairResidue : pairChain.second )
					{
						if ( _residueToPositions.find( pairResidue.first ) != _residueToPositions.end() )
						{
							_bufferSelections[ _residueToPositions[ pairResidue.first ] ] = 1u;
						}
					}
				}
			}

			_buffer->setControlPointSelections( _bufferSelections );
		}

		void SecondaryStructure::print() const
		{
			VTX_INFO( "Control points: " + std::to_string( _bufferPositions.size() )
					  + " / Indices: " + std::to_string( _buffferIndices.size() ) );
			VTX_DEBUG( "Sizeof secondary structure: " + std::to_string( sizeof( *this ) ) );
		}

	} // namespace Model
} // namespace VTX
