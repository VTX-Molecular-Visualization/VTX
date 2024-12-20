#ifndef __VTX_RENDERER_CONTEXT_DEFAULT__
#define __VTX_RENDERER_CONTEXT_DEFAULT__

#include "concept_context.hpp"

namespace VTX::Renderer::Context
{
	/**
	 * @brief Default context for no-gpu usage (CI).
	 */
	class Default : public BaseContext
	{
	  public:
		inline void
		build( const RenderQueue &, const Links &, const Handle, const std::vector<BufferData> &, Instructions &, InstructionsDurationRanges & )
		{
		}

		inline void resize( const RenderQueue &, const size_t, const size_t ) {}

		inline void setOutput( const Handle ) {}

		template<typename T>
		inline void setValue( const T &, const Key &, const size_t = 0 )
		{
		}

		template<typename T>
		inline void reserveData( const size_t p_size, const Key & p_key, const T = T() )
		{
		}

		template<typename T>
		inline void set( const std::vector<T> &, const Key & )
		{
		}

		template<typename T>
		inline void setSub(
			const std::vector<T> &,
			const Key &,
			const size_t = 0,
			const bool	 = false,
			const size_t = 0
		)
		{
		}

		template<typename T>
		inline void get( std::vector<T> & p_data, const Key & p_key )
		{
		}

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

		inline void getTextureData( std::any &, const size_t, const size_t, const std::string &, const E_CHAN_OUT ) {}

		inline void compute( const ComputePass & ) {}
	};
} // namespace VTX::Renderer::Context

#endif
