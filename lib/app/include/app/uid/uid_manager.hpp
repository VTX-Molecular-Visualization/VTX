#ifndef VTX_APP_UID_UID_MANAGER_HPP
#define VTX_APP_UID_UID_MANAGER_HPP

#include <util/uid.hpp>

namespace VTX::App::Uid
{

	/**
	 * @brief Manage multiple UID pools.
	 */
	class UIDManager
	{
	  public:
		Util::Uid::Pool<UID32> & getPickingPool() { return _pickingPool; }

	  private:
		/**
		 * @brief Pool for picking UIDs (atoms/residues).
		 */
		Util::Uid::Pool<UID32> _pickingPool;
	};
} // namespace VTX::App::Uid

#endif
