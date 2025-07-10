#pragma once
#include <core/struct/system.hpp>
#include <filesystem>
#include <unordered_map>
#include <util/datalocker.hpp>
#include <vector>
namespace fs = std::filesystem;

namespace pdb100
{
	inline const fs::path g_pdb100DirectoryPath { PDB100_DATABASE_DIR };

	/**
	 * @brief Class responsible for generating a report at the application teardown
	 */
	class Reporter
	{
	  public:
		Reporter() = default;
		~Reporter();
		Reporter( const Reporter & )			 = delete;
		Reporter & operator=( const Reporter & ) = delete;
		Reporter( Reporter && )					 = delete;
		Reporter & operator=( Reporter && )		 = delete;

	  private:
		// std::stringstream _out;
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

	struct Atom
	{
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
		char  name[ 4 ] { '\0' };
		char  symbol[ 2 ] { '\0' };
	};
	struct Residue
	{
		char	 res_3letterCode[ 3 ] { '\0' };
		uint32_t num = 0;
		char	 chain_name[ 3 ] { '\0' };
	};
	struct SecondaryStruct
	{
		Residue begin;
		Residue end;
	};
	struct Helix
	{
		SecondaryStruct ss;
	};
	struct Strand
	{
		SecondaryStruct ss;
	};
	struct System
	{
		char					  code[ 4 ] { '\0' };
		VTX::Core::Struct::System system;
		// std::vector<Atom>	atoms;
		std::vector<Helix>	helixes;
		std::vector<Strand> strands;
	};
	using SystemMap		 = std::unordered_map<uint32_t, System>;
	using FileCollection = std::list<std::string>;

	struct Context
	{
		fs::path						dbDir = g_pdb100DirectoryPath;
		VTX::Util::DataLocker<Reporter> log;
		FileCollection					pdb100_system;
		SystemMap						results;
	};
} // namespace pdb100
