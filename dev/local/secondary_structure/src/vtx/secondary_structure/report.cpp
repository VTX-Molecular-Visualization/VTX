#include <fmt/format.h>
#include <fstream>
#include <optional>
#include <vtx/secondary_structure/report.hpp>
#include <vtx/secondary_structure/shared.hpp>

namespace pdb100
{

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
			string( p_type ),
			chainName( p_chemSystem, p_startIdx ),
			p_chemSystem.residueOriginalIds[ p_startIdx ],
			chainName( p_chemSystem, p_endIdx ),
			p_chemSystem.residueOriginalIds[ p_endIdx ],
			correctnessStr( p_isBeginCorrect ),
			correctnessStr( p_isEndCorrect )
		);
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
	Reporter::Reporter( fs::path p_ ) : _mustWrite( true ), _reportPath( std::move( p_ ) ) {}

	Reporter::~Reporter()
	{
		if ( not _mustWrite )
			return;
		_betaSheetCorrectnessRate /= oneIfZero( _items.size() );
		_alphaHelixCorrectnessRate /= oneIfZero( _items.size() );

		std::ofstream outFile { _reportPath };

		outFile << "Report Summary\n";
		outFile << "\tNumber of structures inspected : " << _items.size() << "\n";
		outFile << "\tNumber of structures without secondary structure data : " << _num_noSs << "\n";
		outFile << "\tNumber of RCSB Beta-sheet : " << _num_betaSheet << "\n";
		outFile << "\tNumber of RCSB Alpha-helix : " << _num_alphaHelix << "\n";
		outFile << "\tNumber of failed predictions : " << _num_failed << "\n";
		outFile << "\tNumber of fully successful system predictions : " << _num_success << "\n";
		outFile << "\n";
		outFile << "\tsuccess rate : "
				<< static_cast<double>(
					   static_cast<double>( _num_success ) / static_cast<double>( _num_failed + _num_success )
				   )
				<< "\n";
		outFile << "\tfail rate : "
				<< static_cast<double>(
					   static_cast<double>( _num_failed ) / static_cast<double>( _num_failed + _num_success )
				   )
				<< "\n";
		outFile << "\tno data rate : " << ( static_cast<double>( _num_noSs ) / ( _items.size() ) ) << "\n";
		outFile << "\tFully correct Beta-sheet rate mean : " << _betaSheetCorrectnessRate << "\n";
		outFile << "\tFully correct Alpha-helix rate mean : " << _alphaHelixCorrectnessRate << "\n";
		outFile << "\n";
		outFile << "______________________\n";
		outFile << "\n";
		outFile << "Details\n";
		outFile << "\n";
		outFile << "______________________\n";
		outFile << "\n";
		for ( auto & it : _items )
		{
			if ( it.resultSummary != Item::ResultSummary::fail )
				continue;
			outFile << "Structure :" << it.pdb << "\n";
			outFile << "\n";
			outFile << "Successful Beta-sheet start prediction : " << it.correctnessRates.beginBetaSheet << "\n";
			outFile << "Successful Beta-sheet end prediction : " << it.correctnessRates.endBetaSheet << "\n";
			outFile << "Successful full Beta-sheet prediction : " << it.correctnessRates.fullBetaSheet << "\n";
			outFile << "Successful Alpha-helix start prediction : " << it.correctnessRates.beginAlphaHelix << "\n";
			outFile << "Successful Alpha-helix end prediction : " << it.correctnessRates.endAlphaHelix << "\n";
			outFile << "Successful full Alpha-helix prediction : " << it.correctnessRates.fullAlphaHelix << "\n";
			outFile << "\n";
			outFile << it.details << "\n";
			outFile << "\n";
			outFile << "______________________\n";
			outFile << "\n";
		}
	}

	void Reporter::add( Item p_item )
	{
		switch ( p_item.resultSummary )
		{
		case Item::ResultSummary::success: _num_success++; break;
		case Item::ResultSummary::fail: _num_failed++; break;
		case Item::ResultSummary::no_ss: _num_noSs++; break;
		}
		_num_betaSheet += p_item.correctnessRates.numBetaSheet;
		_num_alphaHelix += p_item.correctnessRates.numAlphaHelix;
		_betaSheetCorrectnessRate += p_item.correctnessRates.fullBetaSheet;
		_alphaHelixCorrectnessRate += p_item.correctnessRates.fullAlphaHelix;

		_items.push_back( std::move( p_item ) );
	}

} // namespace pdb100
