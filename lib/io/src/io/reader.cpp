// Needed for io/reader.hpp
#include <core/struct/topology.hpp>
#include <util/thread.hpp>
// !Needed for io/reader.hpp
#include "io/reader.hpp"
//
#include <core/chemdb/atom.hpp>
#include <core/chemdb/bond.hpp>
#include <core/chemdb/category.hpp>
#include <core/chemdb/residue.hpp>
#include <core/chemdb/secondary_structure.hpp>
#include <fstream>
#include <map>
#include <optional>
#include <unordered_set>
#include <util/chrono.hpp>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )

namespace VTX::IO
{
	namespace ChemDB = VTX::Core::ChemDB;

	struct SystemReader::_Impl
	{
		VTX::FilePath							filePath;
		READER_OPTION							readerOption;
		std::reference_wrapper<Util::StopToken> stopToken;
		std::optional<std::string>				buffer; // kept alive for memory_reader
		chemfiles::Trajectory					trajectory;
		chemfiles::Frame						currentFrame;
		chemfiles::Topology						topology;
		const std::vector<chemfiles::Residue> * residues		 = nullptr;
		const std::vector<chemfiles::Bond> *	bonds			 = nullptr;
		const chemfiles::Residue *				currentResidue	 = nullptr;
		const chemfiles::Atom *					currentAtom		 = nullptr;
		size_t									currentAtomIndex = 0;
		size_t									currentFrameIdx	 = 0;

		_Impl( const VTX::FilePath & p_path, const READER_OPTION p_options, Util::StopToken & p_stopToken ) :
			filePath( p_path ), readerOption( p_options ), stopToken( p_stopToken ),
			trajectory( chemfiles::Trajectory( p_path.string(), 'r' ) )
		{ _init(); }

		_Impl(
			MemoryBuffer &&		  p_buffer,
			const VTX::FilePath & p_path,
			const READER_OPTION	  p_options,
			Util::StopToken &	  p_stopToken
		) :
			filePath( p_path ), readerOption( p_options ), stopToken( p_stopToken ), buffer( std::move( p_buffer ) ),
			trajectory(
				chemfiles::Trajectory::memory_reader(
					buffer->c_str(),
					buffer->size(),
					chemfiles::guess_format( p_path.string() )
				)
			)
		{ _init(); }

		size_t frameCount() const { return trajectory.size(); }

		void get(
			const Core::ChemDB::Category::Dictionary & p_categories,
			Core::Struct::Topology &				   p_topology,
			Metadata &								   p_metadata
		)
		{
			Util::ScopedChrono chrono( "SystemReader::_Impl::get" );

			if ( stopToken.get().stop_requested() )
			{
				return;
			}

			// Metadata.
			p_metadata.path			= filePath;
			p_metadata.readerOption = readerOption;
			p_metadata.pdbIDCode	= currentFrame.get( "pdb_idcode" ) ? currentFrame.get( "pdb_idcode" )->as_string()
																	   : PDB_ID_CODE_DEFAULT;
			p_metadata.name			= currentFrame.get( "name" ) ? currentFrame.get( "name" )->as_string() : "";
			p_metadata.performedReaderOption = READER_OPTION::NONE;
			p_metadata.topologyState		 = TOPOLOGY_STATE::OK;

			// Strip leading dot from extension (e.g. ".pdb" -> "pdb")
			std::string ext = filePath.extension().string();
			ext				= ext.starts_with( '.' ) ? ext.substr( 1 ) : ext;

			// No residues, create a wrapping one.
			if ( residues->empty() )
			{
				p_metadata.topologyState |= TOPOLOGY_STATE::MISSING_RESIDUES;

				VTX_WARNING( "No residues found, wrapping atoms in unnamed residue" );

				chemfiles::Residue unk( "", 0 );
				for ( size_t i = 0; i < currentFrame.size(); ++i )
				{
					unk.add_atom( i );
				}
				currentFrame.add_residue( unk );
				topology = currentFrame.topology();
				residues = &topology.residues();
				bonds	 = &topology.bonds();
			}

			Index currentChainIndex				= INVALID_INDEX;
			Index currentChainResidueCount		= 0;
			Index expectedNextAtomIndexInChain	= INVALID_INDEX;

			std::unordered_set<std::string>		seenChainNames;
			std::string							previousChainName;
			std::map<Index, std::vector<Index>> mapResidueBonds;
			std::map<Index, std::vector<Index>> mapResidueExtraBonds;

			ChemDB::Category::TYPE lastCategoryEnum = ChemDB::Category::TYPE::UNKNOWN;

			const Index residueCount = Index( residues->size() );
			p_topology.initResidues( residueCount );
			p_topology.initAtoms( Index( currentFrame.size() ) );

			for ( Index residueIdx = 0; residueIdx < residueCount; ++residueIdx )
			{
				if ( stopToken.get().stop_requested() )
				{
					return;
				}

				currentResidue = &( ( *residues )[ residueIdx ] );

				std::string		  chainName	  = _residueStringProp( "chainname" );
				const std::string residueName = currentResidue->name();
				const Index		  residueId	  = Index( currentResidue->id().value_or( INVALID_INDEX ) );
				const Index		  residueFirstAtomIndex
					= currentResidue->size() == 0 ? INVALID_INDEX : Index( *currentResidue->begin() );

				if ( chainName.empty() )
				{
					VTX_WARNING( "Residue {} has no chain name, wrapping residue in unnamed chain", residueIdx );
					p_metadata.topologyState |= TOPOLOGY_STATE::MISSING_CHAIN_INFO;
					chainName = "-";
				}
				else if ( residueIdx > 0 && chainName != previousChainName && seenChainNames.contains( chainName ) )
				{
					VTX_WARNING( "Chain '{}' is used for not contiguous multiple chains", chainName );
					p_metadata.topologyState |= TOPOLOGY_STATE::CHAIN_DEGENERATED;
				}

				const ChemDB::Category::TYPE categoryEnum = _findCategoryType( ext, residueName );

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

					currentChainResidueCount	 = 0;
					expectedNextAtomIndexInChain = INVALID_INDEX;

					if ( not seenChainNames.contains( chainName ) )
					{
						seenChainNames.emplace( chainName );
					}
					lastCategoryEnum = categoryEnum;
				}

				previousChainName = chainName;
				currentChainResidueCount++;

				if ( expectedNextAtomIndexInChain != INVALID_INDEX && residueFirstAtomIndex != INVALID_INDEX
					 && residueFirstAtomIndex != expectedNextAtomIndexInChain )
				{
					VTX_WARNING( "Residues in chain '{}' do not form a contiguous atom range", chainName );
					p_metadata.topologyState |= TOPOLOGY_STATE::CHAIN_ORDER_DEGENERATED;
				}
				if ( residueFirstAtomIndex != INVALID_INDEX )
				{
					expectedNextAtomIndexInChain = residueFirstAtomIndex + Index( currentResidue->size() );
				}

				if ( currentResidue->size() == 0 )
				{
					VTX_WARNING( "Empty residue found" );
				}

				p_topology.residueChainIndexes[ residueIdx ]	 = currentChainIndex;
				p_topology.residueFirstAtomIndexes[ residueIdx ] = residueFirstAtomIndex;
				p_topology.residueAtomCounts[ residueIdx ]		 = Index( currentResidue->size() );
				p_topology.residueOriginalIds[ residueIdx ]		 = residueId;
				p_topology.residueSymbols[ residueIdx ]			 = ChemDB::Residue::getSymbolFromName( residueName );
				p_topology.residueNames[ residueIdx ]			 = residueName;
				ChemDB::Category::get( p_categories, residueName, p_topology.residueCategories[ residueIdx ] );

				const std::string ss = _residueStringProp( "secondary_structure" );
				if ( not ss.empty() )
				{
					p_topology.residueSecondaryStructureTypes[ residueIdx ]
						= ChemDB::SecondaryStructure::pdbFormattedToEnum( ss );
				}

				mapResidueBonds.emplace( residueIdx, std::vector<Index>() );
				mapResidueExtraBonds.emplace( residueIdx, std::vector<Index>() );

				bool  isFirstAtomInResidue = true;
				Index previousAtomIndex	   = INVALID_INDEX;

				for ( chemfiles::Residue::const_iterator it = currentResidue->cbegin(); it != currentResidue->cend();
					  ++it )
				{
					const Index atomIndex = Index( *it );
					currentAtom			  = &currentFrame[ atomIndex ];
					currentAtomIndex	  = atomIndex;

					if ( not isFirstAtomInResidue && atomIndex != previousAtomIndex + 1 )
					{
						VTX_WARNING( "Atoms in residue {} are not contiguous", residueIdx );
						p_metadata.topologyState |= TOPOLOGY_STATE::RESIDUE_DEGENERATED;
					}

					p_topology.atomResidueIndexes[ atomIndex ] = residueIdx;
					p_topology.atomNames[ atomIndex ]		   = currentAtom->name();
					p_topology.atomSymbols[ atomIndex ] = ChemDB::Atom::getSymbolFromString( currentAtom->type() );

					previousAtomIndex	 = atomIndex;
					isFirstAtomInResidue = false;
				}
			}

			if ( currentChainResidueCount != 0 )
			{
				p_topology.chainResidueCounts[ currentChainIndex ] = currentChainResidueCount;
			}

			// Bonds — classify as intra- or extra-residue and order by residue.
			const std::vector<chemfiles::Bond::BondOrder> & bondOrders = topology.bond_orders();
			Index											counter	   = 0;

			for ( Index bondIdx = 0; bondIdx < Index( bonds->size() ); ++bondIdx )
			{
				if ( stopToken.get().stop_requested() )
				{
					return;
				}

				const chemfiles::Bond & bond		  = ( *bonds )[ bondIdx ];
				const Index				firstAtomIdx  = Index( bond[ 0 ] );
				const Index				secondAtomIdx = Index( bond[ 1 ] );
				const Index				residueStart  = p_topology.atomResidueIndexes[ firstAtomIdx ];
				const Index				residueEnd	  = p_topology.atomResidueIndexes[ secondAtomIdx ];

				if ( residueStart >= residueCount || residueEnd >= residueCount )
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

			for ( Index residueIdx = 0; residueIdx < residueCount; ++residueIdx )
			{
				if ( stopToken.get().stop_requested() )
				{
					return;
				}

				const std::vector<Index> & intraBonds = mapResidueBonds[ residueIdx ];
				const std::vector<Index> & extraBonds = mapResidueExtraBonds[ residueIdx ];

				p_topology.residueFirstBondIndexes[ residueIdx ] = counter;
				p_topology.residueBondCounts[ residueIdx ]		 = Index( intraBonds.size() + extraBonds.size() );

				for ( Index i = 0; i < intraBonds.size(); ++i, ++counter )
				{
					const chemfiles::Bond & bond					  = ( *bonds )[ intraBonds[ i ] ];
					p_topology.bondPairAtomIndexes[ counter * 2 ]	  = Index( bond[ 0 ] );
					p_topology.bondPairAtomIndexes[ counter * 2 + 1 ] = Index( bond[ 1 ] );
					p_topology.bondOrders[ counter ] = ChemDB::Bond::ORDER( int( bondOrders[ intraBonds[ i ] ] ) );
				}

				for ( Index i = 0; i < extraBonds.size(); ++i, ++counter )
				{
					const chemfiles::Bond & bond					  = ( *bonds )[ extraBonds[ i ] ];
					p_topology.bondPairAtomIndexes[ counter * 2 ]	  = Index( bond[ 0 ] );
					p_topology.bondPairAtomIndexes[ counter * 2 + 1 ] = Index( bond[ 1 ] );
					p_topology.bondOrders[ counter ] = ChemDB::Bond::ORDER( int( bondOrders[ extraBonds[ i ] ] ) );
				}
			}

			assert( counter == counterOld );
		}

		void get( const FrameIndex & p_frameIndex, AtomPositions & p_positions )
		{
			if ( stopToken.get().stop_requested() )
			{
				return;
			}

			currentFrame	= trajectory.read_at( p_frameIndex );
			currentFrameIdx = p_frameIndex;

			if ( stopToken.get().stop_requested() )
			{
				return;
			}

			const chemfiles::span<chemfiles::Vector3D> & pos = currentFrame.positions();
			p_positions.resize( pos.size() );
			for ( size_t i = 0; i < pos.size(); ++i )
				p_positions[ i ] = Vec3f( pos[ i ][ 0 ], pos[ i ][ 1 ], pos[ i ][ 2 ] );
		}

		void set( Util::StopToken & p_ ) noexcept { stopToken = p_; }

	  private:
		void _init()
		{
			chemfiles::set_warning_callback( []( const std::string & ) {} );

			if ( stopToken.get().stop_requested() )
			{
				return;
			}

			if ( trajectory.size() == 0 )
			{
				throw IOException( "Trajectory is empty" );
			}

			currentFrame = trajectory.read();
			topology	 = currentFrame.topology();
			residues	 = &topology.residues();
			bonds		 = &topology.bonds();

			if ( stopToken.get().stop_requested() )
			{
				return;
			}

			if ( currentFrame.size() != topology.size() )
			{
				throw IOException( "Atom/topology size mismatch" );
			}
		}

		std::string _residueStringProp( const std::string & p_property, const std::string & p_default = "" ) const
		{
			const auto & opt = currentResidue->properties().get( p_property );
			return opt ? opt.value().as_string() : p_default;
		}

		static ChemDB::Category::TYPE _findCategoryType(
			const std::string & p_ext,
			const std::string & /*p_residueName*/
		)
		{
			if ( p_ext == "pdb" || p_ext == "mmcif" || p_ext == "mmtf" )
			{
				return ChemDB::Category::TYPE::POLYMER;
			}
			return ChemDB::Category::TYPE::POLYMER;
		}
	};

	void SystemReader::Del::operator()( _Impl * p_impl ) noexcept { delete p_impl; }

	SystemReader::SystemReader(
		const VTX::FilePath & p_path,
		const READER_OPTION	  p_options,
		Util::StopToken &	  p_stopToken
	) : _impl( new _Impl( p_path, p_options, p_stopToken ) )
	{
	}
	SystemReader::SystemReader(
		MemoryBuffer &&		  p_buffer,
		const VTX::FilePath & p_path,
		const READER_OPTION	  p_options,
		Util::StopToken &	  p_stopToken
	) : _impl( new _Impl( std::move( p_buffer ), p_path, p_options, p_stopToken ) )
	{
	}

	void SystemReader::get( const ChemDB::Category::Dictionary & p_d, Core::Struct::Topology & p_t, Metadata & p_m )
	{ _impl->get( p_d, p_t, p_m ); }
	void SystemReader::get( const FrameIndex & p_i, AtomPositions & p_ ) { _impl->get( p_i, p_ ); }
	void SystemReader::get( AtomPositions & p_ ) { _impl->get( 0, p_ ); }
	void SystemReader::set( Util::StopToken & p_ ) noexcept { _impl->set( p_ ); }

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
