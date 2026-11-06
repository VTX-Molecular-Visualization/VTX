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
		UIDManager()								 = default;
		UIDManager( const UIDManager & )			 = delete;
		UIDManager & operator=( const UIDManager & ) = delete;
		UIDManager( UIDManager && )					 = delete;
		UIDManager & operator=( UIDManager && )		 = delete;
		~UIDManager()								 = default;

		Util::Uid::Pool<UID32> & getPickingPool() noexcept { return _pickingPool; }

	  private:
		/**
		 * @brief Pool for picking UIDs (atoms/residues).
		 */
		Util::Uid::Pool<UID32> _pickingPool;
	};
} // namespace VTX::App::Uid

#endif
