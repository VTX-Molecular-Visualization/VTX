#ifndef __VTX_IO_WRITER_SYSTEM__
#define __VTX_IO_WRITER_SYSTEM__
#include <core/struct/system.hpp>
#include <functional>
#include <io/writer/shared.hpp>
#include <util/thread.hpp>

namespace VTX::IO::Writer
{
	/**
	 * @brief Used in the context of a structural data export. Return whether the atom shall be included in the result
	 * or not.
	 */
	using AtomFilter = std::function<bool( const VTX::Core::Struct::System &, const size_t & )>;

	/**
	 * @brief Convenient function used to create the default AtomFilter callable.
	 * @return true
	 */
	inline bool		 returnTrue( const VTX::Core::Struct::System &, const size_t & ) noexcept { return true; }
	const AtomFilter g_takeAllAtoms = AtomFilter( &returnTrue ); // default argument for the write atom filter
	/**
	 * @brief Class responsible for writing structural data from a system
	 */
	struct WriteArgs
	{
		FilePath							destination;
		E_FILE_FORMATS						format	   = E_FILE_FORMATS::none;
		const VTX::Core::Struct::System * molecule   = nullptr;
		AtomFilter							atomFilter = g_takeAllAtoms;
		E_WRITE_TYPE writeType = E_WRITE_TYPE::trajectory; // Placeholder because at some point we will probably need to
														   // write docking results and stuff
		Util::StopToken stopToken;
	};

	void writeFile( WriteArgs );

} // namespace VTX::IO::Writer

#endif
