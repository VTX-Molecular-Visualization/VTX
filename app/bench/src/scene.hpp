#ifndef __VTX_BENCH_SCENE__
#define __VTX_BENCH_SCENE__

#include "camera.hpp"
#include <core/struct/color_layout.hpp>
#include <core/struct/system.hpp>
#include <renderer/proxy/camera.hpp>
#include <renderer/proxy/color_layout.hpp>
#include <renderer/proxy/system.hpp>

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

		inline Camera &					 getCamera() { return _camera; }
		inline Renderer::Proxy::Camera & getProxyCamera() { return _proxyCamera; }

		Renderer::Proxy::System & addSystem( const std::string & p_name );
		void					  removeSystem( const size_t p_index );

		// TODO: remove renderer from here.
		void removeAllSystems( Renderer::Renderer * const p_renderer );

		inline void update( const float p_deltaTime )
		{
			if ( not isUpdate )
			{
				return;
			}

			int i = 0;
			// static uint currentFrame = 0;

			for ( auto & system : _systems )
			{
				system->transform = Util::Math::rotate( system->transform, p_deltaTime, _directions[ i ] );

				//_proxySystems[ i ]->atomPositions
				//	= &system->trajectory.frames[ currentFrame++ % system->trajectory.frames.size() ];
				//_proxySystems[ i ]->onAtomPositions();

				_proxySystems[ i++ ]->onTransform();
			}
		}

		inline const std::vector<std::unique_ptr<Core::Struct::System>> &	 getSystems() const { return _systems; }
		inline const std::vector<std::unique_ptr<Renderer::Proxy::System>> & getProxiesSystems() const
		{
			return _proxySystems;
		}
		inline const Core::Struct::ColorLayout & getColorLayout() const { return _colorLayout; }
		inline void								 setColorLayout( const Core::Struct::ColorLayout & p_colorLayout )
		{
			_colorLayout = p_colorLayout;
			_proxyLayoutColor.onChangeAll();
		}
		inline Renderer::Proxy::ColorLayout & getProxyColorLayout() { return _proxyLayoutColor; }

		bool isUpdate = false;

	  private:
		Camera					_camera;
		Renderer::Proxy::Camera _proxyCamera;

		std::vector<std::unique_ptr<Core::Struct::System>>	  _systems;
		std::vector<std::unique_ptr<Renderer::Proxy::System>> _proxySystems;
		std::vector<Vec3f>									  _directions;

		Core::Struct::ColorLayout	 _colorLayout;
		Renderer::Proxy::ColorLayout _proxyLayoutColor;

		std::unique_ptr<Renderer::Proxy::System> _proxify( const Core::Struct::System & p_system );
	};

} // namespace VTX::Bench

#endif
