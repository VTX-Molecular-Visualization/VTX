#include "app/action/system.hpp"
#include <io/metadata.hpp>
#include <util/constants.hpp>
#include <util/math.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Action::System
{
	void SetName::execute( const Entity p_ent, const std::string_view p_name )
	{
		REG().patch<IO::Metadata>(
			p_ent, [ &p_name ]( IO::Metadata & p_metadata ) { p_metadata.name = std::string( p_name ); } );
	}

	void SetPosition::execute( const Entity p_ent, const Vec3f & p_position )
	{
		REG().patch<Util::Math::Transform>(
			p_ent, [ &p_position ]( Util::Math::Transform & p_transform ) { p_transform.setPosition( p_position ); } );
	}

	void SetRotation::execute( const Entity p_ent, const Quatf & p_rotation )
	{
		if ( Util::Math::length2( p_rotation ) < EPSILON2 )
		{
			return;
		}

		REG().patch<Util::Math::Transform>(
			p_ent, [ &p_rotation ]( Util::Math::Transform & p_transform ) { p_transform.setRotation( p_rotation ); } );
	}

	void SetScale::execute( const Entity p_ent, const Vec3f & p_scale )
	{
		const Vec3f scale = Util::Math::clamp( p_scale, SCALE_MIN, SCALE_MAX );

		REG().patch<Util::Math::Transform>(
			p_ent, [ &scale ]( Util::Math::Transform & p_transform ) { p_transform.setScale( scale ); } );
	}
} // namespace VTX::App::Action::System
