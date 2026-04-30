#include "io/writer/system.hpp"
#include "io/writer/chemfiles.hpp"
#include <util/exceptions.hpp>

namespace VTX::IO::Writer
{
	namespace
	{
		/**
		 * @brief Class used to convert topology atom index into new system index
		 */
		class SystemIndexManager
		{
		  public:
			SystemIndexManager() = delete;
			SystemIndexManager( const std::vector<WriteArgs::System> & p_topols ) : _topologies( p_topols ) {}

			// > Thoughs
			// On the write main loop, we have addChain, bonds, and frame functions.
			// addChain will eventually loop over atoms in a inconsistent order and add them to the writesystem
			// bonds and frame functions will loops over all atoms at once.
			// However, indexing can be fucked up in multiple ways :
			// If there is one system to write from, we would rather keep original atom IDs
			// If there is multiple systems, we need to re-index everything in a new way, accounting filtered atoms and
			// atoms added through a previous system.
			//
			// So we need to keep track of atomIndexes that refer to the
			// topology, and link them to the atomIndex as it has been added to the writeSystem.
			//
			// Knowing that, we need to think about how to make things as simple as possible

			/**
			 * @brief Generate an atom index for the write system. The input index will refer to the current system
			 * being exploited.
			 * @param p_currentSystemAtomIdx
			 * @return
			 */
			inline uint newAtomIdx( const uint & p_currentSystemAtomIdx )
			{
				if ( _multiSystem )
					return _currentSystemAtomIdxMap[ p_currentSystemAtomIdx ] = ++_lastAtomIdx;
				else if ( _currentSystem->topology->atomOriginalIndexes )
					return _currentSystem->topology->atomOriginalIndexes.value()[ p_currentSystemAtomIdx ];
				else
					return p_currentSystemAtomIdx;
			}
			inline const uint & getAtomIdx( const uint & p_currentSystemAtomIdx ) const
			{
				if ( _multiSystem )
					return _currentSystemAtomIdxMap.at( p_currentSystemAtomIdx );
				else if ( _currentSystem->topology->atomOriginalIndexes )
					return _currentSystem->topology->atomOriginalIndexes.value()[ p_currentSystemAtomIdx ];
				else
					return p_currentSystemAtomIdx;
			}

			inline uint newResIdx( const uint & p_currentSystemResIdx )
			{
				if ( _multiSystem )
					return _currentSystemResIdxMap[ p_currentSystemResIdx ] = ++_lastResIdx;
				else
					return _currentSystem->topology->residueOriginalIds[ p_currentSystemResIdx ];
			}
			inline const uint & getResIdx( const uint & p_currentSystemResIdx ) const
			{
				if ( _multiSystem )
					return _currentSystemResIdxMap.at( p_currentSystemResIdx );
				else
					return _currentSystem->topology->residueOriginalIds[ p_currentSystemResIdx ];
			}

			/**
			 * @brief Tells the manager that from now on, we are going to query next system's atom indexes.
			 */
			void nextSystem()
			{
				_currentSystem++;
				_currentSystemAtomIdxMap.clear();
				_currentSystemResIdxMap.clear();
			}

			inline bool multiSystem() const { return _multiSystem; }

		  private:
			std::reference_wrapper<const std::vector<WriteArgs::System>> _topologies;
			bool														 _multiSystem	= _topologies.get().size() > 1;
			std::vector<WriteArgs::System>::const_iterator				 _currentSystem = _topologies.get().begin();

			// We might need to associate topology indexes to a new one defined for the written system. Therefore,
			std::unordered_map<uint, uint> _currentSystemAtomIdxMap;
			std::unordered_map<uint, uint> _currentSystemResIdxMap;
			uint						   _lastAtomIdx = 0;
			uint						   _lastResIdx	= 0;
		};

		using WrittenAtomMap = std::unordered_map<uint, uint>;
		inline bool isResidueOfChain(
			const size_t &						p_residueIdx,
			const size_t &						p_chainIdx,
			const VTX::Core::Struct::Topology & p_mol
		) noexcept
		{
			if ( p_chainIdx < p_mol.getChainCount() - 1 )
				return p_residueIdx < p_mol.chainFirstResidues[ p_chainIdx ] + p_mol.chainResidueCounts[ p_chainIdx ];
			return p_residueIdx < p_mol.getResidueCount();
		}
		inline bool isAtomOfResidue(
			const size_t &						p_atomIdx,
			const size_t &						p_residueIdx,
			const VTX::Core::Struct::Topology & p_mol
		)
		{
			if ( p_residueIdx < p_mol.getResidueCount() - 1 )
				return p_atomIdx
					   < p_mol.residueFirstAtomIndexes[ p_residueIdx ] + p_mol.residueAtomCounts[ p_residueIdx ];
			return p_atomIdx < p_mol.getAtomCount();
		}
		void convert( const VTX::Core::ChemDB::Bond::ORDER & in, E_BOND_ORDER & out ) noexcept
		{
			switch ( in )
			{
			case VTX::Core::ChemDB::Bond::ORDER::UNKNOWN: out = E_BOND_ORDER::unknown; break;
			case VTX::Core::ChemDB::Bond::ORDER::SINGLE: out = E_BOND_ORDER::single; break;
			case VTX::Core::ChemDB::Bond::ORDER::DOUBLE: out = E_BOND_ORDER::doubl; break;
			case VTX::Core::ChemDB::Bond::ORDER::TRIPLE: out = E_BOND_ORDER::triple; break;
			case VTX::Core::ChemDB::Bond::ORDER::QUADRUPLE: out = E_BOND_ORDER::quadruple; break;
			case VTX::Core::ChemDB::Bond::ORDER::QUINTUPLET: out = E_BOND_ORDER::quintuple; break;
			case VTX::Core::ChemDB::Bond::ORDER::AROMATIC: out = E_BOND_ORDER::aromatic; break;
			default: out = E_BOND_ORDER::unknown;
			}
		}

		inline void addAtom(
			const VTX::Core::Struct::Topology & p_topology,
			const size_t &						p_atomIdx,
			System &							p_system,
			Residue &							p_residue,
			SystemIndexManager &				p_indexManager
		)
		{
			Atom w_atom = p_system.newAtom( AtomId { p_indexManager.newAtomIdx( p_atomIdx ) } );
			p_residue.add( w_atom );
			w_atom.setName( p_topology.atomNames[ p_atomIdx ] );
			auto & constSymbol
				= VTX::Core::ChemDB::Atom ::SYMBOL_STR[ static_cast<int>( p_topology.atomSymbols[ p_atomIdx ] ) ];
			w_atom.setSymbol( std::string( constSymbol.begin(), constSymbol.end() ) );
		}
		inline void addResidue(
			const VTX::Core::Struct::Topology & p_topology,
			const size_t &						p_residueIdx,
			System &							p_system,
			Chain &								p_chain,
			AtomFilter &						p_atomFilter,
			SystemIndexManager &				p_indexManager

		)
		{
			Residue w_residue = p_system.newResidue();
			p_chain.add( w_residue );
			w_residue.setResId( static_cast<int>( p_indexManager.newResIdx( p_residueIdx ) ) );
			auto & constSymbol
				= VTX::Core::ChemDB::Residue::SYMBOL_STR[ static_cast<int>( p_topology
																				.residueSymbols[ p_residueIdx ] ) ];
			std::string residueSymbol
				= p_topology.residueSymbols[ p_residueIdx ] == VTX::Core::ChemDB::Residue::SYMBOL::UNKNOWN
					  ? p_topology.residueNames[ p_residueIdx ]
					  : std::string( constSymbol.begin(), constSymbol.end() );
			w_residue.setSymbol( residueSymbol );
			w_residue.set(
				Property { .key	  = "secondary_structure",
						   .value = VTX::Core::ChemDB::SecondaryStructure::enumToPdbFormatted(
							   p_topology.residueSecondaryStructureTypes[ p_residueIdx ]
						   ) }
			);
			w_residue.set(
				Property { .key	  = "is_standard_pdb",
						   .value = VTX::Core::ChemDB::Residue::checkIfStandardFromName( residueSymbol ) }
			);
			for ( size_t it_atomIdx = p_topology.residueFirstAtomIndexes[ p_residueIdx ];
				  isAtomOfResidue( it_atomIdx, p_residueIdx, p_topology );
				  it_atomIdx++ )
			{
				if ( p_atomFilter( p_topology, it_atomIdx ) )
				{
					addAtom( p_topology, it_atomIdx, p_system, w_residue, p_indexManager );
					;
				}
			}
		}
		inline void addChain(
			const VTX::Core::Struct::Topology & p_topology,
			const size_t &						p_chainIdx,
			System &							p_system,
			AtomFilter &						p_atomFilter,
			SystemIndexManager &				p_indexManager

		)
		{
			Chain w_chain = p_system.newChain();
			w_chain.setName( p_topology.chainNames[ p_chainIdx ] );

			// When I wrote this, we didn't read the chainID from chemfiles and we don't store it anywhere. So we will
			// put the name instead for now.
			w_chain.setId( p_topology.chainNames[ p_chainIdx ] );

			for ( size_t it_residueIdx = p_topology.chainFirstResidues[ p_chainIdx ];
				  isResidueOfChain( it_residueIdx, p_chainIdx, p_topology );
				  it_residueIdx++ )
			{
				addResidue( p_topology, it_residueIdx, p_system, w_chain, p_atomFilter, p_indexManager );
			}
		}
		inline void setBonds(
			const VTX::Core::Struct::Topology & p_mol,
			System &							p_system,
			AtomFilter &						p_atomFilter,
			const SystemIndexManager &			p_indexManager
		)
		{
			for ( size_t bondIdx = 0; bondIdx < p_mol.getBondCount(); bondIdx += 2 )
			{
				size_t atomIdx1 = p_mol.bondPairAtomIndexes[ bondIdx ],
					   atomIdx2 = p_mol.bondPairAtomIndexes[ bondIdx + 1 ];
				if ( p_atomFilter( p_mol, atomIdx1 ) && p_atomFilter( p_mol, atomIdx2 ) )
				{
					E_BOND_ORDER w_bondOrder = E_BOND_ORDER::unknown;
					convert( p_mol.bondOrders[ bondIdx >> 1 ], w_bondOrder );
					p_system.bind(
						AtomId { p_indexManager.getAtomIdx( atomIdx1 ) },
						AtomId { p_indexManager.getAtomIdx( atomIdx2 ) },
						w_bondOrder
					);
				}
			}
		}
		inline void fillFrames(
			const TrajectoryFrameGetter &  p_traj,
			const Core::Struct::Topology & p_topology,
			System &					   p_system,
			SystemIndexManager &		   p_indexManager

		)
		{
			for ( size_t frameIdx = 0; frameIdx < p_traj.frameCount(); frameIdx++ )
			{
				Frame				   w_frame				= p_system.newFrame();
				std::span<const Vec3f> currentAtomPositions = p_traj.getAtomPositions( static_cast<uint>( frameIdx ) );
				for ( size_t it_atomIdx = 0; it_atomIdx < currentAtomPositions.size(); it_atomIdx++ )
				{
					Atom w_atom;

					// if the atom doesn't exist for some reason, we skip to the next
					if ( p_system.fetch( w_atom, AtomId { p_indexManager.getAtomIdx( it_atomIdx ) } ) )
					{
						const VTX::Vec3f & coords = currentAtomPositions[ it_atomIdx ];
						w_frame.set( w_atom, AtomCoordinates { .x = coords[ 0 ], .y = coords[ 1 ], .z = coords[ 2 ] } );
					}
				}
			}
		}

		void writeTrajectoryFile( WriteArgs p_args )
		{
			ChemfilesTrajectory writer;
			System				w_system = writer.system();
			SystemIndexManager	indexManager( p_args.topologies );

			for ( auto & system : p_args.topologies )
			{
				auto & topology = system.topology;
				if ( topology == nullptr )
					continue;
				uint atomWritten = 0;
				for ( size_t chainIdx = 0; chainIdx < system.topology->getChainCount(); chainIdx++ )
				{
					addChain( *system.topology, chainIdx, w_system, system.atomFilter, indexManager );
				}

				if ( p_args.stopToken.stop_requested() )
					return;

				// TODO : This algorithm doesn't work for bonds and positions.
				// When an atom is filtered out, there is a shift between the position index that contains all atoms,
				// and the atomIdx from the system being written. Maybe we need to hold two counter while looping : the
				// unfiltered index and the filtered atom index.
				setBonds( *system.topology, w_system, system.atomFilter, indexManager );

				if ( p_args.stopToken.stop_requested() )
					return;

				fillFrames( system.trajectory, *topology, w_system, indexManager );
				indexManager.nextSystem();
			}

			// We fill the write destination at the very end so if we stopped due to the stoptoken, nothing get written
			writer.setWriteDestination( std::move( p_args.destination ) );
			writer.setWriteFormat( std::move( p_args.format ) );
		}
	} // namespace

	void writeFile( WriteArgs p_args )
	{
		if ( p_args.stopToken.stop_requested() )
			return;
		if ( p_args.writeType != E_WRITE_TYPE::trajectory )
			throw VTXException( "Other type of writings aren't implemented yet" );
		if ( p_args.topologies.empty() )
			throw VTXException( "Nothing to write." );
		writeTrajectoryFile( std::move( p_args ) );
	}
} // namespace VTX::IO::Writer
