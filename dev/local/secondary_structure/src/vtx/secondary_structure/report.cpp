#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <optional>
#include <vtx/secondary_structure/report.hpp>
#include <vtx/secondary_structure/shared.hpp>
#include <vtx/secondary_structure/shared/interprocess.hpp>

namespace pdb100
{

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
	const std::string & chainName( const VTX::Core::Struct::System & p_sys, const uint64_t & p_vtxResId );
	std::string			writeSsReportString(
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

		std::cout << "Writing report ... ";
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
			if ( it.resultSummary != ReportItem<std::string>::ResultSummary::fail )
				continue;
			outFile << "Structure :" << it.pdb << "\n";
			outFile << "\n";
			if ( it.correctnessRates.numBetaSheet > 0 )
			{
				outFile << "Successful Beta-sheet start prediction : " << it.correctnessRates.beginBetaSheet << "\n";
				outFile << "Successful Beta-sheet end prediction : " << it.correctnessRates.endBetaSheet << "\n";
				outFile << "Successful full Beta-sheet prediction : " << it.correctnessRates.fullBetaSheet << "\n";
			}
			else
			{
				outFile << "No predicted Beta-sheets.\n";
			}
			if ( it.correctnessRates.numAlphaHelix )
			{
				outFile << "Successful Alpha-helix start prediction : " << it.correctnessRates.beginAlphaHelix << "\n";
				outFile << "Successful Alpha-helix end prediction : " << it.correctnessRates.endAlphaHelix << "\n";
				outFile << "Successful full Alpha-helix prediction : " << it.correctnessRates.fullAlphaHelix << "\n";
			}
			else
			{
				outFile << "No predicted Alpha-helixes.\n";
			}
			outFile << "\n";
			outFile << it.details << "\n";
			outFile << "\n";
			outFile << "______________________\n";
			outFile << "\n";
		}
		std::cout << "done.\n";
	}

	void Reporter::add( ReportItem<std::string> p_item )
	{
		switch ( p_item.resultSummary )
		{
		case ReportItem<std::string>::ResultSummary::success: _num_success++; break;
		case ReportItem<std::string>::ResultSummary::fail: _num_failed++; break;
		case ReportItem<std::string>::ResultSummary::no_ss: _num_noSs++; break;
		}
		_num_betaSheet += p_item.correctnessRates.numBetaSheet;
		_num_alphaHelix += p_item.correctnessRates.numAlphaHelix;
		_betaSheetCorrectnessRate += p_item.correctnessRates.fullBetaSheet;
		_alphaHelixCorrectnessRate += p_item.correctnessRates.fullAlphaHelix;

		_items.push_back( std::move( p_item ) );
	}

} // namespace pdb100
