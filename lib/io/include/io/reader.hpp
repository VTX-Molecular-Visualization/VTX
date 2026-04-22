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
	struct Topology;
} // namespace VTX::Core::Struct
namespace VTX::Core::ChemDB::Category
{
	struct Dictionary;
}

namespace VTX::IO
{
	using MemoryBuffer	= std::string;
	using AtomPositions = std::vector<Vec3f>;
	using FrameIndex	= size_t;

	constexpr std::string_view PDB_ID_CODE_DEFAULT = "----";

	struct Metadata
	{
		mutable std::string * pdbCode;
		mutable std::string * name;
		mutable bool *		  isSecondaryStructureLoadedFromFile;
		mutable bool *		  isTopologyDegenerated;
	};

	class SystemReader
	{
	  public:
		SystemReader() = delete;
		SystemReader( const VTX::FilePath &, Util::StopToken & );
		SystemReader( MemoryBuffer, const VTX::FilePath &, Util::StopToken & );

		size_t frameCount() const;

		void get( const Core::ChemDB::Category::Dictionary &, VTX::Core::Struct ::Topology & ) noexcept;

		/**
		 * @brief Always return Frame 0's positions
		 * @param
		 */
		void get( AtomPositions & ) noexcept;
		void get( const FrameIndex &, AtomPositions & ) noexcept;
		void get( const Metadata & ) noexcept;
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
