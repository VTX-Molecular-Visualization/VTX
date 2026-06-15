#ifndef __VTX_CORE_CHEMDB_CATEGORY__
#define __VTX_CORE_CHEMDB_CATEGORY__

#include <array>
#include <string>
#include <unordered_set>
#include <util/types.hpp>

namespace VTX::Core::ChemDB::Category
{
	enum class TYPE : uint8_t
	{
		POLYMER,
		CARBOHYDRATE,
		LIGAND,
		ION,
		SOLVENT,
		WATER,

		UNKNOWN,
		COUNT,
	};

	constexpr std::array<std::string_view, size_t( TYPE::COUNT )> TYPE_STR
		= { "Polymer", "Carbohydrate", "Ligand", "Ion", "Solvent", "Water", "Unknown" };

	/**
	 * @brief Data structure used for resname-category association.
	 */
	struct Dictionary
	{
		std::array<std::unordered_set<std::string>, static_cast<uint>( TYPE::COUNT )> vocabularies;
		TYPE																		  fallbackAttribution
			= TYPE::LIGAND; // When a resname is not in the vocabulary, type attribution falls into the LIGAND by
							// default, which might has the largest resname diversity among all categories
	};

	/**
	 * @brief Use the dictionary to find the category of the input resname
	 * @param  Dictionary
	 * @param  Resname
	 * @param  TYPE output
	 */
	TYPE get( const Dictionary &, const std::string & );

	/**
	 * @brief Create default dictionary based on static data compiled with VTX
	 * @return
	 */
	Dictionary createDefaultDictionary();
} // namespace VTX::Core::ChemDB::Category

#endif
