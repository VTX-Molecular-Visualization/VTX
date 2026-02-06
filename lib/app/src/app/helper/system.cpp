#include "app/helper/system.hpp"
#include <core/struct/system.hpp>
namespace VTX::App::Helper::System
{
	ECS::Entity getSystemByName( const std::string p_name ) noexcept
	{
		for ( auto it_entity : REG().view<Core::Struct::System>() )
		{
			auto & metadata = REG().get<Core::Struct::System>( it_entity );

			// For now we use the PDB code but the function aims to retrieve a system using what is visible to the user.
			if ( metadata.name == p_name )
				return it_entity;
		}

		return entt::null;
	}

} // namespace VTX::App::Helper::System
