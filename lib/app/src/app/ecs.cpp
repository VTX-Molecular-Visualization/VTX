#include "app/ecs.hpp"
#include <iostream>

namespace
{
	// Global ECS registry instance.
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
	VTX::Registry * _reg = nullptr;
} // namespace

namespace VTX::App::ECS
{
	void setRegistry( Registry & p_reg ) { _reg = &p_reg; }

	Registry & registry() noexcept
	{
		assert( _reg );
		return *_reg;
	}

	void toString()
	{
		for ( const entt::entity e : registry().view<entt::entity>() )
		{
			std::cout << "entity " << static_cast<int>( e ) << '\n';

			for ( auto && [ id, storage ] : registry().storage() )
			{
				if ( storage.contains( e ) )
				{
					std::cout << " - component id: " << id << '\n';
				}
			}
		}
	}
} // namespace VTX::App::ECS
