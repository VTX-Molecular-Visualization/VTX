#include <archive.h>
#include <archive_entry.h>
#include <array>
#include <core/chemdb/secondary_structure.hpp>
#include <filesystem>
#include <fstream>
#include <io/util/secondary_structure.hpp>
#include <iostream>
#include <thread>
//
#include <fmt/format.h>
#include <io/reader/system.hpp>
//
#include <vtx/secondary_structure/child/parse.hpp>
#include <vtx/secondary_structure/child/tests.hpp>
#include <vtx/secondary_structure/shared/interprocess.hpp>
#include <vtx/secondary_structure/shared/shared.hpp>
namespace fs = std::filesystem;

namespace pdb100
{

	/**
	 * @brief Use libarchive to decompress the directory , take the first file and write it at destination.
	 * @param src
	 * @param dest
	 */
	void decompressFile( const fs::path & src, const fs::path & dest );

	void decompressFile( const fs::path & src, const fs::path & dest )
	{
		std::vector<char> cpp_buffer;
		cpp_buffer.resize( fs::file_size( src ) );

		struct archive *	   a;
		struct archive_entry * entry;
		int					   r;

		a = archive_read_new();
		archive_read_support_filter_gzip( a );
		archive_read_support_format_raw( a );

		r = archive_read_open_file( a, src.string().c_str(), 10240 );
		if ( r != ARCHIVE_OK )
			throw VTX::VTXException( "Issue with archive <{}>. Opening error.", src.string() );
		if ( archive_read_next_header( a, &entry ) != ARCHIVE_OK )
			throw VTX::VTXException( "Issue with archive <{}>. Header readin error.", src.string() );

		size_t		  redBytes = 0;
		std::ofstream outStrm( dest );
		do
		{
			redBytes = archive_read_data( a, cpp_buffer.data(), cpp_buffer.size() );
			outStrm.write( cpp_buffer.data(), redBytes );
		} while ( redBytes != 0 );

		r = archive_read_free( a );
		if ( r != ARCHIVE_OK )
			throw VTX::VTXException( "Issue with archive <{}>. Freeing error.", src.string() );
	}
	const std::string & chainName( const VTX::Core::Struct::System & p_sys, const uint64_t & p_vtxResId )
	{
		if ( p_sys.chainNames.size() == 1 )
			return p_sys.chainNames[ 0 ];

		const std::string * ret		 = &p_sys.chainNames[ 0 ];
		uint64_t			chainIdx = 1;
		for ( ; chainIdx < p_sys.chainNames.size(); chainIdx++ )
		{
			if ( p_sys.chainFirstResidues[ chainIdx ] > p_vtxResId )
				return *ret;
			ret = &p_sys.chainNames[ chainIdx - 1 ];
		}
		return *ret;
	}
	const char * string( const VTX::Core::ChemDB::SecondaryStructure::TYPE & p_ );
	std::string	 writeSsReportString(
		 const VTX::Core::Struct::System &					 p_chemSystem,
		 const VTX::Core::ChemDB::SecondaryStructure::TYPE & p_type,
		 const bool &										 p_isBeginCorrect,
		 const bool &										 p_isEndCorrect,
		 const uint64_t &									 p_startIdx,
		 const uint64_t &									 p_endIdx
	 );
	std::string writeSs( const SecondaryStruct & p_ss );
	std::string writeRcsbSs( const System & p_system );

	std::string writeRcsbSs( const System & p_system )
	{
		std::string ret;
		ret += "RCSB PDB Secondary Structure : \n";
		if ( p_system.strands.empty() )
			ret += "\tNo Beta-sheet\n";
		if ( p_system.helixes.empty() )
			ret += "\tNo right Alpha-helix\n";

		if ( p_system.helixes.empty() or p_system.strands.empty() )
			ret += "\n";

		if ( not p_system.helixes.empty() )
		{
			ret += "\tHelixes : \n";
			for ( auto & ss : p_system.helixes )
				ret += writeSs( ss.ss );
			ret += "\n";
		}

		if ( not p_system.strands.empty() )
		{
			ret += "\tBeta-sheets : \n";
			for ( auto & ss : p_system.strands )
				ret += writeSs( ss.ss );
			ret += "\n";
		}
		return ret;
	}
	std::string writeSs( const SecondaryStruct & p_ss )
	{
		std::string ret = fmt::format(
			"\t\tBegin : {}-{}\n\t\tEnd : {}-{}\n",
			p_ss.begin.chain_name,
			p_ss.begin.num,
			p_ss.end.chain_name,
			p_ss.end.num
		);
		return ret;
	}

	const char * string( const VTX::Core::ChemDB::SecondaryStructure::TYPE & p_ )
	{
		using Type = VTX::Core::ChemDB::SecondaryStructure::TYPE;

		switch ( p_ )
		{
		case Type::STRAND: return "Beta-sheet";
		case Type::HELIX_ALPHA_RIGHT: return "Alpha-helix right";
		default: return "other";
		}
	}
	std::string writeSsReportString(
		const VTX::Core::Struct::System &					p_chemSystem,
		const VTX::Core::ChemDB::SecondaryStructure::TYPE & p_type,
		const bool &										p_isBeginCorrect,
		const bool &										p_isEndCorrect,
		const uint64_t &									p_startIdx,
		const uint64_t &									p_endIdx
	)
	{
		static const auto correctnessStr = []( bool _ ) -> const char *
		{
			if ( _ )
				return "correct";
			return "incorrect";
		};
		return fmt::format(
			"Predicted SS : {}\n\tFrom {}-{} to {}-{}\n\tBegin is {}\n\tEnd is {}\n",
			pdb100::string( p_type ),
			chainName( p_chemSystem, p_startIdx ),
			p_chemSystem.residueOriginalIds[ p_startIdx ],
			chainName( p_chemSystem, p_endIdx ),
			p_chemSystem.residueOriginalIds[ p_endIdx ],
			correctnessStr( p_isBeginCorrect ),
			correctnessStr( p_isEndCorrect )
		);
	}
	namespace
	{

		const uint32_t & endNum( const SecondaryStruct & p_ss ) { return p_ss.end.num; }
		const uint32_t & beginNum( const SecondaryStruct & p_ss ) { return p_ss.begin.num; }

		template<typename SS>
		bool isSsBorder(
			const std::vector<SS> & p_ssCollection,
			const uint64_t			idx,
			const uint32_t & ( *p_fn )(const SecondaryStruct &)
		)
		{
			for ( auto & ss : p_ssCollection )
			{
				if ( ( *p_fn )( ss.ss ) == idx )
					return true;
			}
			return false;
		}

		struct TestContext
		{
			struct SsSummary
			{
				VTX::Core::ChemDB::SecondaryStructure::TYPE type;

				bool beginCorrect = false;
				bool endCorrect	  = false;
			};
			std::vector<SsSummary> ssSumaries;
			std::string			   report;
			bool				   isEverythingCorrect = true;
			bool				   inStrand			   = false;
			bool				   inHelix			   = false;
			bool				   beginSsCorrect	   = false;

			uint64_t beginSsIdx = 0;
			uint64_t currentIdx = 0;
		};

		template<typename SS>
		inline VTX::Core::ChemDB::SecondaryStructure::TYPE ssType()
		{
			static_assert( false );
		}
		template<>
		inline VTX::Core::ChemDB::SecondaryStructure::TYPE ssType<Strand>()
		{
			return VTX::Core::ChemDB::SecondaryStructure::TYPE::STRAND;
		}
		template<>
		inline VTX::Core::ChemDB::SecondaryStructure::TYPE ssType<Helix>()
		{
			return VTX::Core::ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT;
		}

		template<typename SS>
		inline bool & ssTypeBegin( TestContext & p_ )
		{
			static_assert( false );
		}
		template<>
		inline bool & ssTypeBegin<Strand>( TestContext & p_ )
		{
			return p_.inStrand;
		}
		template<>
		inline bool & ssTypeBegin<Helix>( TestContext & p_ )
		{
			return p_.inHelix;
		}
		inline bool & ssTypeBegin( TestContext & p_, const VTX::Core::ChemDB::SecondaryStructure::TYPE & p_type )
		{
			if ( p_type == VTX::Core::ChemDB::SecondaryStructure::TYPE::STRAND )
				return p_.inStrand;
			if ( p_type == VTX::Core::ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT )
				return p_.inHelix;
			return p_.inHelix;
		}

		template<typename SS>
		inline bool & otherSsTypeBegin( TestContext & p_ )
		{
			static_assert( false );
		}
		template<>
		inline bool & otherSsTypeBegin<Strand>( TestContext & p_ )
		{
			return p_.inHelix;
		}
		template<>
		inline bool & otherSsTypeBegin<Helix>( TestContext & p_ )
		{
			return p_.inStrand;
		}

		template<typename SS>
		inline const std::vector<SS> & ssCollection( const System & p_system )
		{
			static_assert( false );
		}
		template<>
		inline const std::vector<Strand> & ssCollection<Strand>( const System & p_system )
		{
			return p_system.strands;
		}
		template<>
		inline const std::vector<Helix> & ssCollection<Helix>( const System & p_system )
		{
			return p_system.helixes;
		}

		template<typename SS>
		void terminateSs(
			const VTX::Core::ChemDB::SecondaryStructure::TYPE & type,
			const System &										p_system,
			TestContext &										p_context
		)
		{
			ssTypeBegin( p_context, type ) = false;
			bool endSsCorrect			   = isSsBorder(
				 ssCollection<SS>( p_system ), p_system.system.residueOriginalIds[ p_context.currentIdx ], &endNum
			 );
			if ( not endSsCorrect )
				p_context.isEverythingCorrect = false;
			p_context.report += writeSsReportString(
				p_system.system,
				type,
				p_context.beginSsCorrect,
				endSsCorrect,
				p_context.beginSsIdx,
				p_context.currentIdx
			);
			p_context.ssSumaries.push_back(
				TestContext::SsSummary {
					.type = type, .beginCorrect = p_context.beginSsCorrect, .endCorrect = endSsCorrect }
			);
		}

		/**
		 * @brief Do the SS specific action
		 * @return wether or not the loop shall continue
		 */
		template<typename SS, typename OtherSS>
		inline bool specificSsComparison(
			const System &								  p_system,
			VTX::Core::ChemDB::SecondaryStructure::TYPE & p_currentType,
			const std::vector<SS> &						  p_ssCollection,
			TestContext &								  p_context
		)
		{
			using Type = VTX::Core::ChemDB::SecondaryStructure::TYPE;
			if ( ssTypeBegin<SS>( p_context ) and ssType<SS>() == p_currentType ) // Still in the SS
				return true;

			if ( ssTypeBegin<SS>( p_context ) and ssType<SS>() != p_currentType ) // Ending the SS
			{
				terminateSs<SS>( ssType<SS>(), p_system, p_context );
				return false;
			}
			if ( not ssTypeBegin<SS>( p_context ) and ssType<SS>() == p_currentType ) // Begining the SS
			{
				if ( otherSsTypeBegin<SS>( p_context ) )
					terminateSs<OtherSS>( ssType<OtherSS>(), p_system, p_context );

				p_context.beginSsIdx			  = p_context.currentIdx;
				otherSsTypeBegin<SS>( p_context ) = false;
				ssTypeBegin<SS>( p_context )	  = true;
				p_context.beginSsCorrect		  = isSsBorder(
					 p_ssCollection, p_system.system.residueOriginalIds[ p_context.currentIdx ], &beginNum
				 );
				if ( not p_context.beginSsCorrect )
					p_context.isEverythingCorrect = false;

				return true;
			}

			return false;
		}
		void computeCorrectnessRates( const TestContext & p_context, ReportItem<std::string>::Rates & p_rates )
		{
			for ( auto & it : p_context.ssSumaries )
			{
				if ( it.type == Type::HELIX_ALPHA_RIGHT )
				{
					p_rates.beginAlphaHelix += static_cast<float>( static_cast<int>( it.beginCorrect ) );
					p_rates.endAlphaHelix += static_cast<float>( static_cast<int>( it.endCorrect ) );
					p_rates.fullAlphaHelix
						+= static_cast<float>( static_cast<int>( it.beginCorrect and it.endCorrect ) );
					p_rates.numAlphaHelix++;
				}
				else if ( it.type == Type::STRAND )
				{
					p_rates.beginBetaSheet += static_cast<float>( static_cast<int>( it.beginCorrect ) );
					p_rates.endBetaSheet += static_cast<float>( static_cast<int>( it.endCorrect ) );
					p_rates.fullBetaSheet
						+= static_cast<float>( static_cast<int>( it.beginCorrect and it.endCorrect ) );
					p_rates.numBetaSheet++;
				}
			}
			p_rates.beginAlphaHelix /= oneIfZero( p_rates.numAlphaHelix );
			p_rates.endAlphaHelix /= oneIfZero( p_rates.numAlphaHelix );
			p_rates.fullAlphaHelix /= oneIfZero( p_rates.numAlphaHelix );
			p_rates.beginBetaSheet /= oneIfZero( p_rates.numBetaSheet );
			p_rates.endBetaSheet /= oneIfZero( p_rates.numBetaSheet );
			p_rates.fullBetaSheet /= oneIfZero( p_rates.numBetaSheet );
		}
		void convert( const ReportItem<std::string> & p_in, ReportItem<String> & p_out )
		{
			p_out.correctnessRates = p_in.correctnessRates;
			p_out.resultSummary	   = p_in.resultSummary;
			p_out.details.assign( p_out.details.begin(), p_out.details.end() );
			p_out.pdb.assign( p_out.pdb.begin(), p_out.pdb.end() );
		}

		/**
		 * @brief Report results in place of an existing entry
		 * @param p_index index of the entry
		 * @param p_item
		 */
		void postReportItem( const size_t & p_index, ReportItem<std::string> p_item )
		{
			boost::interprocess::named_mutex mutex( open_or_create, shm::rsltMap::MUTEX );
			boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock( mutex );
			boost::interprocess::managed_shared_memory						   sharedSegment(
				boost::interprocess::open_only, pdb100::shm::rsltMap::SEGNAME
			);

			void_allocator alloc( sharedSegment.get_segment_manager() );
			auto		   rsltMapAndInt = sharedSegment.find<ReportItemCollection>( pdb100::shm::rsltMap::OBJNAME );

			ReportItem<String> item { .pdb = String( alloc ), .details = String( alloc ) };
			convert( p_item, item );
			rsltMapAndInt.first->at( p_index ) = std::move( item );
		}

		/**
		 * @brief Put a crashed report in the shared memory.
		 * @return Index of the crashed report
		 */
		size_t postCrashItem( std::string p_pdbCode )
		{
			boost::interprocess::named_mutex mutex( open_or_create, shm::rsltMap::MUTEX );
			boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock( mutex );
			boost::interprocess::managed_shared_memory						   sharedSegment(
				boost::interprocess::open_only, pdb100::shm::rsltMap::SEGNAME
			);
			const size_t itemSizeApproximation = sizeof( ReportItem<String> ) + 400;
			if ( sharedSegment.get_free_memory() < itemSizeApproximation )
				boost::interprocess::managed_shared_memory::grow(
					pdb100::shm::rsltMap::SEGNAME, itemSizeApproximation
				);

			void_allocator alloc( sharedSegment.get_segment_manager() );
			auto		   rsltMapAndInt = sharedSegment.find<ReportItemCollection>( pdb100::shm::rsltMap::OBJNAME );

			ReportItem<String> item { .resultSummary = ResultSummary::crashed,
									  .pdb			 = String( alloc ),
									  .details		 = String( alloc ) };
			p_pdbCode[ 4 ] = '\0';
			item.pdb.assign( p_pdbCode.begin(), p_pdbCode.end() );
			size_t ret = rsltMapAndInt.first->size();
			rsltMapAndInt.first->push_back( std::move( item ) );
			return ret;
		}

		/**
		 * @brief Compute differences between our prediction and RCSB's and post a report item
		 */
		void compare( System & p_system )
		{
			using Type = VTX::Core::ChemDB::SecondaryStructure::TYPE;
			TestContext context;
			for ( ; context.currentIdx < p_system.system.residueSecondaryStructureTypes.size(); context.currentIdx++ )
			{
				Type & type = p_system.system.residueSecondaryStructureTypes[ context.currentIdx ];

				if ( specificSsComparison<Helix, Strand>( p_system, type, p_system.helixes, context ) )
					continue;

				if ( specificSsComparison<Strand, Helix>( p_system, type, p_system.strands, context ) )
					continue;
			}
			if ( context.inHelix )
				terminateSs<Helix>( Type::HELIX_ALPHA_RIGHT, p_system, context );
			if ( context.inStrand )
				terminateSs<Strand>( Type::STRAND, p_system, context );

			ReportItem<std::string>::ResultSummary summary = ReportItem<std::string>::ResultSummary::success;
			if ( not context.isEverythingCorrect )
			{
				summary		   = ReportItem<std::string>::ResultSummary::fail;
				context.report = writeRcsbSs( p_system ) + context.report;
			}
			ReportItem<std::string>::Rates rates;
			computeCorrectnessRates( context, rates );
			postReportItem(
				p_system.resultIndex,
				ReportItem<std::string> { .resultSummary	= std::move( summary ),
										  .correctnessRates = std::move( rates ),
										  .pdb				= std::string( p_system.code, sizeof( p_system.code ) ),
										  .details			= std::move( context.report ) }
			);
		}
	} // namespace

	void testSystem( const fs::path & p_systemPath, System & p_system )
	{
		std::stringstream strm;
		strm << "vtx_file_" << std::this_thread::get_id() << ".cif";
		fs::path decompressedFile = fs::temp_directory_path() / strm.str();
		if ( fs::exists( decompressedFile ) )
			fs::remove( decompressedFile );
		decompressFile( p_systemPath, decompressedFile );

		parse( decompressedFile, p_system.strands );
		parse( decompressedFile, p_system.helixes );

		if ( p_system.strands.empty() and p_system.helixes.empty() )
		{
			postReportItem(
				p_system.resultIndex,
				ReportItem<std::string> { .resultSummary = ReportItem<std::string>::ResultSummary::no_ss,
										  .pdb			 = std::string( p_system.code, sizeof( p_system.code ) ) }
			);
			return;
		}

		VTX::IO::Reader::System reader;

		reader.readFile( decompressedFile, p_system.system );
		size_t resIdx = 0;
		for ( auto & resSsType : p_system.system.residueSecondaryStructureTypes )
		{
			if ( resSsType != VTX::Core::ChemDB::SecondaryStructure::TYPE::UNKNOWN )
				throw std::runtime_error(
					fmt::format(
						"Chemfile System <{}> has already secondary structure informations for residue <{}>",
						p_system.code,
						resIdx
					)
				);
			resIdx++;
		}

		VTX::IO::Util::SecondaryStructure::computeStride( p_system.system );
		compare( p_system );
	}
	void testSystem( const fs::path & p_systemPath )
	{
		std::string systemName = p_systemPath.stem().string();
		try
		{
			System newSystem;
			memcpy_s( newSystem.code, sizeof( newSystem.code ), systemName.data(), sizeof( newSystem.code ) );
			newSystem.resultIndex = postCrashItem( newSystem.code );
			testSystem( p_systemPath, newSystem );
		}
		catch ( std::exception & e )
		{
			std::cout << "Structure <" << systemName << "> throws because <" << e.what() << ">\n";
		}
	}
	/**
	 * @brief Return A path pointing toward a system archive, or empty string if there is none left.
	 * @return
	 */
	std::string fetchSystemArchivePath()
	{
		boost::interprocess::named_mutex mutex( open_or_create, shm::filestrDeque::MUTEX );
		boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock( mutex );
		boost::interprocess::managed_shared_memory						   sharedSegment(
			boost::interprocess::open_only, pdb100::shm::filestrDeque::SEGNAME
		);
		auto fileStrDeque = sharedSegment.find<StringDeque>( pdb100::shm::filestrDeque::OBJNAME );
		log() << "Size of Deque : <" << fileStrDeque.first->size() << ">\n";
		if ( fileStrDeque.first->empty() )
			return {};
		std::string ret( fileStrDeque.first->back().begin(), fileStrDeque.first->back().end() );
		fileStrDeque.first->pop_back();
		return ret;
	}
	void testSystems()
	{
		uint64_t testedSystemNumber = 0;
		while ( true )
		{
			std::string systemToTest = fetchSystemArchivePath();
			continue; // Solving shm issue for now.
			if ( systemToTest.empty() )
				break;
			testSystem( systemToTest );
			testedSystemNumber++;
		}
	}

} // namespace pdb100
