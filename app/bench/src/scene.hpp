#ifndef __VTX_BENCH_SCENE__
#define __VTX_BENCH_SCENE__

#include "camera_controller.hpp"
#include <core/struct/topology.hpp>
#include <renderer/caches.hpp>
#include <renderer/color.hpp>
#include <renderer/descriptors.hpp>
#include <renderer/representation.hpp>
#include <unordered_map>
#include <unordered_set>
#include <util/math/bitset.hpp>
#include <util/math/range.hpp>

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
			std::unique_ptr<Core::Struct::Topology>								   topology;
			std::vector<Vec3f>													   positions;
			Mat4f																   transform = MAT4F_ID;
			uint																   uid		 = 0;
			Util::Math::Range<UID32>											   atomUids;
			Util::Math::Range<UID32>											   residueUids;
			std::unordered_map<Renderer::E_COLOR_SCHEME, Renderer::IndexRangeList> colorSchemeAtoms;
			std::unordered_map<Renderer::ColorIndex, Renderer::IndexRangeList>	   customColorAtoms;
			std::unordered_map<Entity, Renderer::IndexRangeList>				   presetAtoms;
			Util::Math::BitSet													   visibility;
			Util::Math::BitSet													   selection;
		};

		[[nodiscard]] Renderer::Cache::System _buildRendererSystem( const SystemEntry & ) const;

		CameraController										 _camera;
		std::vector<SystemEntry>								 _systems;
		mutable std::unordered_map<uint, Renderer::Desc::Handle> _syncedSystems;
		uint													 _nextSystemUid = 1;
		UID32													 _nextUID32		= 1;
	};

} // namespace VTX::Bench

#endif
