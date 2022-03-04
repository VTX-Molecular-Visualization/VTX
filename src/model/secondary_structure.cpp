#include "secondary_structure.hpp"
#include "id.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "representation/representation_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "view/d3/ribbon.hpp"

namespace VTX
{
	namespace Model
	{
		SecondaryStructure::SecondaryStructure( Molecule * const p_molecule ) :
			BaseModel3D( VTX::ID::Model::MODEL_SECONDARY_STRUCTURE ), _molecule( p_molecule )
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating secondary structure..." );

			refresh( false );

			chrono.stop();
			VTX_INFO( "Secondary structure created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SecondaryStructure::refresh( const bool p_refreshBuffers )
		{
			_bufferCaPositions.clear();
			_bufferCaODirections.clear();
			_bufferSSTypes.clear();
			_bufferColors.clear();
			_bufferVisibilities.clear();
			_bufferIds.clear();
			_bufferIndices.clear();
			_residueToPositions.clear();
			_residueToIndices.clear();
			_data.clear();

			const Molecule::AtomPositionsFrame & positions = _molecule->getAtomPositionFrame( _molecule->getFrame() );

			// Temporary vectors, merged with buffers is SS is constructed.
			std::vector<Vec4f>		caPositions;
			std::vector<Vec3f>		caODirections;
			std::vector<uint>		ssTypes;
			std::vector<Color::Rgb> colors;
			std::vector<uint>		visibilities;
			std::vector<Model::ID>	ids;
			std::vector<uint>		residueIndex;

			for ( uint chainIdx = 0; chainIdx < _molecule->getChainCount(); ++chainIdx )
			{
				const Chain * const chain = _molecule->getChain( chainIdx );
				if ( chain == nullptr )
				{
					continue;
				}

				uint residueCount = chain->getResidueCount();

				// Not enought residues.
				if ( residueCount < 4 ) /// TODO: what to do ?
				{
					// VTX_DEBUG( "Chain residue count < 4" );
					// std::cout << "residue count < 4 in chain " << chain->getName() << std::endl;
					continue;
				}

				uint			idxFirstResidue = chain->getIndexFirstResidue();
				bool			createVectors	= true;
				const Residue * residueLast		= nullptr;
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					if ( createVectors )
					{
						caPositions	  = std::vector<Vec4f>();
						caODirections = std::vector<Vec3f>();
						ssTypes		  = std::vector<uint>();
						colors		  = std::vector<Color::Rgb>();
						visibilities  = std::vector<uint>();
						ids			  = std::vector<Model::ID>();
						residueIndex  = std::vector<uint>();

						createVectors = false;
					}

					const Residue * const residue = _molecule->getResidue( idxFirstResidue + residueIdx );

					if ( residue == nullptr )
					{
						continue;
					}

					// Use backbone to compute spline data.
					// Find alpha carbon.
					const Model::Atom * const CA = residue->getAlphaCarbon();

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

					// Add secondary structure type.
					ssTypes.emplace_back( uint( residue->getSecondaryStructure() ) );

					// Add color.
					if ( residue->getRepresentation() != nullptr )
					{
						switch ( residue->getRepresentation()->getRibbonData().colorMode )
						{
						case Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL:
							colors.emplace_back( Generic::COLORS_JMOL[ uint( residue->getSecondaryStructure() ) ] );
							break;
						case Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN:
							colors.emplace_back( residue->getMoleculePtr()->getColor() );
							break;
						case Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM:
							colors.emplace_back( residue->getRepresentation()->getColor() );
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
					else
					{
						colors.emplace_back( Generic::COLORS_JMOL[ uint( residue->getSecondaryStructure() ) ] );
					}

					visibilities.emplace_back( uint( _molecule->isVisible() && chain->isVisible()
													 && residue->isVisible() && CA->isVisible() && O->isVisible() ) );

					ids.emplace_back( residue->getId() );
					if ( residueLast != nullptr
						 && residue->getIndexInOriginalChain() != residueLast->getIndexInOriginalChain() + 1 )
					{
						_tryConstruct(
							chainIdx, residueIndex, caPositions, caODirections, ssTypes, colors, visibilities, ids );
						createVectors = true;
					}

					residueLast = residue;
				}

				// Update buffers and index mapping if SS is constructed.
				_tryConstruct( chainIdx, residueIndex, caPositions, caODirections, ssTypes, colors, visibilities, ids );
			}

			// Reverse indices to render the other side.
			// std::vector<uint> indicesReverse = _bufferIndices;
			// std::reverse( indicesReverse.begin(), indicesReverse.end() );
			//_bufferIndices.insert( _bufferIndices.end(), indicesReverse.begin(), indicesReverse.end() );

			_bufferCaPositions.shrink_to_fit();
			_bufferCaODirections.shrink_to_fit();
			_bufferSSTypes.shrink_to_fit();
			_bufferColors.shrink_to_fit();
			_bufferVisibilities.shrink_to_fit();
			_bufferIds.shrink_to_fit();
			_bufferIndices.shrink_to_fit();

			if ( p_refreshBuffers )
			{
				_fillBuffer();
			}
		}

		void SecondaryStructure::_tryConstruct( const uint						p_chainIdx,
												const std::vector<uint> &		p_residueIndex,
												const std::vector<Vec4f> &		p_caPositions,
												std::vector<Vec3f> &			p_caODirections,
												const std::vector<uint> &		p_ssTypes,
												const std::vector<Color::Rgb> & p_colors,
												const std::vector<uint> &		p_visibilities,
												const std::vector<Model::ID> &	p_ids )
		{
			if ( p_caPositions.size() >= 4 )
			{
				const size_t nbControlPoints = p_caPositions.size();

				_residueToPositions.emplace( p_residueIndex[ 0 ], uint( _bufferCaPositions.size() ) );
				_residueToIndices.emplace( p_residueIndex[ 0 ], uint( _bufferIndices.size() ) );

				const uint offset = uint( _bufferCaPositions.size() );

				// Add segment with duplicate first index to evaluate B-spline at 0-1.
				_bufferIndices.emplace_back( offset );
				_bufferIndices.emplace_back( offset );
				_bufferIndices.emplace_back( offset + 1 );
				_bufferIndices.emplace_back( offset + 2 );

				for ( uint i = 1; i < nbControlPoints - 2; ++i )
				{
					_residueToPositions.emplace( p_residueIndex[ i ], uint( _bufferCaPositions.size() + i ) );
					_residueToIndices.emplace( p_residueIndex[ i ], uint( _bufferIndices.size() ) );

					_bufferIndices.emplace_back( offset + i - 1 );
					_bufferIndices.emplace_back( offset + i );
					_bufferIndices.emplace_back( offset + i + 1 );
					_bufferIndices.emplace_back( offset + i + 2 );
				}

				/// TODO: better on GPU ?
				_checkOrientationAndFlip( p_caODirections );

				// Merge buffers.
				auto it = _data.find( p_chainIdx );
				if ( it == _data.end() )
				{
					_data.emplace( p_chainIdx, std::vector<uint>() );
				}
				_data[ p_chainIdx ].insert(
					std::end( _data[ p_chainIdx ] ), std::begin( p_residueIndex ), std::end( p_residueIndex ) );

				_bufferCaPositions.insert( _bufferCaPositions.end(), p_caPositions.cbegin(), p_caPositions.cend() );
				_bufferCaODirections.insert(
					_bufferCaODirections.end(), p_caODirections.cbegin(), p_caODirections.cend() );
				_bufferSSTypes.insert( _bufferSSTypes.end(), p_ssTypes.cbegin(), p_ssTypes.cend() );
				_bufferColors.insert( _bufferColors.end(), p_colors.cbegin(), p_colors.cend() );
				_bufferVisibilities.insert( _bufferVisibilities.end(), p_visibilities.cbegin(), p_visibilities.cend() );
				_bufferIds.insert( _bufferIds.end(), p_ids.cbegin(), p_ids.cend() );
			}
		}

		void SecondaryStructure::_init() {}

		void SecondaryStructure::_fillBuffer()
		{
			assert( _bufferCaPositions.size() == _bufferCaODirections.size() );
			assert( _bufferCaPositions.size() == _bufferSSTypes.size() );
			assert( _bufferCaPositions.size() == _bufferColors.size() );
			assert( _bufferCaPositions.size() == _bufferVisibilities.size() );
			assert( _bufferCaPositions.size() == _bufferIds.size() );

			_buffer->setControlPointPositions( _bufferCaPositions );
			_buffer->setControlPointDirections( _bufferCaODirections );
			_buffer->setSecondaryStructures( _bufferSSTypes );
			_buffer->setColors( _bufferColors );
			_buffer->setVisibilities( _bufferVisibilities );
			_buffer->setIds( _bufferIds );
			_buffer->setIndices( _bufferIndices );

			refreshSelection();
		}

		const Math::AABB & SecondaryStructure::getAABB() const { return _molecule->getAABB(); }

		const Math::Transform & SecondaryStructure::getTransform() const { return _molecule->getTransform(); };

		void SecondaryStructure::_computeAABB() const
		{
			/// TODO
		}

		void SecondaryStructure::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::Ribbon>( this, VTX::ID::View::D3_RIBBON_PATCH ) );
		}

		void SecondaryStructure::refreshColors()
		{
			_bufferColors.clear();
			_bufferColors.reserve( _bufferCaPositions.size() );
			for ( const SSDataPair & pair : _data )
			{
				for ( const uint residueIdx : pair.second )
				{
					const Residue * const residue = _molecule->getResidue( residueIdx );

					if ( residue == nullptr )
						continue;

					Generic::SECONDARY_STRUCTURE_COLOR_MODE colorMode = Setting::SS_COLOR_MODE_DEFAULT;
					if ( residue->getRepresentation()->hasToDrawRibbon() )
					{
						colorMode = residue->getRepresentation()->getRibbonData().colorMode;
					}
					switch ( colorMode )
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
			_buffer->setColors( _bufferColors );
		}

		void SecondaryStructure::refreshVisibilities()
		{
			_bufferVisibilities.clear();
			_bufferVisibilities.reserve( _bufferCaPositions.size() );
			for ( const SSDataPair & pair : _data )
			{
				const Chain * const chain = _molecule->getChain( pair.first );
				for ( const uint residueIdx : pair.second )
				{
					const Residue * const	  residue = _molecule->getResidue( residueIdx );
					const Model::Atom * const CA	  = residue->getAlphaCarbon();
					const Model::Atom * const O		  = residue->findFirstAtomByName( "O" );
					_bufferVisibilities.emplace_back( _molecule->isVisible() && chain->isVisible()
													  && residue->isVisible() && CA->isVisible() && O->isVisible() );
				}
			}

			_bufferVisibilities.shrink_to_fit();
			_buffer->setVisibilities( _bufferVisibilities );
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
				for ( const Model::Selection::PairChainIds & pairChain : *p_selection )
				{
					for ( const Model::Selection::PairResidueIds & pairResidue : pairChain.second )
					{
						if ( _residueToPositions.find( pairResidue.first ) != _residueToPositions.end() )
						{
							_bufferSelections[ _residueToPositions[ pairResidue.first ] ] = 1u;
						}
					}
				}
			}

			_bufferSelections.shrink_to_fit();
			_buffer->setSelections( _bufferSelections );
		}

		void SecondaryStructure::print() const
		{
			VTX_INFO( "Control points: " + std::to_string( _bufferCaPositions.size() )
					  + " / Indices: " + std::to_string( _bufferIndices.size() ) );
			VTX_DEBUG( "Sizeof secondary structure: " + std::to_string( sizeof( *this ) ) );
		}

	} // namespace Model
} // namespace VTX
