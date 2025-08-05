#ifndef __VTX_APP_CORE_LIBRARY_LIBRARY_SYSTEM__
#define __VTX_APP_CORE_LIBRARY_LIBRARY_SYSTEM__

#include "library.hpp"
#include <util/collection.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::Library
{
	class LibrarySystem
	{
	  public:
		template<typename T>
		void load( const FilePath & p_path )
		{
			assert( not _libraries.has<Library<T>>() );
			_libraries.create<Library<T>>( p_path );
		}

		template<typename T>
		inline Library<T> * const getLibrary()
		{
			assert( _libraries.has<Library<T>>() );
			return _libraries.get<Library<T>>();
		}

	  private:
		Util::Collection<std::unique_ptr<ILibrary>> _libraries;
	};

} // namespace VTX::App::Core::Library

namespace VTX::App
{
	inline Core::Library::LibrarySystem & LIBRARY_SYSTEM()
	{
		return Util::Singleton<Core::Library::LibrarySystem>::get();
	}
} // namespace VTX::App
#endif
