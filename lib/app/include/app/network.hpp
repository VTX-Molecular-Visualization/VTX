#ifndef __VTX_APP_NETWORK__
#define __VTX_APP_NETWORK__

#include "core/network/network_request.hpp"
#include "manager/network_manager.hpp"
#include <memory>
#include <type_traits>

namespace VTX
{
	template<typename T, typename = std::enable_if<std::is_base_of<T, App::Core::Network::NetworkRequest>::value>>
	inline void VTX_NETWORK_REQUEST()
	{
		const std::shared_ptr<T> actionPtr = std::make_shared<T>();
		App::Manager::NetworkManager::get().sendRequest( actionPtr );
	}
	template<typename T,
			 typename... Args,
			 typename = std::enable_if<std::is_base_of<T, App::Core::Network::NetworkRequest>::value>>
	inline void VTX_NETWORK_REQUEST( Args... _args )
	{
		const std::shared_ptr<T> actionPtr = std::make_shared<T>( _args... );
		App::Manager::NetworkManager::get().sendRequest( actionPtr );
	}
} // namespace VTX

#endif
