#include "app/action/system.hpp"
#include "app/services.hpp"
#include <util/event_hub.hpp>
#include <util/math.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Action::System
{
	void SetPosition::execute( const ECS::Entity p_ent, const Vec3f & p_position )
	{
		REG().patch<Util::Math::Transform>(
			p_ent, [ &p_position ]( Util::Math::Transform & p_transform ) { p_transform.setPosition( p_position ); }
		);
	}

	void SetRotation::execute( const ECS::Entity p_ent, const Quatf & p_rotation )
	{
		REG().patch<Util::Math::Transform>(
			p_ent, [ &p_rotation ]( Util::Math::Transform & p_transform ) { p_transform.setRotation( p_rotation ); }
		);
	}

	void SetScale::execute( const ECS::Entity p_ent, const Vec3f & p_scale )
	{
		REG().patch<Util::Math::Transform>(
			p_ent, [ &p_scale ]( Util::Math::Transform & p_transform ) { p_transform.setScale( p_scale ); }
		);
	}
} // namespace VTX::App::Action::System
