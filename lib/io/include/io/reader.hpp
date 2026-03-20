#ifndef __VTX_IO_READER__
#define __VTX_IO_READER__

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
	struct System;
} // namespace VTX::Core::Struct

namespace VTX::IO
{
	using MemoryBuffer	= std::string;
	using AtomPositions = std::vector<Vec3f>;
	using FrameIndex	= size_t;
	struct PdbIdCode
	{
		mutable std::string * code;
	};

	class SystemReader
	{
	  public:
		SystemReader() = delete;
		SystemReader( const VTX::FilePath &, Util::StopToken & );
		SystemReader( MemoryBuffer, const VTX::FilePath &, Util::StopToken & );

		size_t frameCount() const;

		void get( VTX::Core::Struct ::System & ) noexcept;

		/**
		 * @brief Always return Frame 0's positions
		 * @param
		 */
		void get( AtomPositions & ) noexcept;
		void get( const FrameIndex &, AtomPositions & ) noexcept;
		void get( const PdbIdCode & ) noexcept;
		void set( Util::StopToken & ) noexcept;

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
