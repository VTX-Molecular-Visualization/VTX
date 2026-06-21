#ifndef __VTX_APP_PASS_MESH_UPDATER__
#define __VTX_APP_PASS_MESH_UPDATER__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include <renderer/descriptors.hpp>
#include <unordered_map>
#include <vector>

namespace VTX::App::Pass
{
	/**
	 * @brief System that update mesh and push to renderer.
	 */
	class MeshUpdater : public IPass
	{
	  public:
		MeshUpdater();

		/**
		 * @brief Update the renderer with pending changes.
		 */
		void update( const float, const float );

	  private:
		/**
		 * @brief Active entities mapped to renderer indexes.
		 */
		std::unordered_map<Entity, Renderer::Desc::Handle> _meshes;

		/**
		 * @brief Entities pending to be added/removed.
		 */
		std::vector<Entity>					_meshAdded;
		std::vector<Renderer::Desc::Handle> _meshRemoved;

		/**
		 * @brief Push mesh data to renderer.
		 */
		void _onMeshLoad( const Events::MeshLoad & );
		void _onUpdateTransform( Registry &, const Entity );
		void _onDestroyMesh( Registry &, const Entity );
	};
} // namespace VTX::App::Pass

#endif // !__VTX_APP_PASS_MESH_UPDATER__
