#include <core/chemdb/secondary_structure.hpp>
#include <io/util/secondary_structure.hpp>
#include <iostream>
#include <thread>
//
#include <fmt/format.h>
#include <io/reader/system.hpp>
#include <vtx/secondary_structure/files.hpp>
#include <vtx/secondary_structure/parse.hpp>
#include <vtx/secondary_structure/tests.hpp>

namespace pdb100
{

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
		inline std::string writeSsReportString(
			const VTX::Core::Struct::System &					p_chemSystem,
			const VTX::Core::ChemDB::SecondaryStructure::TYPE & p_type,
			const bool &										p_isBeginCorrect,
			const bool &										p_isEndCorrect,
			const uint64_t &									p_startIdx,
			const uint64_t &									p_endIdx
		)
		{
			static const auto isCorrect = []( bool _ ) -> const char *
			{
				if ( _ )
					return "correct";
				return "incorrect";
			};
			return fmt::format(
				"Predicted SS : {}\n\tFrom {}-{} to {}-{}\n\tBegin is {}\n\tEnd is {}",
				string( p_type ),
				chainName( p_chemSystem, p_startIdx ),
				p_chemSystem.residueOriginalIds[ p_startIdx ],
				chainName( p_chemSystem, p_endIdx ),
				p_chemSystem.residueOriginalIds[ p_endIdx ],
				isCorrect( p_isBeginCorrect ),
				isCorrect( p_isEndCorrect )
			);
		}

		struct TestContext
		{
			std::string report;
			bool		isEverythingCorrect = true;
			bool		inStrand			= false;
			bool		inHelix				= false;
			bool		beginSsCorrect		= false;

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

		/**
		 * @brief Do the SS specific action
		 * @return wether or not the loop shall continue
		 */
		template<typename SS>
		inline bool specificSsComparison(
			const VTX::Core::Struct::System &			  p_chemSystem,
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
				ssTypeBegin<SS>( p_context ) = false;
				bool endSsCorrect			 = isSsBorder( p_ssCollection, p_context.currentIdx, &endNum );
				if ( not endSsCorrect )
					p_context.isEverythingCorrect = false;
				p_context.report += writeSsReportString(
					p_chemSystem,
					ssType<SS>(),
					p_context.beginSsCorrect,
					endSsCorrect,
					p_context.beginSsIdx,
					p_context.currentIdx
				);
				return true;
			}
			if ( not ssTypeBegin<SS>( p_context ) and ssType<SS>() == p_currentType ) // Begining the SS
			{
				p_context.beginSsIdx			  = p_context.currentIdx;
				otherSsTypeBegin<SS>( p_context ) = false;
				ssTypeBegin<SS>( p_context )	  = true;
				p_context.beginSsCorrect		  = isSsBorder( p_ssCollection, p_context.currentIdx, &beginNum );
				if ( not p_context.beginSsCorrect )
					p_context.isEverythingCorrect = false;

				return true;
			}

			return false;
		}

		std::string writeSs( SecondaryStruct & p_ss )
		{
			std::string ret = fmt::format(
				"\t\tBegin : {}-{}\n\t\tEnd : {}-{}",
				p_ss.begin.chain_name,
				p_ss.begin.num,
				p_ss.end.chain_name,
				p_ss.end.num
			);
			return ret;
		}
		std::string writeRcsbSs( System & p_system )
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
			}

			if ( not p_system.strands.empty() )
			{
				ret += "\tBeta-sheets : \n";
				for ( auto & ss : p_system.strands )
					ret += writeSs( ss.ss );
			}
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

				if ( specificSsComparison( p_system.system, type, p_system.helixes, context ) )
					continue;

				if ( specificSsComparison( p_system.system, type, p_system.strands, context ) )
					continue;
			}
			Reporter::Item::ResultSummary summary = Reporter::Item::ResultSummary::success;
			if ( not context.isEverythingCorrect )
			{
				summary		   = Reporter::Item::ResultSummary::fail;
				context.report = std::string( "" ) + context.report;
			}

			reporter().open()->add(
				Reporter::Item { .resultSummary = std::move( summary ),
								 .pdb			= std::string( p_system.code, sizeof( p_system.code ) ),
								 .details		= std::move( context.report ) }
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
			reporter().open()->add(
				Reporter::Item { .resultSummary = Reporter::Item::ResultSummary::no_ss,
								 .pdb			= std::string( p_system.code, sizeof( p_system.code ) ) }
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
	void testSystem( const fs::path & p_systemPath, SystemMap & p_systemMap )
	{
		std::string systemName = p_systemPath.stem().string();
		systemName			   = std::string( systemName.data(), systemName.find( '.' ) );

		uint32_t idx = *reinterpret_cast<uint32_t *>( systemName.data() );
		if ( systemName.size() > 4 )
			throw std::exception( std::format( "System name <{}> larger than 4 char", systemName ).c_str() );
		if ( p_systemMap.contains( idx ) )
			throw std::exception( std::format( "System <{}> already in map", systemName ).c_str() );
		p_systemMap.emplace( idx, System {} );

		System & newSystem = p_systemMap.at( idx );
		memcpy_s( newSystem.code, sizeof( newSystem.code ), systemName.data(), systemName.size() );

		testSystem( p_systemPath, newSystem );
	}
} // namespace pdb100
