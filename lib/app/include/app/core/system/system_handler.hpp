#ifndef __VTX_APP_CORE_SYSTEM_SYSTEM_HANDLER__
#define __VTX_APP_CORE_SYSTEM_SYSTEM_HANDLER__

#include "app/core/system/base_system.hpp"
#include <cassert>
#include <concepts>
#include <map>
#include <memory>
#include <util/hashing.hpp>
#include <vector>

namespace VTX::App::Core::System
{
	template<typename T>
	concept SystemConcept = std::derived_from<T, BaseSystem>;

	class SystemHandler
	{
	  public:
		SystemHandler()	 = default;
		~SystemHandler() = default;

		template<SystemConcept T>
		void store( const Hash & p_hash )
		{
			assert( not _systems.contains( p_hash ) );
			const std::unique_ptr<BaseSystem> & ptr = _storage.emplace_back( std::make_unique<T>() );
			_systems.emplace( p_hash, ptr.get() );
		}

		void reference( const Hash & p_hash, BaseSystem * p_systemPtr )
		{
			assert( not _systems.contains( p_hash ) );
			_systems.emplace( p_hash, p_systemPtr );
		}

		inline bool exists( const Hash & p_hash ) const { return _systems.find( p_hash ) != _systems.end(); }

		template<SystemConcept T>
		inline T & get( const Hash & p_hash )
		{
			assert( _systems.contains( p_hash ) );
			return static_cast<T &>( *_systems[ p_hash ] );
		}

	  private:
		std::vector<std::unique_ptr<BaseSystem>> _storage;
		std::map<Hash, BaseSystem *>			 _systems;
	};
} // namespace VTX::App::Core::System

#endif
