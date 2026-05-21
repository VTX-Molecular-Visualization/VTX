#ifndef __VTX_BENCH_SCENE__
#define __VTX_BENCH_SCENE__

#include "camera_controller.hpp"
#include <core/struct/topology.hpp>
#include <renderer/caches.hpp>
#include <renderer/color.hpp>
#include <renderer/system_data.hpp>
#include <unordered_set>
#include <util/math/bitset.hpp>

namespace VTX::Renderer
{
	class Renderer;
}

namespace VTX::Bench
{
	class Scene
	{
	  public:
		Scene() = delete;
		Scene( const size_t p_width, const size_t p_height );

		inline CameraController & getCamera() { return _camera; }

		void addSystem( const std::string & p_name );
		void removeSystem( const size_t p_index );
		void syncRenderer( Renderer::Renderer & p_renderer ) const;

		inline void update( const float p_deltaTime )
		{
			if ( not isUpdate )
			{
				return;
			}

			//
		}

		bool isUpdate = false;

	  private:
		struct SystemEntry
		{
			std::unique_ptr<Core::Struct::Topology> topology;
			std::vector<Vec3f>						positions;
			Mat4f									transform = MAT4F_ID;
			SystemUID								uid		  = 0;
			std::vector<PickingUID>					atomUids;
			std::vector<PickingUID>					residueUids;
		};

		[[nodiscard]] Renderer::SystemData _buildRendererSystem( const SystemEntry & ) const;
		[[nodiscard]] std::vector<Renderer::ColorIndex> _buildAtomColors(
			const Core::Struct::Topology & p_topology
		) const;
		[[nodiscard]] Renderer::MapRepresentationRanges _buildDefaultRepresentation(
			const Core::Struct::Topology & p_topology
		) const;

		CameraController		 _camera;
		std::vector<SystemEntry> _systems;
		mutable std::unordered_set<SystemUID> _syncedSystemUids;
		SystemUID				 _nextSystemUid	 = 1;
		PickingUID				 _nextPickingUid = 1;
	};

} // namespace VTX::Bench

#endif
