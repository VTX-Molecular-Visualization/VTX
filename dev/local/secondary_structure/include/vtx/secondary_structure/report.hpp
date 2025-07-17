#pragma once

#include <core/struct/system.hpp>
#include <filesystem>
namespace fs = std::filesystem;

namespace pdb100
{
	using Type = VTX::Core::ChemDB::SecondaryStructure::TYPE;

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
		std::vector<Helix>		  helixes;
		std::vector<Strand>		  strands;
	};

	const std::string & chainName( const VTX::Core::Struct::System & p_sys, const uint64_t & p_vtxResId );
	const char *		string( const VTX::Core::ChemDB::SecondaryStructure::TYPE & p_ );
	std::string			writeSsReportString(
				const VTX::Core::Struct::System &					p_chemSystem,
				const VTX::Core::ChemDB::SecondaryStructure::TYPE & p_type,
				const bool &										p_isBeginCorrect,
				const bool &										p_isEndCorrect,
				const uint64_t &									p_startIdx,
				const uint64_t &									p_endIdx
			);
	std::string writeSs( const SecondaryStruct & p_ss );
	std::string writeRcsbSs( const System & p_system );

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

		struct Item
		{
			enum class ResultSummary
			{
				none,	 // no results yet
				success, // secondary structure match perfectly with file's data
				fail,	 // difference have been found between prediction and file's data
				no_ss	 // no secondary structure in file's data
			} resultSummary;
			struct Rates
			{
				float	 beginBetaSheet	 = 0.f;
				float	 endBetaSheet	 = 0.f;
				float	 fullBetaSheet	 = 0.f;
				uint16_t numBetaSheet	 = 0;
				float	 beginAlphaHelix = 0.f;
				float	 endAlphaHelix	 = 0.f;
				float	 fullAlphaHelix	 = 0.f;
				uint16_t numAlphaHelix	 = 0;
			} correctnessRates;
			std::string pdb;
			std::string details;
		};

		void add( Item );

	  private:
		bool			  _mustWrite = false;
		fs::path		  _reportPath;
		std::vector<Item> _items;
		uint32_t		  _num_success				 = 0;
		uint32_t		  _num_failed				 = 0;
		uint32_t		  _num_noSs					 = 0;
		uint32_t		  _num_betaSheet			 = 0;
		uint32_t		  _num_alphaHelix			 = 0;
		float			  _betaSheetCorrectnessRate	 = 0.f;
		float			  _alphaHelixCorrectnessRate = 0.f;
	};

} // namespace pdb100
