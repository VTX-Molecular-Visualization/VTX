#ifndef __VTX_RENDERER_FACADE__
#define __VTX_RENDERER_FACADE__

#include "struct_infos.hpp"
#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX::Renderer
{
	class Renderer;
	namespace Proxy
	{
		struct Camera;
		struct ColorLayout;
		struct System;
		class RenderSettings;
		struct Representation;
		struct Voxels;
	} // namespace Proxy

	/**
	 * @brief The facade is the only way to access the renderer from another package.
	 */
	class Facade
	{
	  public:
		Facade() = delete;
		Facade( const size_t p_width, const size_t p_height, const FilePath & p_shaderPath, void * p_loader = nullptr );
		~Facade();
		void resize( const size_t p_width, const size_t p_height, const uint p_output = 0 );
		void build( const uint p_output = 0, void * p_loader = nullptr );
		void render( const float p_deltaTime, const float p_elapsedTime );
		void setOutput( const uint p_output );
		void addProxySystem( Proxy::System & p_proxy );
		void removeProxySystem( Proxy::System & p_proxy );
		void addProxySystems( std::vector<Proxy::System *> & p_proxies );
		void removeProxySystems( std::vector<Proxy::System *> & p_proxies );
		void addProxyRepresentation( Proxy::Representation & p_proxy );
		void removeProxyRepresentation( Proxy::Representation & p_proxy );
		void addProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies );
		void removeProxyRepresentations( std::vector<Proxy::Representation *> & p_proxies );
		void setProxyCamera( Proxy::Camera & p_proxy );
		void setProxyColorLayout( Proxy::ColorLayout & p_proxy );
		void setProxyRenderSettings( Proxy::RenderSettings & p_proxy );
		void setProxyVoxels( Proxy::Voxels & p_proxy );
		void snapshot(
			std::vector<uchar> & p_image,
			const size_t		 p_width,
			const size_t		 p_height,
			const float			 p_fov,
			const float			 p_near,
			const float			 p_far
		);
		Vec2i		getPickedIds( const size_t p_x, const size_t p_y ) const;
		void		setNeedUpdate( const bool p_value );
		StructInfos getInfos() const;

		Util::Callback<> & onReady();

	  private:
		std::unique_ptr<Renderer> _renderer;
	};

} // namespace VTX::Renderer

#endif
