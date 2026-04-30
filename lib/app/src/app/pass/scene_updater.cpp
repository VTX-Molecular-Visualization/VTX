#include "app/pass/scene_updater.hpp"
#include "app/scene/color_layout.hpp"
#include "app/scene/graphics_config.hpp"
#include "app/services.hpp"
#include <core/struct/topology.hpp>
#include <renderer/renderer.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{
	SceneUpdater::SceneUpdater( const ECS::Entity & p_ent ) : _entity( p_ent )
	{
		auto & reg = REG();

		// Update functions.
		reg.on_update<Util::Math::AABB>().connect<&SceneUpdater::_onUpdateAABB>( this );
		reg.on_update<Util::Math::Transform>().connect<&SceneUpdater::_onUpdateTransform>( this );
		reg.on_destroy<Core::Struct::Topology>().connect<&SceneUpdater::_onSystemDestroy>( this );
		// TODO: Keep only construct and use custom event to update each value at once.
		reg.on_construct<Scene::GraphicsConfig>().connect<&SceneUpdater::_onUpdateGraphicsConfig>( this );
		reg.on_update<Scene::GraphicsConfig>().connect<&SceneUpdater::_onUpdateGraphicsConfig>( this );
		reg.on_construct<Scene::ColorLayout>().connect<&SceneUpdater::_onUpdateColorLayout>( this );
		reg.on_update<Scene::ColorLayout>().connect<&SceneUpdater::_onUpdateColorLayout>( this );

		reg.on_update<Renderer::GraphicsConfig>().connect<&SceneUpdater::_onUpdateGraphicsConfigPreset>( this );
		reg.on_update<Renderer::Color::Layout>().connect<&SceneUpdater::_onUpdateColorLayoutPreset>( this );
	}

	void SceneUpdater::_onUpdateAABB( ECS::Registry & p_r, ECS::Entity p_e )
	{
		// TODO: use event instead of this.
		if ( p_e == _entity )
		{
			return;
		}

		_recomputeSceneAABB( p_r );
	}

	void SceneUpdater::_onUpdateTransform( ECS::Registry & p_r, ECS::Entity p_e )
	{
		auto systems = p_r.view<Core::Struct::Topology, Util::Math::AABB>();
		if ( not systems.contains( p_e ) )
		{
			return;
		}

		_recomputeSceneAABB( p_r );
	}

	void SceneUpdater::_onSystemDestroy( ECS::Registry & p_r, ECS::Entity p_e )
	{
		_recomputeSceneAABB( p_r, p_e );
	}

	void SceneUpdater::_recomputeSceneAABB( ECS::Registry & p_r, ECS::Entity p_excluded )
	{
		p_r.patch<Util::Math::AABB>(
			_entity,
			[ &p_r, p_excluded ]( Util::Math::AABB & p_sceneAABB )
			{
				p_sceneAABB.invalidate();

				auto systems = p_r.view<Core::Struct::Topology, Util::Math::AABB, Util::Math::Transform>();
				for ( const ECS::Entity system : systems )
				{
					if ( system == p_excluded )
					{
						continue;
					}

					const auto & [ aabb, transform ] = systems.get<Util::Math::AABB, Util::Math::Transform>( system );
					p_sceneAABB.extend( aabb.transformed( transform ) );
				}
			}
		);
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

} // namespace VTX::App::Pass
