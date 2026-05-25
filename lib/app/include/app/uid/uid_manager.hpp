#ifndef __VTX_APP_CORE_UID_UID_MANAGER__
#define __VTX_APP_CORE_UID_UID_MANAGER__

#include <util/uid.hpp>

namespace VTX::App::Uid
{

	/**
	 * @brief Manage multiple UID pools.
	 */
	class UIDManager
	{
	  public:
		inline Util::Uid::Pool<UID32> & getPickingPool() { return _pickingPool; }

	  private:
		/**
		 * @brief Pool for picking UIDs (atoms/residues).
		 */
		Util::Uid::Pool<UID32> _pickingPool;
	};
} // namespace VTX::App::Uid

#endif
