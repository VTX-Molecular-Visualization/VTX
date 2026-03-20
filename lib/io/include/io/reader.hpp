#ifndef __VTX_IO_READER__
#define __VTX_IO_READER__

#include <memory>
#include <string>
#include <vector>

namespace VTX
{
	class FilePath;
	struct Vec3f;
} // namespace VTX
namespace VTX::Util
{
	class StopToken;
	class FilePath;
} // namespace VTX::Util

namespace VTX::Core::Struct
{
	struct System;
	struct Frame;
} // namespace VTX::Core::Struct

namespace VTX::IO::Reader
{
	using MemoryBuffer	= std::string;
	using AtomPositions = std::vector<Vec3f>;

	class SystemReader
	{
	  public:
		SystemReader() = delete;
		SystemReader( const FilePath &, Util::StopToken & );
		SystemReader( MemoryBuffer, const FilePath &, Util::StopToken & );

		void get( Core::Struct ::System & ) noexcept;
		void get( AtomPositions & ) noexcept;

	  private:
		struct _Impl;
		struct Del
		{
			void operator()( _Impl * ) noexcept;
		};
		std::unique_ptr<_Impl, Del> _impl = nullptr;
	};

} // namespace VTX::IO::Reader

#endif // !__VTX_IO_READER__
