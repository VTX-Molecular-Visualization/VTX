#pragma once
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <type_traits>

namespace VTX::IO::test
{
	using namespace boost::interprocess;

	const char SHM_FILESTR_DEQUE_SEGNAME[]	= "VTX__SHM_FILESTR_DEQUE";
	const char SHM_FILESTR_DEQUE_OBJNAME[]	= "file_collection";
	const char SHM_FILESTR_MUTEX[]			= "VTX__SHM_FILESTR_MUTEX";
	const char SHM_REREADRSLT_MAP_SEGNAME[] = "VTX__SHM_REREADRSLT_MAP";
	const char SHM_REREADRSLT_MAP_OBJNAME[] = "result_collection";
	const char SHM_REREADRSLT_MUTEX[]		= "VTX__SHM_FILESTR_MUTEX";

	typedef allocator<char, managed_shared_memory::segment_manager>	  CharAllocator;
	typedef basic_string<char, std::char_traits<char>, CharAllocator> String;
	typedef allocator<String, managed_shared_memory::segment_manager> StringAllocator;

	typedef deque<String, std::char_traits<char>, StringAllocator>		   StringDeque;
	typedef allocator<StringDeque, managed_shared_memory::segment_manager> StringDequeAllocator;

	typedef allocator<uint64_t, managed_shared_memory::segment_manager> Uint64Allocator;

	enum class RereadResult : uint8_t
	{
		fully_working	 = 0,
		frame_mismatch	 = 1 << 0,
		chain_mismatch	 = 1 << 1,
		residue_mismatch = 1 << 2,
		atom_mismatch	 = 1 << 3,
		crashed			 = 1 << 7
	};
	typedef allocator<RereadResult, managed_shared_memory::segment_manager> RereadResultAllocator;
	typedef allocator<std::pair<uint64_t, RereadResult>, managed_shared_memory::segment_manager>
		Uint64RereadResultPairAllocator;

	typedef map<uint64_t, RereadResult, std::less<uint64_t>, Uint64RereadResultPairAllocator> RereadResultMap;
	typedef allocator<RereadResultMap, managed_shared_memory::segment_manager>				  RereadResultMapAllocator;

	using _RereadResultInt = std::underlying_type<RereadResult>::type;
	RereadResult operator|( const RereadResult & l, const RereadResult & r ) noexcept
	{
		return static_cast<RereadResult>( static_cast<_RereadResultInt>( l ) | static_cast<_RereadResultInt>( r ) );
	}
	RereadResult operator&( const RereadResult & l, const RereadResult & r ) noexcept
	{
		return static_cast<RereadResult>( static_cast<_RereadResultInt>( l ) & static_cast<_RereadResultInt>( r ) );
	}

} // namespace VTX::IO::test
