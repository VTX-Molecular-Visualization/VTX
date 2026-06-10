// Needed for io/reader.hpp
#include <core/struct/topology.hpp>
#include <util/thread.hpp>
// !Needed for io/reader.hpp
#include "io/reader.hpp"
//
#include "io/util/bond_order_guessing.hpp"
#include "io/util/bond_recomputation.hpp"
#include "io/util/secondary_structure.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <core/chemdb/atom.hpp>
#include <core/chemdb/bond.hpp>
#include <core/chemdb/category.hpp>
#include <core/chemdb/residue.hpp>
#include <core/chemdb/secondary_structure.hpp>
#include <fstream>
#include <map>
#include <optional>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <util/chrono.hpp>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )

namespace VTX::IO
{
	using namespace VTX::Core::ChemDB;
	using namespace VTX::Core::Struct;
	using namespace VTX::Util;

	struct SystemReader::_Impl
	{
		FilePath								_filePath;
		READER_OPTION							_readerOption;
		std::reference_wrapper<StopToken>		_stopToken;
		std::optional<std::string>				_buffer; // kept alive for memory_reader
		chemfiles::Trajectory					_trajectory;
		chemfiles::Frame						_currentFrame;
		chemfiles::Topology						_topology;
		const std::vector<chemfiles::Residue> * _residues			 = nullptr;
		const std::vector<chemfiles::Bond> *	_bonds				 = nullptr;
		const chemfiles::Residue *				_currentResidue		 = nullptr;
		const chemfiles::Atom *					_currentAtom		 = nullptr;
		int64_t									_currentAtomIndex	 = -1;
		int64_t									_currentFrameIdx	 = -1;
		std::vector<Index>						_atomOriginalIndexes = {};
		AtomPositions							_firstFrame;

		_Impl( const FilePath & p_path, const READER_OPTION p_options, StopToken & p_stopToken ) :
			_filePath( p_path ), _readerOption( p_options ), _stopToken( p_stopToken ),
			_trajectory( chemfiles::Trajectory( p_path.string(), 'r' ) )
		{
			_init();
		}

		_Impl(
			MemoryBuffer &&		p_buffer,
			const FilePath &	p_path,
			const READER_OPTION p_options,
			StopToken &			p_stopToken
		) :
			_filePath( p_path ), _readerOption( p_options ), _stopToken( p_stopToken ),
			_buffer( std::move( p_buffer ) ), _trajectory(
												  chemfiles::Trajectory::memory_reader(
													  _buffer->c_str(),
													  _buffer->size(),
													  chemfiles::guess_format( p_path.string() )
												  )
											  )
		{
			_init();
		}

		size_t frameCount() const { return _trajectory.size(); }

		void get( const Category::Dictionary & p_categories, Topology & p_topology, Metadata & p_metadata )
		{
			ScopedChrono chrono( "SystemReader::_Impl::get" );
			VTX_INFO( "Reading topology" );

			if ( _stopToken.get().stop_requested() )
			{
				return;
			}

			// Metadata.
			_atomOriginalIndexes.clear();
			p_metadata.path			= _filePath;
			p_metadata.readerOption = _readerOption;
			p_metadata.pdbIDCode	= _currentFrame.get( "pdb_idcode" ) ? _currentFrame.get( "pdb_idcode" )->as_string()
																		: PDB_ID_CODE_DEFAULT;
			p_metadata.name			= _currentFrame.get( "name" ) ? _currentFrame.get( "name" )->as_string() : "";
			p_metadata.performedReaderOption = READER_OPTION::NONE;
			p_metadata.missingData			 = MISSING_DATA::SECONDARY_STRUCTURE;
			p_metadata.topologyState		 = TOPOLOGY_STATE::OK;

			// Strip leading dot from extension (e.g. ".pdb" -> "pdb")
			std::string ext = _filePath.extension().string();
			ext				= ext.starts_with( '.' ) ? ext.substr( 1 ) : ext;

			// No residues, rebuild topology with an artificial residue.
			if ( _residues->empty() )
			{
				p_metadata.topologyState |= TOPOLOGY_STATE::MISSING_RESIDUES;
				VTX_WARNING( "No residues found" );
				_retopologize( p_categories, p_topology, p_metadata );
				return;
			}

			Index					  currentChainIndex		   = INVALID_INDEX;
			Index					  currentChainResidueCount = 0;
			Index					  expectedNextAtomIndex	   = 0;
			Index					  coveredAtomCount		   = 0;
			std::unordered_set<Index> recomputableAtomIndexes;
			std::unordered_set<Index> recomputableBondOrderIndexes;

			std::unordered_set<std::string> seenChainNames;
			std::string						previousChainName;

			Category::TYPE lastCategoryEnum = Category::TYPE::UNKNOWN;

			const Index residueCount = static_cast<Index>( _residues->size() );
			p_topology.initResidues( residueCount );
			p_topology.initAtoms( static_cast<Index>( _currentFrame.size() ) );

			for ( Index residueIdx = 0; residueIdx < residueCount; ++residueIdx )
			{
				if ( _stopToken.get().stop_requested() )
				{
					return;
				}

				_currentResidue = &( ( *_residues )[ residueIdx ] );

				std::string		  chainName				= _residueStringProp( "chainname" );
				const std::string residueName			= _currentResidue->name();
				const bool		  isEmptyResidue		= _currentResidue->size() == 0;
				Index			  residueFirstAtomIndex = isEmptyResidue ? static_cast<Index>( _currentFrame.size() )
																		 : static_cast<Index>( *_currentResidue->begin() );
				coveredAtomCount += static_cast<Index>( _currentResidue->size() );

				if ( residueIdx > 0 && chainName != previousChainName && seenChainNames.contains( chainName ) )
				{
					VTX_WARNING( "Chain '{}' is used for not contiguous multiple chains", chainName );
					p_metadata.topologyState |= TOPOLOGY_STATE::CHAIN_DEGENERATED;
					break;
				}

				const Category::TYPE categoryEnum = _findCategoryType( ext, residueName );

				const bool createNewChain = p_topology.getChainCount() == 0 || not seenChainNames.contains( chainName )
											|| categoryEnum != lastCategoryEnum;

				if ( createNewChain )
				{
					if ( currentChainIndex != INVALID_INDEX )
					{
						p_topology.chainResidueCounts[ currentChainIndex ] = currentChainResidueCount;
					}

					p_topology.appendNewChain();
					currentChainIndex++;

					p_topology.chainNames[ currentChainIndex ]		   = chainName;
					p_topology.chainFirstResidues[ currentChainIndex ] = residueIdx;

					currentChainResidueCount = 0;

					if ( not seenChainNames.contains( chainName ) )
					{
						seenChainNames.emplace( chainName );
					}
					lastCategoryEnum = categoryEnum;
				}

				previousChainName = chainName;
				currentChainResidueCount++;

				if ( isEmptyResidue )
				{
					VTX_WARNING( "Empty residue found" );
					residueFirstAtomIndex = expectedNextAtomIndex;
				}

				if ( residueFirstAtomIndex != expectedNextAtomIndex )
				{
					VTX_WARNING( "Residues in chain '{}' do not form a contiguous atom range", chainName );
					p_metadata.topologyState |= TOPOLOGY_STATE::CHAIN_ORDER_DEGENERATED;
					break;
				}
				if ( not isEmptyResidue )
				{
					expectedNextAtomIndex = residueFirstAtomIndex + static_cast<Index>( _currentResidue->size() );
				}

				p_topology.residueChainIndexes[ residueIdx ]	 = currentChainIndex;
				p_topology.residueFirstAtomIndexes[ residueIdx ] = residueFirstAtomIndex;
				p_topology.residueAtomCounts[ residueIdx ]		 = static_cast<Index>( _currentResidue->size() );
				_fillResidue( p_categories, residueIdx, residueIdx, p_topology, p_metadata );

				bool  isFirstAtomInResidue = true;
				Index previousAtomIndex	   = INVALID_INDEX;

				for ( chemfiles::Residue::const_iterator it = _currentResidue->cbegin(); it != _currentResidue->cend();
					  ++it )
				{
					const Index atomIndex = static_cast<Index>( *it );

					if ( not isFirstAtomInResidue && atomIndex != previousAtomIndex + 1 )
					{
						VTX_WARNING( "Atoms in residue {} are not contiguous", residueIdx );
						p_metadata.topologyState |= TOPOLOGY_STATE::RESIDUE_DEGENERATED;
						break;
					}

					_fillAtom( atomIndex, atomIndex, residueIdx, p_topology, recomputableAtomIndexes );

					previousAtomIndex	 = atomIndex;
					isFirstAtomInResidue = false;
				}

				// Retopo.
				if ( p_metadata.topologyState != TOPOLOGY_STATE::OK )
				{
					break;
				}
			}

			if ( p_metadata.topologyState != TOPOLOGY_STATE::OK )
			{
				_retopologize( p_categories, p_topology, p_metadata );
				return;
			}

			if ( coveredAtomCount != static_cast<Index>( _currentFrame.size() ) )
			{
				p_metadata.topologyState |= TOPOLOGY_STATE::ATOM_RESIDUE_MEMBERSHIP;
				VTX_WARNING( "Some atoms are not assigned to any residue" );

				_retopologize( p_categories, p_topology, p_metadata );
				return;
			}

			if ( currentChainResidueCount != 0 )
			{
				p_topology.chainResidueCounts[ currentChainIndex ] = currentChainResidueCount;
			}

			_fillBonds( p_metadata, p_topology, recomputableAtomIndexes, recomputableBondOrderIndexes );
			get( 0, _firstFrame );
			_recomputeMissingData(
				p_metadata, p_topology, _firstFrame, recomputableAtomIndexes, recomputableBondOrderIndexes
			);
		}

		void get( const FrameIndex & p_frameIndex, AtomPositions & p_positions )
		{
			if ( _stopToken.get().stop_requested() )
			{
				return;
			}

			if ( p_frameIndex == 0 && not _firstFrame.empty() )
			{
				p_positions = std::move( _firstFrame );
				_firstFrame.clear();
			}

			_currentFrameIdx = p_frameIndex;
			_currentFrame	 = _trajectory.read_at( p_frameIndex );

			if ( _stopToken.get().stop_requested() )
			{
				return;
			}

			const chemfiles::span<chemfiles::Vector3D> & pos = _currentFrame.positions();
			p_positions.resize( pos.size() );

			// Retopologized.
			if ( not _atomOriginalIndexes.empty() )
			{
				for ( size_t i = 0; i < pos.size(); ++i )
				{
					const Index sourceAtomIndex = _atomOriginalIndexes[ i ];
					p_positions[ i ]			= _toVec3f( pos[ sourceAtomIndex ] );
				}
			}
			// Normal.
			else
			{
				for ( size_t i = 0; i < pos.size(); ++i )
				{
					p_positions[ i ] = _toVec3f( pos[ i ] );
				}
			}
		}

		void set( StopToken & p_ ) noexcept { _stopToken = p_; }

	  private:
		void _init()
		{
			chemfiles::set_warning_callback( []( const std::string & ) {} );

			if ( _stopToken.get().stop_requested() )
			{
				return;
			}

			if ( _trajectory.size() == 0 )
			{
				throw IOException( "Trajectory is empty" );
			}

			_currentFrame = _trajectory.read();
			_topology	  = _currentFrame.topology();
			_residues	  = &_topology.residues();
			_bonds		  = &_topology.bonds();

			if ( _stopToken.get().stop_requested() )
			{
				return;
			}

			if ( _currentFrame.size() != _topology.size() )
			{
				throw IOException( "Atom/topology size mismatch" );
			}
		}

		std::string _residueStringProp( const std::string & p_property, const std::string & p_default = "" ) const
		{
			const auto & opt = _currentResidue->properties().get( p_property );
			return opt ? opt.value().as_string() : p_default;
		}

		static Category::TYPE _findCategoryType(
			const std::string & p_ext,
			const std::string & /*p_residueName*/
		)
		{
			if ( p_ext == "pdb" || p_ext == "mmcif" || p_ext == "mmtf" )
			{
				return Category::TYPE::POLYMER;
			}
			return Category::TYPE::POLYMER;
		}

		void _retopologize( const Category::Dictionary & p_categories, Topology & p_topology, Metadata & p_metadata )
		{
			ScopedChrono chrono( "SystemReader::_Impl::_retopologize" );
			VTX_INFO( "Retopologizing structure" );

			// Clean previous data.
			p_topology = Topology();

			// Temporary structures.
			struct Residue
			{
				Index			   sourceResidueIndex = INVALID_INDEX;
				Index			   firstAtomIndex	  = INVALID_INDEX;
				std::vector<Index> atomIndexes;
				bool			   isArtificial = false;
			};

			struct Chain
			{
				std::string		   name;
				std::vector<Index> residueIndexes;
			};

			// Local data.
			std::vector<Chain>					   chains;
			std::vector<Residue>				   residues;
			std::unordered_map<std::string, Index> chainNameToIndex;
			std::vector<Index>					   oldAtomToNewAtom( _currentFrame.size(), INVALID_INDEX );
			Topology							   topology;

			chains.reserve( _residues->size() );
			residues.reserve( _residues->size() );

			// Create mapping of data old-new.
			for ( Index sourceResidueIndex = 0; sourceResidueIndex < static_cast<Index>( _residues->size() );
				  ++sourceResidueIndex )
			{
				if ( _stopToken.get().stop_requested() )
				{
					return;
				}

				_currentResidue = &( ( *_residues )[ sourceResidueIndex ] );

				if ( _currentResidue->size() == 0 )
				{
					VTX_WARNING( "Empty residue found. Skipping residue." );
					continue;
				}

				const std::string chainName = _residueStringProp( "chainname" );

				auto chainIt = chainNameToIndex.find( chainName );
				if ( chainIt == chainNameToIndex.end() )
				{
					const Index chainIndex = static_cast<Index>( chains.size() );
					chainIt				   = chainNameToIndex.emplace( chainName, chainIndex ).first;
					chains.emplace_back( Chain { .name = chainName } );
				}

				Residue residue { .sourceResidueIndex = sourceResidueIndex };
				residue.atomIndexes.reserve( _currentResidue->size() );

				for ( chemfiles::Residue::const_iterator it = _currentResidue->cbegin(); it != _currentResidue->cend();
					  ++it )
				{
					const Index atomIndex = static_cast<Index>( *it );
					if ( atomIndex >= static_cast<Index>( _currentFrame.size() ) )
					{
						VTX_WARNING( "Residue {} references invalid atom {}", sourceResidueIndex, atomIndex );
						continue;
					}

					if ( oldAtomToNewAtom[ atomIndex ] != INVALID_INDEX )
					{
						VTX_WARNING( "Atom {} is referenced by multiple residues. Skipping duplicate.", atomIndex );
						continue;
					}

					residue.atomIndexes.emplace_back( atomIndex );
					oldAtomToNewAtom[ atomIndex ] = 0;
				}

				if ( residue.atomIndexes.empty() )
				{
					VTX_WARNING( "Empty residue found. Skipping residue." );
					continue;
				}

				std::sort( residue.atomIndexes.begin(), residue.atomIndexes.end() );
				residue.firstAtomIndex = residue.atomIndexes.front();

				const Index residueIndex = static_cast<Index>( residues.size() );
				residues.emplace_back( std::move( residue ) );
				std::vector<Index> & chainResidueIndexes = chains[ chainIt->second ].residueIndexes;
				chainResidueIndexes.emplace_back( residueIndex );
			}

			std::vector<Index> orphanAtomIndexes;
			for ( Index atomIndex = 0; atomIndex < static_cast<Index>( _currentFrame.size() ); ++atomIndex )
			{
				if ( oldAtomToNewAtom[ atomIndex ] == INVALID_INDEX )
				{
					orphanAtomIndexes.emplace_back( atomIndex );
				}
			}

			if ( not orphanAtomIndexes.empty() )
			{
				if ( not _residues->empty() )
				{
					p_metadata.topologyState |= TOPOLOGY_STATE::ATOM_RESIDUE_MEMBERSHIP;
					VTX_WARNING( "Some atoms are not assigned to any residue, wrapping them in an artificial residue" );
				}

				const std::string artificialChainName = "";
				auto			  chainIt			  = chainNameToIndex.find( artificialChainName );
				if ( chainIt == chainNameToIndex.end() )
				{
					const Index chainIndex = static_cast<Index>( chains.size() );
					chainIt				   = chainNameToIndex.emplace( artificialChainName, chainIndex ).first;
					chains.emplace_back( Chain { .name = artificialChainName } );
				}

				Residue residue;
				residue.firstAtomIndex	 = orphanAtomIndexes.front();
				residue.atomIndexes		 = std::move( orphanAtomIndexes );
				residue.isArtificial	 = true;
				const Index residueIndex = static_cast<Index>( residues.size() );
				residues.emplace_back( std::move( residue ) );
				chains[ chainIt->second ].residueIndexes.emplace_back( residueIndex );
			}

			std::fill( oldAtomToNewAtom.begin(), oldAtomToNewAtom.end(), INVALID_INDEX );

			for ( Chain & chain : chains )
			{
				std::sort(
					chain.residueIndexes.begin(),
					chain.residueIndexes.end(),
					[ &residues ]( const Index p_lhs, const Index p_rhs )
					{ return residues[ p_lhs ].firstAtomIndex < residues[ p_rhs ].firstAtomIndex; }
				);
			}

			topology.initChains( static_cast<Index>( chains.size() ) );
			topology.initResidues( static_cast<Index>( residues.size() ) );
			topology.initAtoms( static_cast<Index>( _currentFrame.size() ) );
			_atomOriginalIndexes.resize( _currentFrame.size() );

			Index					  targetResidueIndex = 0;
			Index					  targetAtomIndex	 = 0;
			std::unordered_set<Index> recomputableAtomIndexes;
			std::unordered_set<Index> recomputableBondOrderIndexes;

			// Fill contiguous topology.
			for ( Index chainIndex = 0; chainIndex < static_cast<Index>( chains.size() ); ++chainIndex )
			{
				if ( _stopToken.get().stop_requested() )
				{
					return;
				}

				const Chain & chain = chains[ chainIndex ];

				topology.chainNames[ chainIndex ]		  = chain.name;
				topology.chainFirstResidues[ chainIndex ] = targetResidueIndex;
				topology.chainResidueCounts[ chainIndex ] = static_cast<Index>( chain.residueIndexes.size() );

				for ( const Index residueIndex : chain.residueIndexes )
				{
					if ( _stopToken.get().stop_requested() )
					{
						return;
					}

					Residue & residue = residues[ residueIndex ];

					topology.residueChainIndexes[ targetResidueIndex ]	   = chainIndex;
					topology.residueFirstAtomIndexes[ targetResidueIndex ] = targetAtomIndex;
					topology.residueAtomCounts[ targetResidueIndex ] = static_cast<Index>( residue.atomIndexes.size() );

					if ( residue.isArtificial )
					{
						_fillArtificialResidue( targetResidueIndex, topology );
					}
					else
					{
						_fillResidue(
							p_categories, residue.sourceResidueIndex, targetResidueIndex, topology, p_metadata
						);
					}

					for ( const Index sourceAtomIndex : residue.atomIndexes )
					{
						oldAtomToNewAtom[ sourceAtomIndex ]		= targetAtomIndex;
						_atomOriginalIndexes[ targetAtomIndex ] = sourceAtomIndex;

						_fillAtom(
							sourceAtomIndex, targetAtomIndex, targetResidueIndex, topology, recomputableAtomIndexes
						);
						targetAtomIndex++;
					}

					targetResidueIndex++;
				}
			}

			_fillBonds( p_metadata, topology, recomputableAtomIndexes, recomputableBondOrderIndexes, oldAtomToNewAtom );
			get( 0, _firstFrame );
			_recomputeMissingData(
				p_metadata, topology, _firstFrame, recomputableAtomIndexes, recomputableBondOrderIndexes
			);
			p_topology = std::move( topology );
			// Keep track of original atom indexes for trajectory remapping.
			p_topology.atomOriginalIndexes = _atomOriginalIndexes;
		}

		void _fillArtificialResidue( const Index p_targetResidueIndex, Topology & p_topology )
		{
			p_topology.residueOriginalIds[ p_targetResidueIndex ] = INVALID_INDEX;
			p_topology.residueSymbols[ p_targetResidueIndex ]	  = Residue::SYMBOL::UNKNOWN;
			p_topology.residueNames[ p_targetResidueIndex ]		  = "";
			p_topology.residueCategories[ p_targetResidueIndex ]  = Category::TYPE::UNKNOWN;
			p_topology.categoryResidues[ toUnderlying( Category::TYPE::UNKNOWN ) ].push_back( p_targetResidueIndex );
			p_topology.residueSecondaryStructureTypes[ p_targetResidueIndex ] = SecondaryStructure::TYPE::UNKNOWN;
		}

		void _fillResidue(
			const Category::Dictionary & p_categories,
			const Index					 p_sourceResidueIndex,
			const Index					 p_targetResidueIndex,
			Topology &					 p_topology,
			Metadata &					 p_metadata
		)
		{
			_currentResidue = &( ( *_residues )[ p_sourceResidueIndex ] );

			const std::string residueName = _currentResidue->name();
			const Index		  residueId	  = static_cast<Index>( _currentResidue->id().value_or( INVALID_INDEX ) );

			p_topology.residueOriginalIds[ p_targetResidueIndex ] = residueId;
			p_topology.residueSymbols[ p_targetResidueIndex ]	  = Residue::getSymbolFromName( residueName );
			p_topology.residueNames[ p_targetResidueIndex ]		  = residueName;

			const Category::TYPE category						 = Category::get( p_categories, residueName );
			p_topology.residueCategories[ p_targetResidueIndex ] = category;
			p_topology.categoryResidues[ toUnderlying( category ) ].push_back( p_targetResidueIndex );

			const std::string ss = _residueStringProp( "secondary_structure" );
			if ( not ss.empty() )
			{
				p_topology.residueSecondaryStructureTypes[ p_targetResidueIndex ]
					= SecondaryStructure::pdbFormattedToEnum( ss );
				p_metadata.missingData &= ~MISSING_DATA::SECONDARY_STRUCTURE;
			}
		}

		void _fillAtom(
			const Index					p_sourceAtomIndex,
			const Index					p_targetAtomIndex,
			const Index					p_targetResidueIndex,
			Topology &					p_topology,
			std::unordered_set<Index> & p_recomputableAtomIndexes
		)
		{
			_currentAtom	  = &_currentFrame[ p_sourceAtomIndex ];
			_currentAtomIndex = p_sourceAtomIndex;

			const Atom::SYMBOL atomSymbol = Atom::getSymbolFromString( _currentAtom->type() );

			p_topology.atomResidueIndexes[ p_targetAtomIndex ] = p_targetResidueIndex;
			p_topology.atomNames[ p_targetAtomIndex ]		   = _currentAtom->name();
			p_topology.atomSymbols[ p_targetAtomIndex ]		   = atomSymbol;

			if ( atomSymbol == Atom::SYMBOL::A_N || atomSymbol == Atom::SYMBOL::A_C || atomSymbol == Atom::SYMBOL::A_S
				 || atomSymbol == Atom::SYMBOL::A_P )
			{
				p_recomputableAtomIndexes.emplace( p_targetAtomIndex );
			}
		}

		void _fillBonds(
			Metadata &					 p_metadata,
			Topology &					 p_topology,
			std::unordered_set<Index> &	 p_recomputableAtomIndexes,
			std::unordered_set<Index> &	 p_recomputableBondOrderIndexes,
			const std::span<const Index> p_oldAtomToNewAtom = {}
		)
		{
			std::map<Index, std::vector<Index>> mapResidueBonds;
			std::map<Index, std::vector<Index>> mapResidueExtraBonds;

			for ( Index residueIdx = 0; residueIdx < p_topology.getResidueCount(); ++residueIdx )
			{
				mapResidueBonds.emplace( residueIdx, std::vector<Index>() );
				mapResidueExtraBonds.emplace( residueIdx, std::vector<Index>() );
			}

			Index counter = 0;

			for ( Index bondIdx = 0; bondIdx < static_cast<Index>( _bonds->size() ); ++bondIdx )
			{
				if ( _stopToken.get().stop_requested() )
				{
					return;
				}

				const chemfiles::Bond & bond		  = ( *_bonds )[ bondIdx ];
				Index					firstAtomIdx  = static_cast<Index>( bond[ 0 ] );
				Index					secondAtomIdx = static_cast<Index>( bond[ 1 ] );

				if ( not p_oldAtomToNewAtom.empty() )
				{
					if ( firstAtomIdx >= p_oldAtomToNewAtom.size() || secondAtomIdx >= p_oldAtomToNewAtom.size() )
					{
						VTX_WARNING( "Bond {} has an atom with invalid source index. Skipping.", bondIdx );
						continue;
					}

					firstAtomIdx  = p_oldAtomToNewAtom[ firstAtomIdx ];
					secondAtomIdx = p_oldAtomToNewAtom[ secondAtomIdx ];
				}

				if ( firstAtomIdx >= p_topology.getAtomCount() || secondAtomIdx >= p_topology.getAtomCount() )
				{
					VTX_WARNING( "Bond {} has an atom with invalid index. Skipping.", bondIdx );
					continue;
				}

				p_recomputableAtomIndexes.erase( firstAtomIdx );
				p_recomputableAtomIndexes.erase( secondAtomIdx );

				const Index residueStart = p_topology.atomResidueIndexes[ firstAtomIdx ];
				const Index residueEnd	 = p_topology.atomResidueIndexes[ secondAtomIdx ];

				if ( residueStart >= p_topology.getResidueCount() || residueEnd >= p_topology.getResidueCount() )
				{
					VTX_WARNING(
						"Bond {} has an atom with invalid residue index ({} or {}). Skipping.",
						bondIdx,
						residueStart,
						residueEnd
					);
					continue;
				}

				if ( residueStart == residueEnd )
				{
					mapResidueBonds[ residueStart ].emplace_back( bondIdx );
					counter++;
				}
				else
				{
					mapResidueExtraBonds[ residueStart ].emplace_back( bondIdx );
					mapResidueExtraBonds[ residueEnd ].emplace_back( bondIdx );
					counter += 2;
				}
			}

			p_topology.initBonds( counter );

			const Index counterOld = counter;
			counter				   = 0;

			for ( Index residueIdx = 0; residueIdx < p_topology.getResidueCount(); ++residueIdx )
			{
				if ( _stopToken.get().stop_requested() )
				{
					return;
				}

				const std::vector<Index> & intraBonds = mapResidueBonds[ residueIdx ];
				const std::vector<Index> & extraBonds = mapResidueExtraBonds[ residueIdx ];

				p_topology.residueFirstBondIndexes[ residueIdx ] = counter;
				p_topology.residueBondCounts[ residueIdx ]
					= static_cast<Index>( intraBonds.size() + extraBonds.size() );

				for ( Index i = 0; i < intraBonds.size(); ++i, ++counter )
				{
					_fillBond(
						intraBonds[ i ], counter, p_topology, p_recomputableBondOrderIndexes, p_oldAtomToNewAtom
					);
				}

				for ( Index i = 0; i < extraBonds.size(); ++i, ++counter )
				{
					_fillBond(
						extraBonds[ i ], counter, p_topology, p_recomputableBondOrderIndexes, p_oldAtomToNewAtom
					);
				}
			}

			if ( not p_recomputableBondOrderIndexes.empty() )
			{
				p_metadata.missingData |= MISSING_DATA::BOND_ORDERS;
			}

			if ( not p_recomputableAtomIndexes.empty() )
			{
				p_metadata.missingData |= MISSING_DATA::BONDS;
			}

			assert( counter == counterOld );
		}

		void _fillBond(
			const Index					 p_sourceBondIndex,
			const Index					 p_targetBondIndex,
			Topology &					 p_topology,
			std::unordered_set<Index> &	 p_recomputableBondOrderIndexes,
			const std::span<const Index> p_oldAtomToNewAtom = {}
		)
		{
			const std::vector<chemfiles::Bond::BondOrder> & bondOrders = _topology.bond_orders();
			const chemfiles::Bond &							bond	   = ( *_bonds )[ p_sourceBondIndex ];
			Index											firstAtom  = static_cast<Index>( bond[ 0 ] );
			Index											secondAtom = static_cast<Index>( bond[ 1 ] );

			if ( not p_oldAtomToNewAtom.empty() )
			{
				if ( firstAtom >= p_oldAtomToNewAtom.size() || secondAtom >= p_oldAtomToNewAtom.size() )
				{
					VTX_WARNING( "Bond {} has an atom with invalid source index. Skipping.", p_sourceBondIndex );
					return;
				}

				firstAtom  = p_oldAtomToNewAtom[ firstAtom ];
				secondAtom = p_oldAtomToNewAtom[ secondAtom ];
			}

			const Bond::ORDER bondOrder = p_sourceBondIndex < bondOrders.size()
											  ? Bond::ORDER( int( bondOrders[ p_sourceBondIndex ] ) )
											  : Bond::ORDER::UNKNOWN;

			p_topology.bondPairAtomIndexes[ p_targetBondIndex * 2 ]		= firstAtom;
			p_topology.bondPairAtomIndexes[ p_targetBondIndex * 2 + 1 ] = secondAtom;
			p_topology.bondOrders[ p_targetBondIndex ]					= bondOrder;

			if ( bondOrder == Bond::ORDER::UNKNOWN )
			{
				p_recomputableBondOrderIndexes.emplace( p_targetBondIndex );
			}
		}

		void _recomputeMissingData(
			Metadata &						  p_metadata,
			Topology &						  p_topology,
			const AtomPositions &			  p_positions,
			const std::unordered_set<Index> & p_recomputableAtomIndexes,
			const std::unordered_set<Index> & p_recomputableBondOrderIndexes
		)
		{
			assert( p_positions.size() == p_topology.getAtomCount() );

			if ( Enum::hasAnyBit( _readerOption, READER_OPTION::RECOMPUTE_MISSING_BONDS )
				 && Enum::hasAnyBit( p_metadata.missingData, MISSING_DATA::BONDS ) )
			{
				Util::BondRecomputation::recomputeBonds( p_topology, p_positions, p_recomputableAtomIndexes );
				p_metadata.performedReaderOption |= READER_OPTION::RECOMPUTE_MISSING_BONDS;
			}
			if ( Enum::hasAnyBit( _readerOption, READER_OPTION::GUESS_UNKNOWN_BOND_ORDERS )
				 && Enum::hasAnyBit( p_metadata.missingData, MISSING_DATA::BOND_ORDERS ) )
			{
				Util::BondOrderGuessing::recomputeBondOrders( p_topology, p_positions, p_recomputableBondOrderIndexes );
				p_metadata.performedReaderOption |= READER_OPTION::GUESS_UNKNOWN_BOND_ORDERS;
			}
			if ( Enum::hasAnyBit( _readerOption, READER_OPTION::COMPUTE_MISSING_SECONDARY_STRUCTURE )
				 && Enum::hasAnyBit( p_metadata.missingData, MISSING_DATA::SECONDARY_STRUCTURE ) )
			{
				Util::SecondaryStructure::assignSecondaryStructure( p_topology, p_positions );
				p_metadata.performedReaderOption |= READER_OPTION::COMPUTE_MISSING_SECONDARY_STRUCTURE;
			}
		}

		static Vec3f _toVec3f( const chemfiles::Vector3D & p_position )
		{
			return Vec3f( p_position[ 0 ], p_position[ 1 ], p_position[ 2 ] );
		}
	};

	void SystemReader::Del::operator()( _Impl * p_impl ) noexcept { delete p_impl; }

	SystemReader::SystemReader( const FilePath & p_path, const READER_OPTION p_options, StopToken & p_stopToken ) :
		_impl( new _Impl( p_path, p_options, p_stopToken ) )
	{
	}

	SystemReader::SystemReader(
		MemoryBuffer &&		p_buffer,
		const FilePath &	p_path,
		const READER_OPTION p_options,
		StopToken &			p_stopToken
	) : _impl( new _Impl( std::move( p_buffer ), p_path, p_options, p_stopToken ) )
	{
	}

	void SystemReader::get( const Category::Dictionary & p_d, Topology & p_t, Metadata & p_m )
	{
		_impl->get( p_d, p_t, p_m );
	}

	void SystemReader::get( const FrameIndex & p_i, AtomPositions & p_ ) { _impl->get( p_i, p_ ); }

	void SystemReader::get( AtomPositions & p_ ) { _impl->get( 0, p_ ); }

	void SystemReader::set( StopToken & p_ ) noexcept { _impl->set( p_ ); }

	size_t SystemReader::frameCount() const { return _impl->frameCount(); }

	bool isTrajectoryFileFormat( const FilePath & p_path ) noexcept
	{
		std::string extension = p_path.extension().string();
		std::transform( extension.begin(), extension.end(), extension.begin(), tolower );
		const std::array<std::string_view, 4> _trajectoryFileFormatList { ".xtc", ".dcd", ".lammpstrj", ".trj" };
		return std::any_of(
			_trajectoryFileFormatList.begin(),
			_trajectoryFileFormatList.end(),
			[ &extension ]( const std::string_view & v ) { return v == extension; }
		);
	};

} // namespace VTX::IO
