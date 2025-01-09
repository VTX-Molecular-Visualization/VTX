#ifndef __VTX_RENDERER_CONTEXT_DEFAULT__
#define __VTX_RENDERER_CONTEXT_DEFAULT__

#include "concept.hpp"

namespace VTX::Renderer::Context
{
	/**
	 * @brief Default context for no-gpu usage (CI).
	 */
	class Default : public BaseContext
	{
	  public:
		Default( const size_t p_width, const size_t p_height ) : BaseContext { p_width, p_height } {}

		inline void
		build( const RenderQueue &, const Links &, const std::vector<BufferData> &, Instructions &, InstructionsDurationRanges & )
		{
		}

		inline void resize( const RenderQueue &, const size_t, const size_t ) {}

		inline void setOutput( const Handle ) {}

		inline void setValue( const Key &, const void * const, const size_t = 0 ) {}

		inline void reserveData( const Key &, const size_t ) {}

		inline void set( const Key &, const void * const, const size_t ) {}

		inline void setSub( const Key &, const void * const, const size_t, const size_t = 0 ) {}

		inline void get( const Key &, void * const, const size_t ) const {}

		inline void fillInfos( StructInfos & ) const {}

		inline float measureTaskDuration( const Util::Chrono::Task & ) const { return 0.f; }

		inline void compileShaders() const {}

		inline void snapshot(
			std::vector<uchar> &,
			const RenderQueue &,
			const Instructions &,
			const size_t,
			const size_t
		)
		{
		}

		inline void getTextureData( const Key &, std::any &, const size_t, const size_t, const E_CHAN_OUT ) const {}

		inline void compute( const ComputePass & ) {}
	};
} // namespace VTX::Renderer::Context

#endif
