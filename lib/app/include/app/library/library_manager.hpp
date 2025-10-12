#ifndef __VTX_APP_LIBRARY_LIBRARY_SYSTEM__
#define __VTX_APP_LIBRARY_LIBRARY_SYSTEM__

#include "base_library.hpp"
#include <util/collection.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Library
{
	class LibraryManager
	{
	  public:
		LibraryManager()					= default;
		LibraryManager( LibraryManager && ) = default;

		template<typename P>
		BaseLibrary<P> * const load( const FilePath & p_path )
		{
			assert( not _libraries.has<BaseLibrary<P>>() );
			_libraries.create<BaseLibrary<P>>( p_path );
			return getLibrary<P>();
		}

		template<typename P>
		BaseLibrary<P> * const getLibrary()
		{
			assert( _libraries.has<BaseLibrary<P>>() );
			return _libraries.get<BaseLibrary<P>>();
		}

	  private:
		Util::Collection<std::unique_ptr<ILibrary>> _libraries;
	};

} // namespace VTX::App::Library
#endif
