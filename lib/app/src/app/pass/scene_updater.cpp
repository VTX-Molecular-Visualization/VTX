#include "app/pass/scene_updater.hpp"
#include "app/scene/color_layout.hpp"
#include "app/scene/graphics_config.hpp"
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
		// TODO: Keep only construct and use custom event to update each value at once.
		reg.on_construct<Scene::GraphicsConfig>().connect<&SceneUpdater::_onUpdateGraphicsConfig>( this );
		reg.on_update<Scene::GraphicsConfig>().connect<&SceneUpdater::_onUpdateGraphicsConfig>( this );
		reg.on_construct<Scene::ColorLayout>().connect<&SceneUpdater::_onUpdateColorLayout>( this );
		reg.on_update<Scene::ColorLayout>().connect<&SceneUpdater::_onUpdateColorLayout>( this );

		reg.on_update<Renderer::GraphicsConfig>().connect<&SceneUpdater::_onUpdateGraphicsConfigPreset>( this );
		reg.on_update<Renderer::Color::Layout>().connect<&SceneUpdater::_onUpdateColorLayoutPreset>( this );

		// TODO: TMP.
		reg.on_update<Renderer::Representation>().connect<&SceneUpdater::_onUpdateRepresentationPreset>( this );
	}

	void SceneUpdater::_onUpdateAABB( ECS::Registry & p_r, ECS::Entity p_e )
	{
		// TODO: use event instead of this.
		if ( p_e == _entity )
		{
			return;
		}

		auto &		 sceneAABB = p_r.get<Util::Math::AABB>( _entity );
		const auto & otherAABB = p_r.get<Util::Math::AABB>( p_e );

		sceneAABB.extend( otherAABB );
	}

	void SceneUpdater::_onUpdateGraphicsConfig( ECS::Registry & p_r, ECS::Entity )
	{
		auto &		 renderer = RENDERER();
		const auto & instance = p_r.get<Scene::GraphicsConfig>( _entity );
		const auto & preset	  = p_r.get<Renderer::GraphicsConfig>( instance.preset );
		renderer.setGraphicsConfig( preset );
	}

	void SceneUpdater::_onUpdateColorLayout( ECS::Registry & p_r, ECS::Entity )
	{
		auto &		 renderer = RENDERER();
		const auto & instance = p_r.get<Scene::ColorLayout>( _entity );
		const auto & preset	  = p_r.get<Renderer::Color::Layout>( instance.preset );
		renderer.setColorLayout( preset );
	}

	void SceneUpdater::_onUpdateGraphicsConfigPreset( ECS::Registry & p_r, ECS::Entity p_e )
	{
		auto & instance = p_r.get<Scene::GraphicsConfig>( _entity );
		if ( instance.preset == p_e )
		{
			_onUpdateGraphicsConfig( p_r, _entity );
		}
	}

	void SceneUpdater::_onUpdateColorLayoutPreset( ECS::Registry & p_r, ECS::Entity p_e )
	{
		auto & instance = p_r.get<Scene::ColorLayout>( _entity );
		if ( instance.preset == p_e )
		{
			_onUpdateColorLayout( p_r, _entity );
		}
	}

	//
	void SceneUpdater::_onUpdateRepresentationPreset( ECS::Registry & p_r, ECS::Entity p_e )
	{
		auto &		 renderer = RENDERER();
		const auto & preset	  = p_r.get<Renderer::Representation>( p_e );
		renderer.setRepresentation( preset );
	}

} // namespace VTX::App::Pass
