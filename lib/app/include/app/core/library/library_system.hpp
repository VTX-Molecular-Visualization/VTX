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
		template<typename P>
		inline Library<P> * load( const FilePath & p_path )
		{
			assert( not _libraries.has<Library<P>>() );
			_libraries.create<Library<P>>( p_path );
			return getLibrary<P>();
		}

		template<typename P>
		inline Library<P> * const getLibrary()
		{
			assert( _libraries.has<Library<P>>() );
			return _libraries.get<Library<P>>();
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
