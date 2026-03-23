#ifndef __VTX_BENCH_SCENE__
#define __VTX_BENCH_SCENE__

#include "camera.hpp"
#include <core/struct/system.hpp>
#include <renderer/color.hpp>

namespace VTX::Renderer
{
	class Facade;
}

namespace VTX::Bench
{
	class Scene
	{
	  public:
		Scene() = delete;
		Scene( const size_t p_width, const size_t p_height );

		inline Camera & getCamera() { return _camera; }

		void addSystem( const std::string & p_name );
		void removeSystem( const size_t p_index );

		inline void update( const float p_deltaTime )
		{
			// if ( not isUpdate )
			//{
			//	return;
			// }

			int i = 0;
			// static uint currentFrame = 0;

			for ( auto & system : _systems )
			{
				// system->transform = Util::Math::rotate( system->transform, p_deltaTime, _directions[ i ] );

				//_proxySystems[ i ]->atomPositions
				//	= &system->trajectory.frames[ currentFrame++ % system->trajectory.frames.size() ];
				//_proxySystems[ i ]->onAtomPositions();

				//_proxySystems[ i++ ]->onTransform();
			}
		}

		inline const Renderer::Color::Layout & getColorLayout() const { return _colorLayout; }
		inline void setColorLayout( const Renderer::Color::Layout & p_colorLayout ) { _colorLayout = p_colorLayout; }

		bool isUpdate = false;

	  private:
		Camera _camera;
		// Renderer::Proxy::Camera _proxyCamera;

		std::vector<std::unique_ptr<Core::Struct::Topology>> _systems;
		std::vector<Vec3f>								   _directions;

		Renderer::Color::Layout _colorLayout;

		// std::unique_ptr<Renderer::Proxy::System> _proxify( const Core::Struct::System & p_system );
	};

} // namespace VTX::Bench

#endif
