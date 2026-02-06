#include "app/helper/scene.hpp"
#include "app/system/uid.hpp"

namespace VTX::App::Helper::Scene
{
	std::vector<ECS::Entity> getAllSystems()
	{
		const auto view = REG().view<System::UID>();
		return { view.begin(), view.end() };
	}

	std::unordered_map<SystemUID, ECS::Entity> getAllSystemsMap()
	{
		std::unordered_map<SystemUID, ECS::Entity> systemsMap;
		auto									   view = REG().view<System::UID>();

		for ( const auto & [ entity, uid ] : view.each() )
		{
			systemsMap[ uid.system ] = entity;
		}

		return systemsMap;
	}

	std::optional<ECS::Entity> findSystemByRootUID( const SystemUID p_uid )
	{
		const auto view = REG().view<System::UID>();
		for ( const auto & [ entity, uid ] : view.each() )
		{
			if ( uid.system == p_uid )
			{
				return entity;
			}
		}

		return std::nullopt;
	}

	std::optional<ECS::Entity> findSystemByAtomUID( const PickingUID p_uid )
	{
		const auto view = REG().view<System::UID>();
		for ( const auto & [ entity, uid ] : view.each() )
		{
			if ( uid.containsAtomUID( p_uid ) )
			{
				return entity;
			}
		}

		return std::nullopt;
	}

	std::optional<ECS::Entity> findSystemByResidueUID( const PickingUID p_uid )
	{
		const auto view = REG().view<System::UID>();
		for ( const auto & [ entity, uid ] : view.each() )
		{
			if ( uid.containsResidueUID( p_uid ) )
			{
				return entity;
			}
		}

		return std::nullopt;
	}

} // namespace VTX::App::Helper::Scene
