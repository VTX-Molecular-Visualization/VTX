#include "app/ecs.hpp"

namespace
{
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
} // namespace VTX::App::ECS
