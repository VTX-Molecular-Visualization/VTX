#ifndef __VTX_IO_READER__
#define __VTX_IO_READER__

#include "io/constants.hpp"
#include "io/metadata.hpp"
#include <core/struct/trajectory.hpp>
#include <memory>
#include <string>
#include <util/math/aabb.hpp>
#include <util/math/grid.hpp>
#include <util/thread/stop_token.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{
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
		SystemReader( const FilePath &, const READER_OPTION, Util::Thread::StopToken & );
		SystemReader( MemoryBuffer &&, const VTX::FilePath &, const READER_OPTION, Util::Thread::StopToken & );

		size_t frameCount() const;

		/**
		 * @brief Read topology and metadata.
		 */
		void get( const VTX::Core::ChemDB::Category::Dictionary &, VTX::Core::Struct::Topology &, Metadata & );
		void get(
			const VTX::Core::ChemDB::Category::Dictionary &,
			VTX::Core::Struct::Topology &,
			Metadata &,
			VTX::Util::Math::AABB &,
			VTX::Util::Math::Grid<Index> &
		);

		/**
		 * @brief Read position-related data.
		 */
		void get( VTX::Core::Struct::Frame &, const FrameIndex = 0 );

		void set( Util::Thread::StopToken & ) noexcept;

	  private:
		struct _Impl;

		struct Del
		{
			void operator()( _Impl * ) noexcept;
		};

		std::unique_ptr<_Impl, Del> _impl = nullptr;
	};

	bool isTrajectoryFileFormat( const FilePath & p_path );

} // namespace VTX::IO

#endif // !__VTX_IO_READER__
