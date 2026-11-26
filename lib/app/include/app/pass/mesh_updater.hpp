#ifndef VTX_APP_PASS_MESH_UPDATER_HPP
#define VTX_APP_PASS_MESH_UPDATER_HPP

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include <renderer/descriptors.hpp>
#include <unordered_map>
#include <util/event_hub.hpp>
#include <vector>

namespace VTX::App::Pass
{
	/**
	 * @brief System that update mesh and push to renderer.
	 */
	class MeshUpdater : public IPass
	{
	  public:
		/**
		 * @brief Flag that trigger update.
		 */
		static constexpr E_PENDING_UPDATE UPDATE_TRIGGERS = E_PENDING_UPDATE::PASSES;

		/**
		 * @brief Constructor.
		 */
		MeshUpdater();

		/**
		 * @brief Update the renderer with pending changes.
		 */
		void update( float, float );

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
		void _onUpdateTransform( Registry &, Entity );
		void _onDestroyMesh( Registry &, Entity );

		/**
		 * @brief Connections.
		 */
		Util::EventHub::ScopedConnection _meshLoadConnection;
		Util::EventHub::ScopedConnection _transformUpdatedConnection;
		Util::EventHub::ScopedConnection _meshDestroyedConnection;
	};
} // namespace VTX::App::Pass

#endif // !VTX_APP_PASS_MESH_UPDATER_HPP
