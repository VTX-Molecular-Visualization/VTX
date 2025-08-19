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
		/**
		 * @brief Compute the performance metric related to VTX SS assignement algorithm execution time. The metric
		 * shall incorporate the size of the structure being analyze as there is a huge variance between individual and
		 * this shall naturally affect execution time. We do not plan to make our implementatin constant time or
		 * whatever
		 * @param p_rates
		 * @return
		 */
		double computeStrideEfficiency( const Rates & p_rates )
		{
			return 1000 * p_rates.computeStridesElapsedMs / p_rates.numResidues;
		}

		void convert( const ReportItem<String> & p_in, ReportItem<std::string> & p_out )
		{
			p_out.correctnessRates = p_in.correctnessRates;
			p_out.resultSummary	   = p_in.resultSummary;
			p_out.details.assign( p_in.details.begin(), p_in.details.end() );
			p_out.pdb.assign( p_in.pdb.begin(), p_in.pdb.end() );
		}

		/**
		 * @brief Grab each report entries and feed them into the reporter
		 * @param p_reporter
		 */
		void fetchItems( Reporter & p_reporter )
		{
			boost::interprocess::managed_shared_memory sharedSegment(
				boost::interprocess::open_only, pdb100::shm::rsltMap::SEGNAME
			);
			auto rsltMapAndInt = sharedSegment.find<ReportItemCollection>( pdb100::shm::rsltMap::OBJNAME );
			while ( not rsltMapAndInt.first->empty() )
			{
				ReportItem<std::string> item;
				convert( rsltMapAndInt.first->back(), item );
				p_reporter.add( std::move( item ) );
				rsltMapAndInt.first->pop_back();
			}
		}
	} // namespace

	Reporter::Reporter( fs::path p_ ) : _mustWrite( true ), _reportPath( std::move( p_ ) ) {}

	Reporter::~Reporter()
	{
		/* Actual main function of the reporter. Meant to be trigger on normal teardown and on some program termination
		 * so the results are not lost.
		 */
		if ( not _mustWrite )
			return;

		fetchItems( *this );

		std::cout << "Writing report ... ";
		_betaSheetCorrectnessRate /= oneIfZero( _items.size() );
		_alphaHelixCorrectnessRate /= oneIfZero( _items.size() );
		_computeStrideEfficiencyRateMean /= oneIfZero( _computeStrideEfficiencyNumStruct );
		for ( auto & it : _items )
			if ( it.correctnessRates.numResidues > 0 )
				_computeStrideEfficiencyStdDev
					+= abs( _computeStrideEfficiencyRateMean - computeStrideEfficiency( it.correctnessRates ) );
		_computeStrideEfficiencyStdDev /= oneIfZero( _computeStrideEfficiencyNumStruct );

		std::ofstream outFile { _reportPath };

		outFile << "Report Summary\n";
		outFile << "\tNumber of structures inspected : " << _items.size() << "\n";
		outFile << "\tNumber of structures that caused a crash : " << _num_crashed << "\n";
		outFile << "\tNumber of structures without secondary structure data : " << _num_noSs << "\n";
		outFile << "\tNumber of RCSB Beta-sheet : " << _num_betaSheet << "\n";
		outFile << "\tNumber of RCSB Alpha-helix : " << _num_alphaHelix << "\n";
		outFile << "\tNumber of failed predictions : " << _num_failed << "\n";
		outFile << "\tNumber of fully successful system predictions : " << _num_success << "\n";
		outFile << "\tAssignement efficiency (microseconds/residue) mean : " << _computeStrideEfficiencyRateMean
				<< "\n";
		outFile << "\tAssignement efficiency standard deviation: " << _computeStrideEfficiencyStdDev << "\n";
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
		for ( auto & it : _items )
		{
			outFile << "\n";
			outFile << "______________________\n";
			outFile << "\n";

			if ( it.resultSummary == ReportItem<std::string>::ResultSummary::crashed )
			{
				outFile << "Structure :" << it.pdb << " crashed." << "\n";
				continue;
			}
			outFile << "Structure :" << it.pdb << "\n";
			if ( it.correctnessRates.numResidues == 0 )
				outFile << "0 res found ?" << "\n";
			else
				outFile << "Computation efficiency :" << computeStrideEfficiency( it.correctnessRates ) << "\n";
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
		case ReportItem<std::string>::ResultSummary::crashed: _num_crashed++; break;
		}
		_num_betaSheet += p_item.correctnessRates.numBetaSheet;
		_num_alphaHelix += p_item.correctnessRates.numAlphaHelix;
		_betaSheetCorrectnessRate += p_item.correctnessRates.fullBetaSheet;
		_alphaHelixCorrectnessRate += p_item.correctnessRates.fullAlphaHelix;
		if ( p_item.correctnessRates.numResidues > 0 )
		{
			_computeStrideEfficiencyRateMean += computeStrideEfficiency( p_item.correctnessRates );
			_computeStrideEfficiencyNumStruct++;
		}
		_items.push_back( std::move( p_item ) );
	}

} // namespace pdb100
