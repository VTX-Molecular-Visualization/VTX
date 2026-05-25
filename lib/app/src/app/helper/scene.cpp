#include "app/helper/scene.hpp"
#include "app/system/uid.hpp"

namespace VTX::App::Helper::Scene
{
	std::vector<Entity> getAllSystems()
	{
		const auto view = REG().view<System::UID>();
		return { view.begin(), view.end() };
	}

	std::optional<Entity> findSystemByAtomUID( const UID32 p_uid )
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

	std::optional<Entity> findSystemByResidueUID( const UID32 p_uid )
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
