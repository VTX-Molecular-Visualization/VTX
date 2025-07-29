#pragma once

#include <core/struct/system.hpp>
#include <filesystem>
#include <vtx/secondary_structure/shared/interprocess.hpp>
namespace fs = std::filesystem;

namespace pdb100
{

	/**
	 * @brief Class responsible for generating a report at the application teardown
	 */
	class Reporter
	{
	  public:
		Reporter( fs::path );
		~Reporter();
		Reporter( const Reporter & )			 = delete;
		Reporter & operator=( const Reporter & ) = delete;
		Reporter( Reporter && )					 = default;
		Reporter & operator=( Reporter && )		 = default;

		void add( ReportItem<std::string> );

	  private:
		bool								 _mustWrite = false;
		fs::path							 _reportPath;
		std::vector<ReportItem<std::string>> _items;
		uint32_t							 _num_success				= 0;
		uint32_t							 _num_failed				= 0;
		uint32_t							 _num_noSs					= 0;
		uint32_t							 _num_betaSheet				= 0;
		uint32_t							 _num_alphaHelix			= 0;
		float								 _betaSheetCorrectnessRate	= 0.f;
		float								 _alphaHelixCorrectnessRate = 0.f;
	};

} // namespace pdb100
