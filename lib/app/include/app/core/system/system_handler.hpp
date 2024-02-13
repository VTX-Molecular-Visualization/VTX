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
		void store( const Util::Hashing::Hash & p_hash )
		{
			assert( _systems.find( p_hash ) == _systems.end() );

			const std::unique_ptr<BaseSystem> & ptr = _storage.emplace_back( std::make_unique<T>() );
			_systems.emplace( p_hash, ptr.get() );
		}

		void reference( const Util::Hashing::Hash & p_hash, BaseSystem * p_systemPtr )
		{
			assert( _systems.find( p_hash ) == _systems.end() );
			_systems.emplace( p_hash, p_systemPtr );
		}

		bool exists( const Util::Hashing::Hash & p_hash ) const { return _systems.find( p_hash ) != _systems.end(); }

		template<SystemConcept T>
		T & get( const Util::Hashing::Hash & p_hash ) const
		{
			return static_cast<T &>( *_systems.at( p_hash ) );
		}

	  private:
		std::map<Util::Hashing::Hash, BaseSystem *> _systems = std::map<Util::Hashing::Hash, BaseSystem *>();

		std::vector<std::unique_ptr<BaseSystem>> _storage = std::vector<std::unique_ptr<BaseSystem>>();
	};
} // namespace VTX::App::Core::System

#endif
