#include <cstring>
#include <fstream>
#include <ranges>
#include <string_view>
#include <vtx/secondary_structure/parse.hpp>

namespace rviews = std::ranges::views;

namespace pdb100
{
	namespace
	{
		const std::string_view beginResidueNameColname { "beg_label_comp_id" };
		const std::string_view beginResidueChainColname { "beg_label_asym_id" };
		const std::string_view beginResidueIdColname { "beg_label_seq_id" };
		const std::string_view endResidueNameColname { "end_label_comp_id" };
		const std::string_view endResidueChainColname { "end_label_asym_id" };
		const std::string_view endResidueIdColname { "end_label_seq_id" };
		struct Colnum
		{
			uint16_t beginResidueName  = 0;
			uint16_t beginResidueId	   = 0;
			uint16_t beginResidueChain = 0;
			uint16_t endResidueName	   = 0;
			uint16_t endResidueId	   = 0;
			uint16_t endResidueChain   = 0;
		};
		struct ParserContext
		{
			Colnum	 colnums;
			bool	 inLoop				= false;
			bool	 mustCheckLoopType	= false;
			bool	 inRightLoop		= false;
			bool	 inWrongLoop		= false;
			bool	 parsingRightColumn = false;
			bool	 parsingRightData	= false;
			uint16_t currentColumnIdx	= 0;
		};
		template<typename T>
		bool rightTablePrefix( const std::string_view & line )
		{
			static_assert( false );
		}
		template<>
		bool rightTablePrefix<Strand>( const std::string_view & line )
		{
			return line.starts_with( "_struct_sheet_range" );
		}
		template<>
		bool rightTablePrefix<Helix>( const std::string_view & line )
		{
			return line.starts_with( "_struct_conf" );
		}
		template<typename T>
		void getValue( const std::string_view & line, const uint32_t & idx, T & value )
		{

			/*
			const auto value
				= line | rviews::split( ' ' )
				  | rviews::filter( []( const auto && element ) { return std::ranges::distance( element ) == 0; } );
			line.find( ' ' );
			*/
		}

	} // namespace
	void parse( const fs::path & p_systemPath, std::vector<Strand> & p_collection )
	{
		std::ifstream inFile { p_systemPath };
		char		  lineBuf[ 1024 ];

		ParserContext context;

		while ( inFile.getline( lineBuf, sizeof( lineBuf ) ) )
		{
			if ( lineBuf[ 0 ] == '#' and context.inLoop )
			{
				context.inLoop = context.inRightLoop = context.parsingRightColumn = context.parsingRightData
					= context.mustCheckLoopType									  = false;
				context.currentColumnIdx										  = 0;
				continue;
			}
			std::string_view lineView { reinterpret_cast<char *>( lineBuf ) };
			if ( lineView.starts_with( "loop_" ) )
			{
				context.inLoop			  = true;
				context.mustCheckLoopType = true;
				continue;
			}
			if ( not context.inLoop )
				continue;

			if ( context.inLoop and not context.mustCheckLoopType and not context.inRightLoop )
				continue;

			if ( context.inLoop and context.mustCheckLoopType )
			{
				context.inRightLoop		   = rightTablePrefix<Strand>( lineView );
				context.mustCheckLoopType  = false;
				context.parsingRightColumn = true;
			}
			if ( not context.inRightLoop )
				continue;

			if ( context.parsingRightColumn and not rightTablePrefix<Strand>( lineView ) )
			{
				context.parsingRightColumn = false;
				context.parsingRightData   = true;
			}

			if ( context.parsingRightData )
			{
				Residue begin, end;
				getValue( lineView, context.colnums.beginResidueChain, begin.chain_name );
				getValue( lineView, context.colnums.beginResidueId, begin.num );
				getValue( lineView, context.colnums.beginResidueName, begin.res_3letterCode );
				getValue( lineView, context.colnums.endResidueChain, end.chain_name );
				getValue( lineView, context.colnums.endResidueId, end.num );
				getValue( lineView, context.colnums.endResidueName, end.res_3letterCode );
				p_collection.emplace_back( Strand { .ss = SecondaryStruct { std::move( begin ), std::move( end ) } } );
			}
			if ( context.parsingRightColumn )
			{
				if ( lineView.find( beginResidueNameColname ) != std::string_view::npos )
					context.colnums.beginResidueName = context.currentColumnIdx;
				if ( lineView.find( beginResidueChainColname ) != std::string_view::npos )
					context.colnums.beginResidueChain = context.currentColumnIdx;
				if ( lineView.find( beginResidueIdColname ) != std::string_view::npos )
					context.colnums.beginResidueId = context.currentColumnIdx;

				if ( lineView.find( endResidueChainColname ) != std::string_view::npos )
					context.colnums.endResidueChain = context.currentColumnIdx;
				if ( lineView.find( endResidueIdColname ) != std::string_view::npos )
					context.colnums.endResidueId = context.currentColumnIdx;
				if ( lineView.find( endResidueNameColname ) != std::string_view::npos )
					context.colnums.endResidueName = context.currentColumnIdx;

				context.currentColumnIdx++;
				continue;
			}
		}
	}
	void parse( const fs::path & p_systemPath, std::vector<Helix> & p_helixes ) {}

} // namespace pdb100
