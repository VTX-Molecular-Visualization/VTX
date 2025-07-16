#include <fstream>
#include <optional>
#include <vtx/secondary_structure/shared.hpp>
namespace pdb100
{
	namespace
	{
		VTX::Util::DataLocker<Reporter> * g_reporterPtr = nullptr;

	}
	Reporter::Reporter( fs::path p_ ) : _mustWrite( true ), _reportPath( std::move( p_ ) ) {}

	Reporter::~Reporter()
	{
		if ( not _mustWrite )
			return;
		std::ofstream outFile { _reportPath };

		outFile << "Report Summary\n";
		outFile << "\tNumber of structures inspected : " << _items.size() << "\n";
		outFile << "\tNumber of structures without secondary structure data : " << _num_noSs << "\n";
		outFile << "\tNumber of failed predictions : " << _num_failed << "\n";
		outFile << "\tNumber of successful predictions : " << _num_success << "\n";
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
			outFile << it.details << "\n";
			outFile << "\n";
			outFile << "______________________\n";
			outFile << "\n";
		}
	}
	void reporter( VTX::Util::DataLocker<Reporter> & _ ) noexcept { g_reporterPtr = &_; }

	VTX::Util::DataLocker<Reporter> & reporter() noexcept { return *g_reporterPtr; }

	void Reporter::add( Item p_item )
	{
		switch ( p_item.resultSummary )
		{
		case Item::ResultSummary::success: _num_success++; break;
		case Item::ResultSummary::fail: _num_failed++; break;
		case Item::ResultSummary::no_ss: _num_noSs++; break;
		}
		_items.push_back( std::move( p_item ) );
	}

} // namespace pdb100
