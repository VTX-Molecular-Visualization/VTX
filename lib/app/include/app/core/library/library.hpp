#ifndef __VTX_APP_CORE_LIBRARY_LIBRARY__
#define __VTX_APP_CORE_LIBRARY_LIBRARY__

#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Library
{
	class ILibrary
	{
	  public:
		virtual ~ILibrary() = default;
	};

	template<typename T>
	class Library : public ILibrary
	{
	  public:
		Library() = delete;
		Library( const FilePath & p_path ) : _path( p_path ) { load(); }

		const std::map<std::string, T> & getItems() { return _items; }

		T * const createItem( const std::optional<std::string_view> p_name )
		{
			std::string name = p_name.has_value() ? std::string { p_name.value() } : "new";

			while ( _items.contains( name ) )
			{
				name += "_copy";
			}

			_items.emplace( name, T() );
			return &_items[ name ];
		}

		T * const copyItem( const std::string_view p_name, const std::string_view p_newName )
		{
			assert( _items.contains( p_name ) );

			_items.emplace( p_newName, _items[ p_name ] );
			return &_items[ p_newName ];
		}

		void removeItem( const std::string_view p_name )
		{
			// TODO
		}

		void setDefault( const std::string_view p_name )
		{
			if ( _items.contains( p_name ) )
			{
				_defaultItem = &_items[ p_name ];
			}
			else
			{
				VTX_ERROR( "Default item '{}' does not exist in library.", p_name );
			}
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
						createItem( name );
					}
				}
			}

			if ( _items.size() == 0 )
			{
				createItem( "Default" );
			}

			// TODO: set default from settings.
		}

		void save()
		{
			std::filesystem::create_directories( _path );
			// TODO: loop through items and save them to the filesystem.
		}

		// TODO: callbacks.

	  private:
		const FilePath			 _path;
		std::map<std::string, T> _items;
		T *						 _defaultItem = nullptr;
	};

} // namespace VTX::App::Core::Library

#endif
