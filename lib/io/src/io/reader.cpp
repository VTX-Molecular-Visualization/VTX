// Needed for io/reader.hpp
#include <core/struct/system.hpp>
#include <util/filesystem.hpp>
#include <util/thread.hpp>
#include <util/types.hpp>
// !Needed for io/reader.hpp
#include "io/reader.hpp"
//
#include <optional>

namespace VTX::IO::Reader
{
	struct SystemReader::_Impl
	{
		std::optional<MemoryBuffer>				buffer;
		FilePath								filePath;
		std::reference_wrapper<Util::StopToken> stopToken;

		_Impl( MemoryBuffer p_buffer, const FilePath & p_path, Util::StopToken & p_stopToken ) :
			buffer( std::move( p_buffer ) ), filePath( p_path ), stopToken( p_stopToken )
		{
		}
		_Impl( const FilePath & p_path, Util::StopToken & p_stopToken ) : filePath( p_path ), stopToken( p_stopToken )
		{
		}

		void   get( Core::Struct ::System & ) noexcept {}
		void   get( const FrameIndex &, AtomPositions & ) noexcept {}
		size_t frameCount() const { return 1; }
	};

	SystemReader::SystemReader( const FilePath & p_path, Util::StopToken & p_stopToken ) :
		_impl( new _Impl( p_path, p_stopToken ) )
	{
	}
	SystemReader::SystemReader( MemoryBuffer p_buffer, const FilePath & p_path, Util::StopToken & p_stopToken ) :
		_impl( new _Impl( std::move( p_buffer ), p_path, p_stopToken ) )
	{
	}

	void   SystemReader::get( Core::Struct::System & p_ ) noexcept { _impl->get( p_ ); }
	void   SystemReader::get( const FrameIndex & p_i, AtomPositions & p_ ) noexcept { _impl->get( p_i, p_ ); }
	void   SystemReader::get( AtomPositions & p_ ) noexcept { _impl->get( 0, p_ ); }
	size_t SystemReader::frameCount() const { return _impl->frameCount(); }

} // namespace VTX::IO::Reader
