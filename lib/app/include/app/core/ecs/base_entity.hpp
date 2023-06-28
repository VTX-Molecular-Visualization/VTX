#ifndef __VTX_APP_CORE_ECS_BASE_ENTITY__
#define __VTX_APP_CORE_ECS_BASE_ENTITY__

#include <entt/entt.hpp>
#include <limits>

namespace VTX::App::Core::ECS
{
	class BaseEntity
	{
	  public:
		BaseEntity( const entt::id_type p_value ) { _entityID = entt::entity( p_value ); }
		BaseEntity( const entt::entity p_value ) { _entityID = p_value; }

		BaseEntity & operator=( const entt::entity p_enttEntity )
		{
			_entityID = p_enttEntity;
			return *this;
		}

		bool operator==( const BaseEntity & p_other ) const { return _entityID == p_other._entityID; }
		bool operator!=( const BaseEntity & p_other ) const { return _entityID != p_other._entityID; }
		bool operator<( const BaseEntity & p_other ) const { return _entityID < p_other._entityID; }
		bool operator<=( const BaseEntity & p_other ) const { return _entityID <= p_other._entityID; }
		bool operator>( const BaseEntity & p_other ) const { return _entityID > p_other._entityID; }
		bool operator>=( const BaseEntity & p_other ) const { return _entityID >= p_other._entityID; }

		operator entt::entity() const { return _entityID; }

	  private:
		entt::entity _entityID = entt::entity( UINT_MAX );
	};

	inline static const BaseEntity INVALID_ENTITY = BaseEntity( UINT_MAX );
} // namespace VTX::App::Core::ECS

#endif
