#ifndef __VTX_IO_READER__
#define __VTX_IO_READER__

#include "io/constants.hpp"
#include "io/metadata.hpp"
#include <memory>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::Util
{
	class StopToken;
} // namespace VTX::Util

namespace VTX::Core::Struct
{
	using Frame = std::vector<Vec3f>;
	struct Topology;
} // namespace VTX::Core::Struct

namespace VTX::Core::ChemDB::Category
{
	struct Dictionary;
}

namespace VTX::IO
{
	using MemoryBuffer = std::string;
	using FrameIndex   = size_t;

	class SystemReader
	{
	  public:
		SystemReader() = delete;
		SystemReader( const FilePath &, const READER_OPTION, VTX::Util::StopToken & );
		SystemReader( MemoryBuffer &&, const VTX::FilePath &, const READER_OPTION, VTX::Util::StopToken & );

		size_t frameCount() const;

		void get( const VTX::Core::ChemDB::Category::Dictionary &, VTX::Core::Struct::Topology &, Metadata & );

		/**
		 * @brief Always return Frame 0's positions.
		 */
		void get( VTX::Core::Struct::Frame & );
		void get( const FrameIndex &, VTX::Core::Struct::Frame & );
		void set( VTX::Util::StopToken & ) noexcept;

	  private:
		struct _Impl;

		struct Del
		{
			void operator()( _Impl * ) noexcept;
		};

		std::unique_ptr<_Impl, Del> _impl = nullptr;
	};

	bool isTrajectoryFileFormat( const FilePath & p_path ) noexcept;

} // namespace VTX::IO

#endif // !__VTX_IO_READER__
