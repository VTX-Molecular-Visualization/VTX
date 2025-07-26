#pragma once

#include <concepts>
#include <filesystem>
#include <stack>
#include <unordered_map>
#include <util/datalocker.hpp>
#include <vector>
#include <vtx/secondary_structure/report.hpp>

namespace fs = std::filesystem;

namespace pdb100
{
	const size_t NUM_THREADS = 16;
	const size_t NUM_PROCESSES = 16;
	template<std::integral INT>
	inline INT oneIfZero( const INT & i )
	{
		return i == 0 ? 1 : i;
	}

	inline const fs::path g_pdb100DirectoryPath { PDB100_DATABASE_DIR };

	using SystemMap		 = std::unordered_map<uint32_t, System>;
	using FileCollection = std::stack<std::string>;

	class Reporter;
	struct Context
	{
		fs::path							  dbDir = g_pdb100DirectoryPath;
		VTX::Util::DataLocker<Reporter>		  log { "report.txt" };
		VTX::Util::DataLocker<FileCollection> pdb100_system;
		SystemMap							  results;
	};

	/**
	 * @brief Set the reporter for the entire process
	 */
	void reporter( VTX::Util::DataLocker<Reporter> & ) noexcept;
	/**
	 * @brief Get current process's reporter
	 * @return
	 */
	VTX::Util::DataLocker<Reporter> & reporter() noexcept;
} // namespace pdb100
