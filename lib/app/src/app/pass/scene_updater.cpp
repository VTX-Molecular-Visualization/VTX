#include "app/pass/scene_updater.hpp"
#include "app/library/preset/color_layout.hpp"
#include "app/library/preset/render_settings.hpp"
#include "app/library/preset/representation.hpp"
#include "app/services.hpp"
#include <renderer/renderer.hpp>
#include <util/math/aabb.hpp>

namespace VTX::App::Pass
{
	SceneUpdater::SceneUpdater( const ECS::Entity & p_ent ) : _entity( p_ent )
	{
		auto & reg = REG();

		// Update functions.
		reg.on_update<Util::Math::AABB>().connect<&SceneUpdater::_onUpdateAABB>( this );
		reg.on_construct<Library::Preset::RenderSettings>().connect<&SceneUpdater::_onUpdateRenderSettings>( this );
		reg.on_update<Library::Preset::RenderSettings>().connect<&SceneUpdater::_onUpdateRenderSettings>( this );
		reg.on_construct<Library::Preset::Representation>().connect<&SceneUpdater::_onUpdateRepresentations>( this );
		reg.on_update<Library::Preset::ColorLayout>().connect<&SceneUpdater::_onUpdateColorLayout>( this );
		reg.on_construct<Library::Preset::ColorLayout>().connect<&SceneUpdater::_onUpdateColorLayout>( this );
		reg.on_update<Library::Preset::Representation>().connect<&SceneUpdater::_onUpdateColorLayout>( this );
	}

	void SceneUpdater::_onUpdateAABB( ECS::Registry & p_r, ECS::Entity p_e )
	{
		// Update only from others.
		if ( p_e == _entity )
		{
			return;
		}

		auto & sceneAABB = p_r.get<Util::Math::AABB>( _entity );
		auto & otherAABB = p_r.get<Util::Math::AABB>( p_e );

		sceneAABB.extend( otherAABB );
	}

	void SceneUpdater::_onUpdateRenderSettings( ECS::Registry & p_r, ECS::Entity )
	{
		auto & renderer				= RENDERER();
		auto & renderSettingsPreset = p_r.get<Library::Preset::RenderSettings>( _entity );
		renderer.setRenderSettings( renderSettingsPreset.getData() );
	}

	void SceneUpdater::_onUpdateColorLayout( ECS::Registry & p_r, ECS::Entity )
	{
		auto & renderer			 = RENDERER();
		auto & colorLayoutPreset = p_r.get<Library::Preset::ColorLayout>( _entity );
		renderer.setColorLayout( colorLayoutPreset.getData() );
	}

	void SceneUpdater::_onUpdateRepresentations( ECS::Registry & p_r, ECS::Entity )
	{
		auto & renderer				= RENDERER();
		auto & representationPreset = p_r.get<Library::Preset::Representation>( _entity );
		renderer.setRepresentation( representationPreset.getData() );
	}
} // namespace VTX::App::Pass
