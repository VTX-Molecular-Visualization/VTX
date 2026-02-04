#ifndef __VTX_APP_CORE_UID_UID_MANAGER__
#define __VTX_APP_CORE_UID_UID_MANAGER__

#include "app/uid/pool.hpp"

namespace VTX::App::Uid
{
	/**
	 * @brief Manager multiple UID pools.
	 */
	class UIDManager
	{
	  public:
		using PickingUIDRange = Util::Math::Range<PickingUID>;
		using RootUIDRange	  = Util::Math::Range<SystemUID>;

		inline Pool<PickingUID> & getPickingPool() { return _pickingPool; }
		inline Pool<SystemUID> &	  getRootPool() { return _rootPool; }

	  private:
		/**
		 * @brief Pool for picking UIDs (atoms/residues).
		 */
		Pool<PickingUID> _pickingPool;

		/**
		 * @brief Pool for root UIDs (system).
		 */
		Pool<SystemUID> _rootPool;
	};
} // namespace VTX::App::Uid

#endif
