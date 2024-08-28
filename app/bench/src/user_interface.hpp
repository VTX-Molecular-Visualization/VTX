#ifndef __VTX_BENCH_USER_INTERFACE__
#define __VTX_BENCH_USER_INTERFACE__

#include <SDL.h>
#include <imgui/imgui_impl_sdl2.h>
#include <renderer/renderer.hpp>
#include <util/type_traits.hpp>

namespace VTX::Bench
{
	class Scene;
	class Camera;

	class UserInterface
	{
	  public:
		UserInterface( const size_t p_width, const size_t p_height );
		~UserInterface();

		inline double getTime() const { return SDL_GetTicks(); }
		inline float  getDeltaTime() const { return ImGui::GetIO().DeltaTime; }
		inline void * getProcAddress() { return reinterpret_cast<void *>( SDL_GL_GetProcAddress ); }
		inline void	  setVSync( const bool p_vsync )
		{
			_vsync = p_vsync;
			SDL_GL_SetSwapInterval( _vsync );
		}

		void draw( Camera * const p_camera, Scene * const p_scene, Renderer::Renderer * const p_renderer );

		inline bool getEvent( SDL_Event & p_event ) const
		{
			bool hasEvent = SDL_PollEvent( &p_event );

			if ( hasEvent )
			{
				ImGui_ImplSDL2_ProcessEvent( &p_event );
			}

			return hasEvent;
		}

	  private:
		SDL_Window *  _window	 = nullptr;
		SDL_GLContext _glContext = nullptr;
		bool		  _vsync	 = true;
		bool		  _drawUi	 = true;

		void _drawMenuBar( Camera * const p_camera, Renderer::Renderer * const p_renderer, Scene * const p_scene );
		void _drawCamera( Camera * const p_camera ) const;
		void _drawRenderer( Renderer::Renderer * const p_renderer );
		void _drawDurations( Renderer::Renderer * const p_renderer ) const;
		void _drawScene( Scene * const p_scene, Renderer::Renderer * const p_renderer );
		void _drawUniforms() const;
		void _drawNodeEditor( Renderer::Renderer * const p_renderer ) const;

		template<typename T>
		void _drawWidget(
			Renderer::Renderer * const p_renderer,
			const Renderer::Uniform &  p_uniform,
			const std::string &		   p_key,
			const bool				   p_isEditable
		) const
		{
			using namespace Renderer;
			const StructUniformValue<T> descValue = std::get<StructUniformValue<T>>( p_uniform.value );

			static std::map<std::string, T> values;
			if ( values.find( p_key ) == values.end() )
			{
				values.emplace( p_key, descValue.value );
			}
			T & value = values[ p_key ];

			bool updated = false;

			if constexpr ( std::is_same<T, bool>::value )
			{
				if ( ImGui::Checkbox( p_uniform.name.c_str(), &value ) )
					updated = true;
			}
			else if constexpr ( is_vec2i<T>::value )
			{
				if ( ImGui::InputInt2( p_uniform.name.c_str(), (int *)&value ) )
					updated = true;
			}
			else if constexpr ( is_vec2f<T>::value )
			{
				if ( ImGui::InputFloat2( p_uniform.name.c_str(), (float *)&value ) )
					updated = true;
			}
			else if constexpr ( is_color4<T>::value )
			{
				if ( ImGui::ColorEdit4( p_uniform.name.c_str(), (float *)( &value ) ) )
					updated = true;
			}
			else if ( descValue.minMax.has_value() )
			{
				const typename StructUniformValue<T>::MinMax & minMax = descValue.minMax.value();

				if constexpr ( std::is_integral<T>::value )
				{
					if ( ImGui::SliderInt( p_uniform.name.c_str(), (int *)( &value ), minMax.min, minMax.max ) )
						updated = true;
				}
				else if constexpr ( std::is_floating_point<T>::value )
				{
					if ( ImGui::SliderFloat( p_uniform.name.c_str(), (float *)( &value ), minMax.min, minMax.max ) )
						updated = true;
				}
			}
			else
			{
				if constexpr ( std::is_integral<T>::value )
				{
					if ( ImGui::DragInt( p_uniform.name.c_str(), (int *)( &value ) ) )
						updated = true;
				}
				else if constexpr ( std::is_floating_point<T>::value )
				{
					if ( ImGui::DragFloat( p_uniform.name.c_str(), (float *)( &value ) ) )
						updated = true;
				}
			}

			if ( p_isEditable && updated )
			{
				p_renderer->setValue( value, p_key );
			}
		}

	}; // namespace VTX::Bench
} // namespace VTX::Bench
#endif
