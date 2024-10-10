#ifndef __VTX_APP_CORE_ECS_BASE_ENTITY__
#define __VTX_APP_CORE_ECS_BASE_ENTITY__

#include <entt/entity/fwd.hpp>
#include <util/constants.hpp>

namespace VTX::App::Core::ECS
{
	class BaseEntity
	{
		friend class Registry;

	  public:
		BaseEntity() = default;
		BaseEntity( const entt::id_type p_value ) { _entityID = entt::entity( p_value ); }
		BaseEntity( const entt::entity p_value ) { _entityID = p_value; }
		virtual ~BaseEntity() = default;

		virtual void setup() {}

		/*
		BaseEntity & operator=( const entt::entity p_enttEntity )
		{
			_entityID = p_enttEntity;
			return *this;
		}
		*/

		bool operator==( const BaseEntity & p_other ) const { return _entityID == p_other._entityID; }
		bool operator!=( const BaseEntity & p_other ) const { return _entityID != p_other._entityID; }
		bool operator<( const BaseEntity & p_other ) const { return _entityID < p_other._entityID; }
		bool operator<=( const BaseEntity & p_other ) const { return _entityID <= p_other._entityID; }
		bool operator>( const BaseEntity & p_other ) const { return _entityID > p_other._entityID; }
		bool operator>=( const BaseEntity & p_other ) const { return _entityID >= p_other._entityID; }

		operator entt::entity() const { return _entityID; }

	  private:
		entt::entity _entityID = entt::entity( UNSIGNED_INT_MAX );

		void _setEntityIDType( const entt::id_type p_value ) { _entityID = entt::entity( p_value ); }
		void _setEntityID( const entt::entity p_value ) { _entityID = p_value; }
	};

	inline const BaseEntity INVALID_ENTITY;
} // namespace VTX::App::Core::ECS

#endif
