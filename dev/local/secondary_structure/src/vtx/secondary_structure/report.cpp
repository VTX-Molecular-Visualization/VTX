#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <optional>
#include <vtx/secondary_structure/report.hpp>
#include <vtx/secondary_structure/shared.hpp>
#include <vtx/secondary_structure/shared/interprocess.hpp>

namespace pdb100
{
	namespace
	{
		void convert( const ReportItem<String> & p_in, ReportItem<std::string> & p_out )
		{
			p_out.correctnessRates = p_in.correctnessRates;
			p_out.resultSummary	   = p_in.resultSummary;
			p_out.details.assign( p_out.details.begin(), p_out.details.end() );
			p_out.pdb.assign( p_out.pdb.begin(), p_out.pdb.end() );
		}
		void fetchItems( std::vector<ReportItem<std::string>> & p_in )
		{
			boost::interprocess::named_mutex		   mutex( open_or_create, shm::rsltMap::MUTEX );
			boost::interprocess::managed_shared_memory sharedSegment(
				boost::interprocess::open_only, pdb100::shm::rsltMap::SEGNAME
			);
			auto rsltMapAndInt = sharedSegment.find<ReportItemCollection>( pdb100::shm::rsltMap::OBJNAME );
			while ( not rsltMapAndInt.first->empty() )
			{
				p_in.push_back( {} );
				convert( rsltMapAndInt.first->back(), p_in.back() );
				rsltMapAndInt.first->pop_back();
			}
		}
	} // namespace

	Reporter::Reporter( fs::path p_ ) : _mustWrite( true ), _reportPath( std::move( p_ ) ) {}

	Reporter::~Reporter()
	{
		if ( not _mustWrite )
			return;

		fetchItems( _items );

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
