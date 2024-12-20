#ifndef __VTX_RENDERER_FACADE__
#define __VTX_RENDERER_FACADE__

#include "renderer/descriptors.hpp"
#include "renderer/struct_infos.hpp"
#include <any>
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
		class Representation;
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
		void clean();
		bool hasContext() const;
		void render( const float p_deltaTime, const float p_elapsedTime );
		void setValue( const std::any & p_value, const std::string & p_key, const size_t p_index = 0 );
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

		size_t		getWidth() const;
		size_t		getHeight() const;
		Vec2i		getPickedIds( const size_t p_x, const size_t p_y ) const;
		void		setNeedUpdate( const bool p_value );
		StructInfos getInfos() const;

		static uint getBufferCount();

		// Dev.
		void addPass( const Pass & p_pass );
		void removePass( const Pass * const p_pass );
		bool addLink(
			Pass * const	   p_passSrc,
			Pass * const	   p_passDest,
			const E_CHAN_OUT & p_channelSrc	 = E_CHAN_OUT::COLOR_0,
			const E_CHAN_IN &  p_channelDest = E_CHAN_IN::_0
		);
		void							   removeLink( const Link * const p_link );
		Passes &						   getPasses();
		Links &							   getLinks();
		RenderQueue &					   getRenderQueue();
		const Output *					   getOutput();
		void							   setOutput( const Output * const p_output );
		bool							   isInRenderQueue( const Pass * const p_pass );
		void							   compileShaders() const;
		const InstructionsDurationRanges & getInstructionsDurationRanges() const;
		const std::vector<Pass *> &		   getAvailablePasses() const;

		bool * showAtoms;
		bool * showBonds;
		bool * showRibbons;
		bool * showVoxels;

		bool * forceUpdate;
		bool * logDurations;

		Draw::Range * drawRangeSpheres;
		Draw::Range * drawRangeCylinders;
		Draw::Range * drawRangeRibbons;
		Draw::Range * drawRangeVoxels;

		Util::Callback<> & onReady();

	  private:
		std::unique_ptr<Renderer> _renderer;
	};

} // namespace VTX::Renderer

#endif
