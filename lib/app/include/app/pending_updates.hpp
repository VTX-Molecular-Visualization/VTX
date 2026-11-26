#ifndef VTX_APP_PENDING_UPDATES_HPP
#define VTX_APP_PENDING_UPDATES_HPP

#include "app/services.hpp"
#include <cstdint>
#include <util/enum.hpp>
#include <util/pending_mask.hpp>

namespace VTX::App
{
	/**
	 * @brief Dirty flags.
	 */
	enum struct E_PENDING_UPDATE : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE	   = 0,
		INPUT	   = 1 << 0,
		ANIMATION  = 1 << 1,
		TRAJECTORY = 1 << 2,
		EVENTS	   = 1 << 3,
		THREADS	   = 1 << 4,
		PASSES	   = 1 << 5,
		RENDER	   = 1 << 6,
		ALL		   = INPUT | ANIMATION | TRAJECTORY | EVENTS | THREADS | PASSES | RENDER
	};

	/**
	 * @brief A class to manage pending updates.
	 * Use a class instead of a type alias to avoid inclusions in services.hpp.
	 */
	// using PendingUpdates = Util::PendingMask<E_PENDING_UPDATE>;
	class PendingUpdates final : public Util::PendingMask<E_PENDING_UPDATE>
	{
	};

	/**
	 * @brief Bind the target pending callback to mark BIT in pending updates.
	 */
	template<E_PENDING_UPDATE BIT, Util::PendingNotifier T>
	void bindPending( T & p_target )
	{
		p_target.setOnPending(
			[]( void * const p_pendingUpdates ) noexcept
			{ static_cast<PendingUpdates *>( p_pendingUpdates )->mark( BIT ); },
			&PENDING_UPDATES()
		);
	}

	/**
	 * @brief Unbind the target pending callback.
	 */
	template<Util::PendingNotifier T>
	void unbindPending( T & p_target ) noexcept
	{ p_target.setOnPending( nullptr, nullptr ); }

} // namespace VTX::App

#endif
