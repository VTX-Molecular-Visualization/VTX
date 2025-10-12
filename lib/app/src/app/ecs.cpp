#include "app/ecs.hpp"

namespace
{
	VTX::App::ECS::Registry * _reg = nullptr;
}

namespace VTX::App::ECS
{
	void setRegistry( Registry & p_reg ) { _reg = &p_reg; }

	Registry & registry() noexcept
	{
		assert( _reg );
		return *_reg;
	}
} // namespace VTX::App::ECS
