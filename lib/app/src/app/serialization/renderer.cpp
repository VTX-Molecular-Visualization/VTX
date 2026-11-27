#include "app/serialization/renderer.hpp"
#include <algorithm>
#include <array>
#include <core/chemdb/atom.hpp>
#include <core/chemdb/residue.hpp>
#include <core/chemdb/secondary_structure.hpp>
#include <magic_enum/magic_enum.hpp>
#include <string>
#include <string_view>
#include <utility>
#include <util/constants.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>
#include <vector>

namespace VTX::Renderer::Color
{
	namespace
	{
		using Entries = std::vector<std::pair<std::string, ColorIndex>>;

		constexpr std::string_view KEY_UNKNOWN		 = "UNKNOWN";
		constexpr std::string_view KEY_HETATM_PREFIX = "HETATM_";

		constexpr std::string_view CATEGORY_ATOMS	 = "atoms";
		constexpr std::string_view CATEGORY_RESIDUES = "residues";
		constexpr std::string_view CATEGORY_CHAINS	 = "chains";
		constexpr std::string_view CATEGORY_RIBBONS	 = "ribbons";
		constexpr std::string_view CATEGORY_CUSTOM	 = "custom";

		template<size_t N>
		Entries _symbolEntries( const std::array<std::string_view, N> & p_symbols, const int p_offset )
		{
			Entries entries;
			entries.reserve( N );
			for ( size_t i = 0; i < N; ++i )
			{
				entries.emplace_back(
					i == 0 ? std::string( KEY_UNKNOWN ) : std::string( p_symbols[ i ] ),
					static_cast<ColorIndex>( p_offset + i )
				);
			}
			return entries;
		}

		const Entries & _atomEntries()
		{
			static const Entries entries = _symbolEntries( Core::ChemDB::Atom::SYMBOL_STR, LAYOUT_OFFSET_ATOMS );
			return entries;
		}

		const Entries & _residueEntries()
		{
			static const Entries entries = _symbolEntries( Core::ChemDB::Residue::SYMBOL_STR, LAYOUT_OFFSET_RESIDUES );
			return entries;
		}

		const Entries & _chainEntries()
		{
			static const Entries entries = []() -> Entries
			{
				constexpr int LETTER_COUNT = ( LAYOUT_COUNT_CHAINS - 1 ) / 2;

				Entries result;
				result.reserve( LAYOUT_COUNT_CHAINS );
				result.emplace_back( std::string( KEY_UNKNOWN ), static_cast<ColorIndex>( LAYOUT_OFFSET_CHAINS ) );
				for ( int i = 0; i < LETTER_COUNT; ++i )
				{
					result.emplace_back(
						std::string( 1, static_cast<char>( 'A' + i ) ),
						static_cast<ColorIndex>( LAYOUT_OFFSET_CHAINS + 1 + i )
					);
				}
				for ( int i = 0; i < LETTER_COUNT; ++i )
				{
					result.emplace_back(
						std::string( KEY_HETATM_PREFIX ) + static_cast<char>( 'A' + i ),
						static_cast<ColorIndex>( LAYOUT_OFFSET_CHAINS + 1 + LETTER_COUNT + i )
					);
				}
				return result;
			}();
			return entries;
		}

		const Entries & _ribbonEntries()
		{
			static const Entries entries = []() -> Entries
			{
				Entries result;
				result.reserve( LAYOUT_COUNT_RIBBONS );
				for ( const Core::ChemDB::SecondaryStructure::TYPE type :
					  magic_enum::enum_values<Core::ChemDB::SecondaryStructure::TYPE>() )
				{
					result.emplace_back(
						std::string( magic_enum::enum_name( type ) ),
						static_cast<ColorIndex>( LAYOUT_OFFSET_RIBBONS + static_cast<int>( type ) )
					);
				}
				return result;
			}();
			return entries;
		}

		void _write( Util::Json::Document & p_json, const std::string_view p_category, const Entries & p_entries, const Layout & p_layout )
		{
			Util::Json::Document & category = p_json[ std::string( p_category ) ];
			category						= Util::Json::Document::object();
			for ( const std::pair<std::string, ColorIndex> & entry : p_entries )
			{
				category[ entry.first ] = p_layout.colors[ entry.second ];
			}
		}

		void _read(
			const Util::Json::Document & p_json,
			const std::string_view		 p_category,
			const Entries &				 p_entries,
			Layout &					 p_layout
		)
		{
			const Util::Json::Document::const_iterator category = p_json.find( std::string( p_category ) );
			if ( category == p_json.end() )
			{
				return;
			}

			for ( Util::Json::Document::const_iterator entry = category->begin(); entry != category->end(); ++entry )
			{
				const std::string			  key	= Util::String::toUpper( entry.key() );
				const Entries::const_iterator index = std::ranges::find( p_entries, key, &Entries::value_type::first );
				if ( index == p_entries.end() )
				{
					VTX_WARNING( "Unknown color layout entry {}.{}", p_category, entry.key() );
					continue;
				}
				p_layout.colors[ index->second ] = entry.value().get<Util::Color::Rgba>();
			}
		}
	} // namespace

	void to_json( Util::Json::Document & p_json, const Layout & p_layout )
	{
		p_json = Util::Json::Document::object();
		_write( p_json, CATEGORY_ATOMS, _atomEntries(), p_layout );
		_write( p_json, CATEGORY_RESIDUES, _residueEntries(), p_layout );
		_write( p_json, CATEGORY_CHAINS, _chainEntries(), p_layout );
		_write( p_json, CATEGORY_RIBBONS, _ribbonEntries(), p_layout );

		Util::Json::Document & custom = p_json[ std::string( CATEGORY_CUSTOM ) ];
		custom						  = Util::Json::Document::array();
		for ( int i = 0; i < LAYOUT_COUNT_CUSTOM; ++i )
		{
			custom.push_back( p_layout.colors[ LAYOUT_OFFSET_CUSTOM + i ] );
		}
	}

	void from_json( const Util::Json::Document & p_json, Layout & p_layout )
	{
		p_layout.colors.fill( COLOR_WHITE );
		_read( p_json, CATEGORY_ATOMS, _atomEntries(), p_layout );
		_read( p_json, CATEGORY_RESIDUES, _residueEntries(), p_layout );
		_read( p_json, CATEGORY_CHAINS, _chainEntries(), p_layout );
		_read( p_json, CATEGORY_RIBBONS, _ribbonEntries(), p_layout );

		const Util::Json::Document::const_iterator custom = p_json.find( std::string( CATEGORY_CUSTOM ) );
		if ( custom == p_json.end() )
		{
			return;
		}

		if ( custom->size() > static_cast<size_t>( LAYOUT_COUNT_CUSTOM ) )
		{
			VTX_WARNING( "Color layout has {} custom colors, only {} are used", custom->size(), LAYOUT_COUNT_CUSTOM );
		}

		for ( size_t i = 0; i < custom->size() && i < static_cast<size_t>( LAYOUT_COUNT_CUSTOM ); ++i )
		{
			p_layout.colors[ LAYOUT_OFFSET_CUSTOM + i ] = custom->at( i ).get<Util::Color::Rgba>();
		}
	}
} // namespace VTX::Renderer::Color
