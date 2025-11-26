#ifndef __VTX_APP_LIBRARY_LIBRARY__
#define __VTX_APP_LIBRARY_LIBRARY__

#include "app/library/base_preset.hpp"
#include <optional>
#include <util/callback.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::App::Library
{

	class ILibrary
	{
	  public:
		virtual ~ILibrary() = default;
	};

	template<ConceptPreset P>
	class BaseLibrary : public ILibrary
	{
	  public:
		using MapPresetsByName = std::map<std::string, std::unique_ptr<P>, std::less<>>;

		BaseLibrary() = delete;
		BaseLibrary( const FilePath & p_path ) : _path( p_path ) { load(); }

		const MapPresetsByName & getPresets() { return _presets; }

		P & getPreset( const std::string_view p_name )
		{
			assert( _presets.contains( p_name ) );
			return *_presets[ std::string { p_name } ];
		}

		P & createPreset( const std::optional<std::string_view> & p_name )
		{
			std::string name = std::string { p_name.has_value() ? p_name.value() : "New preset" };

			_makeNameUnique( name );
			_presets.emplace( name, std::make_unique<P>() );

			onPresetAdded( name );

			return *_presets[ name ];
		}

		P & copyPreset( const std::string_view p_src, const std::optional<std::string_view> & p_dest )
		{
			assert( _presets.contains( p_src ) );
			std::string name = std::string { p_dest.has_value() ? p_dest.value() : p_src };

			_makeNameUnique( name );
			_presets.emplace( name, std::make_unique<P>() );
			*_presets[ name ] = *_presets.at( std::string { p_src } );

			onPresetAdded( name );

			return *_presets[ name ];
		}

		void removePreset( const std::string_view p_name )
		{
			assert( _presets.contains( p_name ) );

			_presets.erase( std::string { p_name } );

			if ( _presets.size() == 0 )
			{
				createPreset( "Default" );
			}

			onPresetDeleted( p_name );
		}

		void renamePreset( const std::string_view p_src, const std::string_view p_dest )
		{
			if ( p_src == p_dest )
				return;

			std::string src { p_src };
			std::string dest { p_dest };

			assert( _presets.contains( src ) );

			_makeNameUnique( dest );
			_presets[ dest ] = std::move( _presets[ src ] );
			_presets.erase( src );

			onPresetRenamed( dest );
		}

		void load()
		{
			if ( std::filesystem::exists( _path ) )
			{
				for ( const auto & entry : std::filesystem::directory_iterator( _path ) )
				{
					if ( entry.is_regular_file() )
					{
						const std::string name = entry.path().stem().string();
						VTX_INFO( "Loading item: {}", name );
						createPreset( name );
					}
				}
			}

			if ( _presets.size() == 0 )
			{
				createPreset( "Default" );
			}
		}

		void save()
		{
			std::filesystem::create_directories( _path );
			// TODO: loop through items and save them to the filesystem.
		}

		Util::Callback<std::string_view> onPresetAdded;
		Util::Callback<std::string_view> onPresetRenamed;
		Util::Callback<std::string_view> onPresetDeleted;

	  private:
		const FilePath	 _path;
		MapPresetsByName _presets;

		void _makeNameUnique( std::string & p_name )
		{
			while ( _presets.contains( p_name ) )
			{
				p_name += "_copy";
			}
		}
	};

} // namespace VTX::App::Library

#endif
