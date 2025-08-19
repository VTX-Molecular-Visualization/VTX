#pragma once

#include <core/chemdb/secondary_structure.hpp>
#include <core/struct/system.hpp>
#include <filesystem>
#include <vector>
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
		size_t					  resultIndex			  = 0xffffffffffffffff;
		float					  computeStridesElapsedMs = 0.f;
		VTX::Core::Struct::System system;
		std::vector<Helix>		  helixes;
		std::vector<Strand>		  strands;
	};
	struct Rates
	{
		float	 beginBetaSheet			 = 0.f;
		float	 endBetaSheet			 = 0.f;
		float	 fullBetaSheet			 = 0.f;
		uint16_t numBetaSheet			 = 0;
		float	 beginAlphaHelix		 = 0.f;
		float	 endAlphaHelix			 = 0.f;
		float	 fullAlphaHelix			 = 0.f;
		uint16_t numAlphaHelix			 = 0;
		float	 computeStridesElapsedMs = 0.f;
		uint32_t numResidues			 = 0;
	};
	enum class ResultSummary
	{
		none,	 // no results yet
		success, // secondary structure match perfectly with file's data
		fail,	 // difference have been found between prediction and file's data
		no_ss,	 // no secondary structure in file's data
		crashed, // Structure reading failed miserably
	};
	template<typename StringType>
	struct ReportItem
	{
		using Rates			= pdb100::Rates;
		using ResultSummary = pdb100::ResultSummary;
		ResultSummary resultSummary;
		Rates		  correctnessRates;
		StringType	  pdb;
		StringType	  details;
	};
	template<std::integral INT>
	inline INT oneIfZero( const INT & i )
	{
		return i == 0 ? 1 : i;
	}

} // namespace pdb100
