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

		// TODO: remove after debug.
		static std::vector<Vec3f> mins, maxs;
		for ( float x = -100.f; x < 100.f; x += 50.f )
		{
			for ( float y = -100.f; y < 100.f; y += 50.f )
			{
				for ( float z = -100.f; z < 100.f; z += 50.f )
				{
					mins.emplace_back( x, y, z );
					maxs.emplace_back( x + 50.f, y + 50.f, z + 50.f );
				}
			}
		}

		RENDERER().setVoxels( mins, maxs );
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

	void SceneUpdater::_onUpdateGraphicsConfig( ECS::Registry & p_r, ECS::Entity p_e )
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

} // namespace VTX::App::Pass
