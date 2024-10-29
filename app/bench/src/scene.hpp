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

		Renderer::Proxy::System & addMolecule( const std::string & p_name );
		void					  removeMolecule( const size_t p_index );

		// TODO: remove renderer from here.
		void removeAllMolecules( Renderer::Renderer * const p_renderer );

		inline void update( const float p_deltaTime )
		{
			if ( not isUpdate )
			{
				return;
			}

			int i = 0;
			// static uint currentFrame = 0;

			for ( auto & molecule : _molecules )
			{
				molecule->transform = Util::Math::rotate( molecule->transform, p_deltaTime, _directions[ i ] );

				//_proxyMolecules[ i ]->atomPositions
				//	= &molecule->trajectory.frames[ currentFrame++ % molecule->trajectory.frames.size() ];
				//_proxyMolecules[ i ]->onAtomPositions();

				_proxyMolecules[ i++ ]->onTransform();
			}
		}

		inline const std::vector<std::unique_ptr<Core::Struct::System>> &	 getMolecules() const { return _molecules; }
		inline const std::vector<std::unique_ptr<Renderer::Proxy::System>> & getProxiesMolecules() const
		{
			return _proxyMolecules;
		}
		inline const Core::Struct::ColorLayout & getColorLayout() const { return _colorLayout; }
		inline void								 setColorLayout( const Core::Struct::ColorLayout & p_colorLayout )
		{
			_colorLayout = p_colorLayout;
			_proxyLayoutColor.onChange();
		}
		inline Renderer::Proxy::ColorLayout & getProxyColorLayout() { return _proxyLayoutColor; }

		bool isUpdate = false;

	  private:
		Camera					_camera;
		Renderer::Proxy::Camera _proxyCamera;

		std::vector<std::unique_ptr<Core::Struct::System>>	  _molecules;
		std::vector<std::unique_ptr<Renderer::Proxy::System>> _proxyMolecules;
		std::vector<Vec3f>									  _directions;

		Core::Struct::ColorLayout	 _colorLayout;
		Renderer::Proxy::ColorLayout _proxyLayoutColor;

		std::unique_ptr<Renderer::Proxy::System> _proxify( const Core::Struct::System & p_molecule );
	};

} // namespace VTX::Bench

#endif
