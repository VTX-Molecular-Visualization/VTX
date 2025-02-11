#ifndef __VTX_RENDERER_FACADE__
#define __VTX_RENDERER_FACADE__

#include "renderer/descriptors.hpp"
#include "renderer/struct_infos.hpp"
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
	 * @brief The facade is the only way to access the renderer from another package (pimpl).
	 */
	class Facade
	{
	  public:
		Facade() = delete;
		Facade( const size_t p_width, const size_t p_height );
		~Facade();

		void setDefault();
		void setOpenGL45( const FilePath & p_shaderPath, void * p_loader = nullptr );

		void resize( const size_t p_width, const size_t p_height );
		void build();
		void clean();
		void render( const float p_deltaTime, const float p_elapsedTime );

		void setOutput( const uint );
		void addProxySystem( Proxy::System & );
		void removeProxySystem( Proxy::System & );
		void addProxySystems( std::vector<Proxy::System *> & );
		void removeProxySystems( std::vector<Proxy::System *> & );
		void setProxyRepresentation( Proxy::Representation & );
		void setProxyCamera( Proxy::Camera & );
		void setProxyColorLayout( Proxy::ColorLayout & );
		void setProxyRenderSettings( Proxy::RenderSettings & );
		void setProxyVoxels( Proxy::Voxels & );
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
		const Passes &					   getPasses() const;
		const Links &					   getLinks() const;
		const RenderQueue &				   getRenderQueue() const;
		const Output * const			   getOutput() const;
		void							   setOutput( const Output * const p_output );
		void							   compileShaders() const;
		const InstructionsDurationRanges & getInstructionsDurationRanges() const;
		const std::vector<Pass *> &		   getAvailablePasses() const;

		void setValue( const float p_value, const std::string & p_key, const size_t p_index = 0 );
		void setValue( const int p_value, const std::string & p_key, const size_t p_index = 0 );
		void setValue( const uint p_value, const std::string & p_key, const size_t p_index = 0 );
		void setValue( const bool p_value, const std::string & p_key, const size_t p_index = 0 );
		void setValue( const Vec2f & p_value, const std::string & p_key, const size_t p_index = 0 );
		void setValue( const Vec2i & p_value, const std::string & p_key, const size_t p_index = 0 );
		void setValue( const Vec3f & p_value, const std::string & p_key, const size_t p_index = 0 );
		void setValue( const Vec4f & p_value, const std::string & p_key, const size_t p_index = 0 );

		// TODO: accessors.
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
