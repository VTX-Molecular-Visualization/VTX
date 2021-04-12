#include "secondary_structure.hpp"
#include "id.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "view/d3/ribbon.hpp"

namespace VTX
{
	namespace Model
	{
		SecondaryStructure::SecondaryStructure( Molecule * const p_molecule ) :
			BaseModel3D( ID::Model::MODEL_SECONDARY_STRUCTURE ), _molecule( p_molecule )
		{
			/// TODO: refacto.
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating secondary structure..." );

			const Molecule::AtomPositionsFrame & positions = p_molecule->getAtomPositionFrame( p_molecule->getFrame() );

			float dirFlag = 0.f; // Loop over chains (1 chain = 1 ribbon).
			for ( uint chainIdx = 0; chainIdx < p_molecule->getChainCount(); ++chainIdx )
			{
				const Chain * const chain = p_molecule->getChain( chainIdx );
				if ( chain == nullptr ) /// TODO: possible ? what to do ?
					continue;

				uint residueCount = chain->getResidueCount();

				// Not enought residues.
				if ( residueCount < 4 ) /// TODO: what to do ?
				{
					// VTX_DEBUG( "Chain residue count < 4" );
					// std::cout << "residue count < 4 in chain " << chain->getName() << std::endl;
					continue;
				}

				// Temporary vectors, merged with buffers is SS is constructed.
				std::vector<Vec4f>		caPositions;
				std::vector<Vec3f>		caODirections;
				std::vector<ushort>		ssTypes;
				std::vector<Color::Rgb> colors;
				std::vector<uint>		residueIndex;

				uint idxFirstResidue = chain->getIndexFirstResidue();
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					const Residue * const residue = p_molecule->getResidue( idxFirstResidue + residueIdx );

					if ( residue == nullptr ) /// TODO: possible? what to do?
						continue;

					// Use backbone to compute spline data.
					// Find alpha carbon.
					const Model::Atom * const CA = residue->findFirstAtomByName( "CA" );

					// Not an amine acid (water, heme, or phosphate groupment).
					if ( CA == nullptr ) /// TODO: what to do ?
					{
						// What to do, skip residue, skip all the chain or split the chain into multiple ribbons?
						const std::string msg = "Missing carbon alpha in chain " + chain->getName() + " residue "
												+ residue->getSymbolName();
						// VTX_DEBUG( msg );
						continue;
					}

					// Find oxygen.
					const Model::Atom * const O = residue->findFirstAtomByName( "O" );
					// Missing oxygen atom.
					if ( O == nullptr ) /// TODO: what to do?
					{
						const std::string msg
							= "Missing oxygen in chain " + chain->getName() + " residue " + residue->getSymbolName();
						// VTX_DEBUG( msg );
						continue;
					}
					/// TODO: For all these "what to do ?" I think we should render it with spheres or b&s...

					// Compute direction between carbon alpha and oxygen.
					const Vec3f & positionCA   = positions[ CA->getIndex() ];
					const Vec3f & positionO	   = positions[ O->getIndex() ];
					const Vec3f	  directionCAO = Util::Math::normalize( positionO - positionCA );

					// Store residue index for later.
					residueIndex.emplace_back( residue->getIndex() );

					// Add carbon alpha (CA) position and CA-O direction.
					caPositions.emplace_back(
						Vec4f( positionCA, float( _bufferCaPositions.size() + caPositions.size() ) ) );
					caODirections.emplace_back( directionCAO );
					// Repeat first control point.
					if ( caPositions.size() == 1 )
					{
						caPositions.emplace_back(
							Vec4f( positionCA, float( _bufferCaPositions.size() + caPositions.size() ) ) );
						caODirections.emplace_back( directionCAO );
					}

					// Add secondary structure type.
					ssTypes.emplace_back( ushort( residue->getSecondaryStructure() ) );

					// Add color.
					switch ( _colorMode )
					{
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
						colors.emplace_back( Generic::COLORS_JMOL[ uint( residue->getSecondaryStructure() ) ] );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN:
						colors.emplace_back( residue->getMoleculePtr()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
						colors.emplace_back( residue->getChainPtr()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE:
						colors.emplace_back( residue->getColor() );
						break;
					default: colors.emplace_back( Color::Rgb::WHITE ); break;
					}
				}

				// Update buffers and index mapping if SS is constructed.
				if ( caPositions.size() >= 4 )
				{
					// Repeat last control point.
					caPositions.emplace_back( caPositions.back() );
					caPositions.back().w++; // Don't forget to increase direction flag.
					caODirections.emplace_back( caODirections.back() );

					const size_t nbControlPoints = caPositions.size();

					//_residueToPositions.emplace( residueIndex[ 0 ], uint( _bufferCaPositions.size() ) );
					const uint offset = uint( _bufferCaPositions.size() );

					// Add segment with duplicate first index to evaluate B-spline at 0-1.
					/*_bufferIndices.emplace_back( offset );
					_bufferIndices.emplace_back( offset );
					_bufferIndices.emplace_back( offset + 1 );
					_bufferIndices.emplace_back( offset + 2 );*/

					for ( uint i = 1; i < nbControlPoints - 2; ++i )
					{
						// map avec l'id des residue (in ibo) pour le drawcall
						_residueToIndices.emplace( residueIndex[ i ], uint( _bufferIndices.size() ) );
						// balec visibility
						_residueToPositions.emplace( residueIndex[ i ], uint( _bufferCaPositions.size() + i ) );

						_bufferIndices.emplace_back( offset + i - 1 );
						_bufferIndices.emplace_back( offset + i );
						_bufferIndices.emplace_back( offset + i + 1 );
						_bufferIndices.emplace_back( offset + i + 2 );
					}

					// Update mappping.
					/*
					_residueToPositions.emplace( residueIndex[ nbControlPoints - 1 ],
												 uint( _bufferCaPositions.size() + nbControlPoints - 1 ) );
					_residueToPositions.emplace( residueIndex[ nbControlPoints - 2 ],
												 uint( _bufferCaPositions.size() + nbControlPoints - 2 ) );
*/
					/// TODO: better on GPU ?
					_checkOrientationAndFlip( caODirections );

					// Merge buffers.
					_bufferCaPositions.insert( _bufferCaPositions.end(), caPositions.cbegin(), caPositions.cend() );
					_bufferCaODirections.insert(
						_bufferCaODirections.end(), caODirections.cbegin(), caODirections.cend() );
					_bufferSSTypes.insert( _bufferSSTypes.end(), ssTypes.cbegin(), ssTypes.cend() );
					_bufferColors.insert( _bufferColors.end(), colors.cbegin(), colors.cend() );
				}
			}

			// Reverse indices to render the other side.
			// std::vector<uint> indicesReverse = _bufferIndices;
			// std::reverse( indicesReverse.begin(), indicesReverse.end() );
			//_bufferIndices.insert( _bufferIndices.end(), indicesReverse.begin(), indicesReverse.end() );

			_bufferCaPositions.shrink_to_fit();
			_bufferCaODirections.shrink_to_fit();
			_bufferSSTypes.shrink_to_fit();
			_bufferColors.shrink_to_fit();
			_bufferIndices.shrink_to_fit();

			chrono.stop();
			VTX_INFO( "Secondary structure created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SecondaryStructure::_init() {}

		void SecondaryStructure::_fillBuffer()
		{
			_buffer->setControlPointPositions( _bufferCaPositions );
			_buffer->setControlPointDirections( _bufferCaODirections );
			_buffer->setControlPointSecondaryStructure( _bufferSSTypes );
			_buffer->setControlPointColors( _bufferColors );
			refreshSelection();
			_buffer->setIndices( _bufferIndices );
		}

		const Math::AABB & SecondaryStructure::getAABB() const { return _molecule->getAABB(); }

		const Math::Transform & SecondaryStructure::getTransform() const { return _molecule->getTransform(); };

		void SecondaryStructure::_computeAABB() const
		{
			/// TODO peut-�tre ?
		}

		void SecondaryStructure::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::Ribbon>( this, ID::View::D3_RIBBON_PATCH ) );
		}

		void SecondaryStructure::setCurrentFrame()
		{
			// TODO: redo with dynamic.
			VTX_WARNING( "SecondaryStructure::setCurrentFrame() not implemented" );
		}

		void SecondaryStructure::_fillBufferColors()
		{
			_bufferColors.clear();
			for ( uint chainIdx = 0; chainIdx < _molecule->getChainCount(); ++chainIdx )
			{
				const Chain * const chain = _molecule->getChain( chainIdx );

				if ( chain == nullptr )
				{
					continue;
				}

				uint residueCount = chain->getResidueCount();

				if ( residueCount < 4 )
				{
					continue;
				}

				uint idxFirstResidue = chain->getIndexFirstResidue();
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					const Residue * const residue = _molecule->getResidue( idxFirstResidue + residueIdx );

					if ( residue == nullptr )
						continue;

					const Model::Atom * const CA = residue->findFirstAtomByName( "CA" );
					const Model::Atom * const O	 = residue->findFirstAtomByName( "O" );

					if ( CA == nullptr || O == nullptr )
					{
						continue;
					}

					switch ( residue->getRepresentation()->getSecondaryStructureColorMode() )
					{
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
						_bufferColors.emplace_back( Generic::COLORS_JMOL[ uint( residue->getSecondaryStructure() ) ] );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN:
						_bufferColors.emplace_back( residue->getMoleculePtr()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
						_bufferColors.emplace_back( residue->getRepresentation()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN:
						_bufferColors.emplace_back( residue->getChainPtr()->getColor() );
						break;
					case Generic::SECONDARY_STRUCTURE_COLOR_MODE::RESIDUE:
						_bufferColors.emplace_back( residue->getColor() );
						break;

					default: _bufferColors.emplace_back( Color::Rgb::WHITE ); break;
					}
				}
			}

			_bufferColors.shrink_to_fit();
			_buffer->setControlPointColors( _bufferColors );
		}

		void SecondaryStructure::_checkOrientationAndFlip( std::vector<Vec3f> & p_directions )
		{
			size_t i;
			for ( i = 1; i < p_directions.size(); ++i )
			{
				if ( Util::Math::dot( p_directions[ i ], p_directions[ i - 1 ] ) < 0.f )
				{
					p_directions[ i ] = -p_directions[ i ];
				}
			}
		}

		void SecondaryStructure::_fillBufferSelections( const Model::Selection::MapChainIds * const p_selection )
		{
			_bufferSelections.clear();
			_bufferSelections.resize( _bufferCaPositions.size(), 0 );

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
			VTX_INFO( "Control points: " + std::to_string( _bufferCaPositions.size() )
					  + " / Indices: " + std::to_string( _bufferIndices.size() ) );
			VTX_DEBUG( "Sizeof secondary structure: " + std::to_string( sizeof( *this ) ) );
		}

	} // namespace Model
} // namespace VTX
