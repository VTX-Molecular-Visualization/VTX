#ifndef __VTX_BENCH_UTIL__
#define __VTX_BENCH_UTIL__

#include <core/struct/topology.hpp>
#include <core/struct/trajectory.hpp>
#include <io/metadata.hpp>
#include <util/types.hpp>

namespace VTX::Bench
{
	struct LoadedSystem
	{
		Core::Struct::Topology topology;
		IO::Metadata		   metadata;
		Core::Struct::Frame	   positions;
	};

	LoadedSystem loadSystem( const FilePath & p_filename );
	LoadedSystem downloadSystem( const std::string & p_pdb );

} // namespace VTX::Bench

#endif
