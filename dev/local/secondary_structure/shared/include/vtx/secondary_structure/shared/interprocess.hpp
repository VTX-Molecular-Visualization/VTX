
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <chrono>

namespace pdb100
{
	using namespace boost::interprocess;

	namespace shm
	{
		namespace filestrDeque
		{
			const char SEGNAME[] = "VTX__SHM_FILESTR_DEQUE";
			const char OBJNAME[] = "file_collection";
			const char MUTEX[]	 = "VTX__SHM_FILESTR_MUTEX";
		} // namespace filestrDeque

		namespace rsltMap
		{
			const char SEGNAME[] = "VTX__SHM_SS_RSLT_MAP";
			const char OBJNAME[] = "result_map";
			const char MUTEX[]	 = "VTX__SHM_RSLT_MAP_MUTEX";
		} // namespace rsltMap

		/**
		 * @brief Will hold timestamp to prove that a thread is still alive, as well as the process ID of a thead
		 */
		namespace livingProof
		{
			const char SEGNAME[] = "VTX__SHM_SS_LIVINGPROOF";
			const char OBJNAME[] = "ss_liveProof";
			const char MUTEX[]	 = "VTX__SHM_SS_LIVINGPROOF_MUTEX";
		} // namespace livingProof

	} // namespace shm

	enum class ResultFlag : uint8_t
	{
		fully_working = 0,
		crashed		  = 1 << 7
	};

	typedef allocator<char, managed_shared_memory::segment_manager>	  CharAllocator;
	typedef basic_string<char, std::char_traits<char>, CharAllocator> String;
	typedef allocator<String, managed_shared_memory::segment_manager> StringAllocator;

	typedef deque<String, StringAllocator>								   StringDeque;
	typedef allocator<StringDeque, managed_shared_memory::segment_manager> StringDequeAllocator;

	typedef allocator<uint64_t, managed_shared_memory::segment_manager> Uint64Allocator;

	typedef allocator<ResultFlag, managed_shared_memory::segment_manager> ResultFlagAllocator;
	typedef allocator<std::pair<const uint64_t, ResultFlag>, managed_shared_memory::segment_manager>
		Uint64ResultFlagPairAllocator;

	typedef map<uint64_t, ResultFlag, std::less<uint64_t>, Uint64ResultFlagPairAllocator> ResultFlagMap;
	typedef allocator<ResultFlagMap, managed_shared_memory::segment_manager>			  ResultFlagMapAllocator;

	using _ResultFlagInt = std::underlying_type<ResultFlag>::type;
	ResultFlag operator|( const ResultFlag & l, const ResultFlag & r ) noexcept
	{
		return static_cast<ResultFlag>( static_cast<_ResultFlagInt>( l ) | static_cast<_ResultFlagInt>( r ) );
	}
	ResultFlag operator&( const ResultFlag & l, const ResultFlag & r ) noexcept
	{
		return static_cast<ResultFlag>( static_cast<_ResultFlagInt>( l ) & static_cast<_ResultFlagInt>( r ) );
	}
	ResultFlag operator*( const bool & l, const ResultFlag & r ) noexcept
	{
		return static_cast<ResultFlag>( l * static_cast<_ResultFlagInt>( r ) );
	}

	inline uint64_t getTimeStamp()
	{
		return static_cast<uint64_t>( std::chrono::time_point<std::chrono::system_clock>().time_since_epoch().count() );
	}

} // namespace pdb100
