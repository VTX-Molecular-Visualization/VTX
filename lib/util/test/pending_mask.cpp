#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <thread>
#include <tuple>
#include <util/pending_mask.hpp>
#include <vector>

namespace
{
	enum struct E_TEST : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE = 0,
		A	 = 1 << 0,
		B	 = 1 << 1,
		C	 = 1 << 2,
		ALL	 = A | B | C
	};

	void countWake( void * const p_counter ) noexcept
	{ static_cast<std::atomic<int> *>( p_counter )->fetch_add( 1, std::memory_order_relaxed ); }

	struct WakeableStub
	{
		void setWake( VTX::Util::WakeCallback::Function, void * ) noexcept {}
	};

	struct ThrowingWakeableStub
	{
		void setWake( VTX::Util::WakeCallback::Function, void * ) {}
	};

	struct NotWakeableStub
	{
	};
} // namespace

static_assert( VTX::Util::Wakeable<VTX::Util::PendingMask<E_TEST>> );
static_assert( VTX::Util::Wakeable<WakeableStub> );
static_assert( not VTX::Util::Wakeable<ThrowingWakeableStub> );
static_assert( not VTX::Util::Wakeable<NotWakeableStub> );

TEST_CASE( "VTX_UTIL - WakeCallback", "[unit]" )
{
	using namespace VTX::Util;

	std::atomic<int> counter = 0;
	WakeCallback	 callback;

	callback();
	CHECK( counter.load() == 0 );

	callback.set( &countWake, &counter );
	callback();
	callback();
	CHECK( counter.load() == 2 );

	callback.set( nullptr, nullptr );
	callback();
	CHECK( counter.load() == 2 );
}

TEST_CASE( "VTX_UTIL - PendingMask - mark and consume", "[unit]" )
{
	using namespace VTX::Util;

	PendingMask<E_TEST> mask;

	CHECK( not mask.any() );
	CHECK( mask.consume() == E_TEST::NONE );

	mask.mark( E_TEST::NONE );
	CHECK( not mask.any() );

	mask.mark( E_TEST::A );
	CHECK( mask.any() );

	mask.mark( E_TEST::B );
	mask.mark( E_TEST::A );
	CHECK( mask.consume() == ( E_TEST::A | E_TEST::B ) );
	CHECK( not mask.any() );
	CHECK( mask.consume() == E_TEST::NONE );
}

TEST_CASE( "VTX_UTIL - PendingMask - wake on transition only", "[unit]" )
{
	using namespace VTX::Util;

	std::atomic<int>	counter = 0;
	PendingMask<E_TEST> mask;

	mask.mark( E_TEST::A );
	CHECK( counter.load() == 0 );
	std::ignore = mask.consume();

	mask.setWake( &countWake, &counter );

	mask.mark( E_TEST::NONE );
	CHECK( counter.load() == 0 );

	mask.mark( E_TEST::A );
	CHECK( counter.load() == 1 );
	mask.mark( E_TEST::B );
	mask.mark( E_TEST::A );
	CHECK( counter.load() == 1 );

	std::ignore = mask.consume();
	mask.mark( E_TEST::C );
	CHECK( counter.load() == 2 );

	std::ignore = mask.consume();
	mask.setWake( nullptr, nullptr );
	mask.mark( E_TEST::A );
	CHECK( counter.load() == 2 );
	CHECK( mask.any() );
}

TEST_CASE( "VTX_UTIL - PendingMask - concurrent marks", "[unit]" )
{
	using namespace VTX::Util;

	constexpr int MARK_COUNT = 10000;

	std::atomic<int>	counter = 0;
	PendingMask<E_TEST> mask;
	mask.setWake( &countWake, &counter );

	{
		std::vector<std::jthread> producers;
		for ( const E_TEST flag : { E_TEST::A, E_TEST::B, E_TEST::C } )
		{
			producers.emplace_back(
				[ &mask, flag ]()
				{
					for ( int i = 0; i < MARK_COUNT; ++i )
					{
						mask.mark( flag );
					}
				}
			);
		}
	}

	CHECK( counter.load() == 1 );
	CHECK( mask.consume() == E_TEST::ALL );
}

TEST_CASE( "VTX_UTIL - PendingMask - concurrent marks and consumes", "[unit]" )
{
	using namespace VTX::Util;

	constexpr int MARK_COUNT = 10000;

	std::atomic<int>	counter			 = 0;
	std::atomic<bool>	running			 = true;
	int					nonEmptyConsumes = 0;
	E_TEST				consumed		 = E_TEST::NONE;
	PendingMask<E_TEST> mask;
	mask.setWake( &countWake, &counter );

	std::jthread consumer(
		[ & ]()
		{
			while ( running.load() )
			{
				const E_TEST flags = mask.consume();
				if ( flags != E_TEST::NONE )
				{
					++nonEmptyConsumes;
					consumed |= flags;
				}
			}
		}
	);

	{
		std::vector<std::jthread> producers;
		for ( const E_TEST flag : { E_TEST::A, E_TEST::B, E_TEST::C } )
		{
			producers.emplace_back(
				[ &mask, flag ]()
				{
					for ( int i = 0; i < MARK_COUNT; ++i )
					{
						mask.mark( flag );
					}
				}
			);
		}
	}

	running = false;
	consumer.join();

	const E_TEST last = mask.consume();
	if ( last != E_TEST::NONE )
	{
		++nonEmptyConsumes;
		consumed |= last;
	}

	CHECK( consumed == E_TEST::ALL );
	CHECK( counter.load() == nonEmptyConsumes );
}
