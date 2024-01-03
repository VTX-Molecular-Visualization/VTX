#ifndef __VTX_BENCH_USER_INTERFACE__
#define __VTX_BENCH_USER_INTERFACE__

#include "camera.hpp"
#include <SDL.h>
#include <imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imnodes/imnodes.h>
#include <renderer/renderer.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::Bench
{
	class UserInterface
	{
	  public:
		UserInterface( const size_t p_width, const size_t p_height )
		{
			// Init SDL2.
			if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) != 0 )
			{
				throw std::runtime_error( "Failed to init SDL: " + std::string( SDL_GetError() ) );
			}

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
			SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
			// SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
			// SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

			_window = SDL_CreateWindow(
				"VTX_RENDERER_BENCH",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				int( p_width ),
				int( p_height ),
				SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
			);

			if ( _window == nullptr )
			{
				throw std::runtime_error( "Failed to create SDL2 window" );
			}

			_glContext = SDL_GL_CreateContext( _window );
			if ( _glContext == nullptr )
			{
				throw std::runtime_error( SDL_GetError() );
			}

			SDL_GL_MakeCurrent( _window, _glContext );
			SDL_GL_SetSwapInterval( _vsync );

			// Init ImGui.
			if ( IMGUI_CHECKVERSION() == false )
			{
				throw std::runtime_error( "IMGUI_CHECKVERSION() failed" );
			}

			ImGui::CreateContext();
			ImGui::StyleColorsDark();

			if ( ImGui_ImplSDL2_InitForOpenGL( _window, _glContext ) == false )
			{
				throw std::runtime_error( "ImGui_ImplSDL2_InitForOpenGL failed" );
			}
			if ( ImGui_ImplOpenGL3_Init( "#version 450 core" ) == false )
			{
				throw std::runtime_error( "ImGui_ImplOpenGL3_Init failed" );
			}

			// ImGui plugins.
			ImNodes::CreateContext();
		}

		~UserInterface()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			if ( ImGui::GetCurrentContext() != nullptr )
			{
				ImNodes::DestroyContext();
				ImGui::DestroyContext();
			}
			if ( _glContext )
			{
				SDL_GL_DeleteContext( _glContext );
			}
			if ( _window )
			{
				SDL_DestroyWindow( _window );
			}
			SDL_Quit();
		}

		inline double getTime() const { return SDL_GetTicks(); }
		inline float  getDeltaTime() const { return ImGui::GetIO().DeltaTime; }
		inline void * getProcAddress() { return reinterpret_cast<void *>( SDL_GL_GetProcAddress ); }
		inline void	  setVSync( const bool p_vsync )
		{
			_vsync = p_vsync;
			SDL_GL_SetSwapInterval( _vsync );
		}

		void draw( Camera * const p_camera, Renderer::Renderer * const p_renderer )
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			// Menu bar.
			if ( ImGui::BeginMainMenuBar() )
			{
				if ( ImGui::BeginMenu( "Shaders" ) )
				{
					if ( ImGui::MenuItem( "Compile" ) )
					{
						p_renderer->compileShaders();
					}

					ImGui::EndMenu();
				}

				if ( ImGui::BeginMenu( "Resolution" ) )
				{
					if ( ImGui::MenuItem( "800x600" ) )
					{
						SDL_SetWindowSize( _window, 800, 600 );
					}
					if ( ImGui::MenuItem( "1920x1080" ) )
					{
						SDL_SetWindowSize( _window, 1920, 1080 );
					}
					if ( ImGui::MenuItem( "2560x1440" ) )
					{
						SDL_SetWindowSize( _window, 2560, 1440 );
					}
					if ( ImGui::MenuItem( "3840x2160" ) )
					{
						SDL_SetWindowSize( _window, 3840, 2160 );
					}

					ImGui::EndMenu();
				}

				ImGui::Checkbox( "UI", &_drawUi );
				if ( ImGui::Checkbox( "Vsync", &_vsync ) )
				{
					setVSync( _vsync );
				}

				bool isLogDurations = p_renderer->isLogDurations();
				if ( ImGui::Checkbox( "Timers", &isLogDurations ) )
				{
					p_renderer->setLogDurations( isLogDurations );
				}
				ImGui::Text( fmt::format( "{} FPS", int( ImGui::GetIO().Framerate ) ).c_str() );

				ImGui::EndMainMenuBar();
			}

			if ( _drawUi )
			{
				// Camera.
				_drawCamera( p_camera );

				// Times.
				_drawDurations( p_renderer );

				// Misc.
				_drawMisc( p_renderer );

				// Node editor.
				_drawNodeEditor( p_renderer );
			}

			// Render.
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
			SDL_GL_SwapWindow( _window );
		}

		void _drawCamera( Camera * const p_camera ) const
		{
			if ( ImGui::Begin( "Camera" ) )
			{
				float near				  = p_camera->getNear();
				float far				  = p_camera->getFar();
				float fov				  = p_camera->getFov();
				float velocityTranslation = p_camera->getVelocityTranslation();
				float velocityRotation	  = p_camera->getVelocityRotation();
				float velocityZoom		  = p_camera->getVelocityZoom();
				// static bool	 isPerspective = true;

				// ImGui::Checkbox( "Perspective", &isPerspective );
				if ( ImGui::SliderFloat( "Near", &near, Camera::NEAR_MIN, Camera::NEAR_MAX ) )
				{
					p_camera->setNear( near );
				}
				if ( ImGui::SliderFloat( "Far", &far, Camera::FAR_MIN, Camera::FAR_MAX ) )
				{
					p_camera->setFar( far );
				}
				if ( ImGui::SliderFloat( "Fov", &fov, Camera::FOV_MIN, Camera::FOV_MAX ) )
				{
					p_camera->setFov( fov );
				}
				if ( ImGui::SliderFloat(
						 "Velocity translation",
						 &velocityTranslation,
						 Camera::VELOCITY_TRANSLATION_MIN,
						 Camera::VELOCITY_TRANSLATION_MAX
					 ) )
				{
					p_camera->setVelocityTranslation( velocityTranslation );
				}
				if ( ImGui::SliderFloat(
						 "Velocity rotation",
						 &velocityRotation,
						 Camera::VELOCITY_ROTATION_MIN,
						 Camera::VELOCITY_ROTATION_MAX
					 ) )
				{
					p_camera->setVelocityRotation( velocityRotation );
				}
				if ( ImGui::SliderFloat(
						 "Velocity zoom", &velocityZoom, Camera::VELOCITY_ZOOM_MIN, Camera::VELOCITY_ZOOM_MAX
					 ) )
				{
					p_camera->setVelocityZoom( velocityZoom );
				}
			}
			ImGui::End();
		}

		void _drawDurations( Renderer::Renderer * const p_renderer ) const
		{
			using namespace Renderer;

			if ( p_renderer->isLogDurations() )
			{
				const InstructionsDurationRanges & durations = p_renderer->getInstructionsDurationRanges();

				if ( ImGui::Begin( "Durations (ms)" ) )
				{
					auto max = std::max_element(
						durations.begin(),
						durations.end(),
						[]( const InstructionsDurationRange & p_lhs, const InstructionsDurationRange & p_rhs )
						{ return p_lhs.duration < p_rhs.duration; }
					);

					for ( size_t i = 0; i < durations.size(); ++i )
					{
						ImGui::ProgressBar(
							durations[ i ].duration / max->duration,
							ImVec2( 0.f, 0.f ),
							std::to_string( durations[ i ].duration ).c_str()
						);
						ImGui::SameLine( 0.0f, ImGui::GetStyle().ItemInnerSpacing.x );
						ImGui::Text( durations[ i ].name.c_str() );
					}
				}
				ImGui::End();
			}
		}

		void _drawMisc( Renderer::Renderer * const p_renderer )
		{
			/*
			static const uint64_t sdlFrequency	= SDL_GetPerformanceFrequency();
			static uint64_t		  lastTime		= 0;
			const uint64_t		  now			= SDL_GetPerformanceCounter();
			const float			  deltaTime		= float( double( now - lastTime ) / sdlFrequency );
			lastTime							= now;
			*/
			const Renderer::StructInfos & infos = p_renderer->getInfos();

			if ( ImGui::Begin( "Misc" ) )
			{
				// ImGui::Checkbox( "Perspective", &isPerspective );

				ImGui::Text( fmt::format( "{} atoms", p_renderer->getAtomCount() ).c_str() );
				ImGui::Text( fmt::format( "{} bonds", p_renderer->getBondCount() ).c_str() );
				// ImGui::Text( fmt::format( "{} FPS", int( 1.f / deltaTime ) ).c_str() );
				ImGui::Text( fmt::format( "{}x{}", p_renderer->getWidth(), p_renderer->getHeight() ).c_str() );
				ImGui::Text( fmt::format( "{} FPS", int( ImGui::GetIO().Framerate ) ).c_str() );

				ImGui::ProgressBar(
					float( ( infos.gpuMemoryInfoTotalAvailable - infos.gpuMemoryInfoCurrentAvailable ) )
						/ infos.gpuMemoryInfoTotalAvailable,
					ImVec2( 0.f, 0.f ),
					fmt::format(
						"{} / {}",
						( infos.gpuMemoryInfoTotalAvailable - infos.gpuMemoryInfoCurrentAvailable ),
						infos.gpuMemoryInfoTotalAvailable
					)
						.c_str()
				);
				ImGui::SameLine( 0.0f, ImGui::GetStyle().ItemInnerSpacing.x );
				ImGui::Text( "GPU memory" );
			}
			ImGui::End();
		}

		void _drawNodeEditor( Renderer::Renderer * const p_renderer ) const
		{
			using namespace Renderer;
			auto & graph = p_renderer->getRenderGraph();

			static bool isOpen = true;
			if ( ImGui::Begin( "Render graph", &isOpen, ImGuiWindowFlags_MenuBar ) )
			{
				ImGui::BeginMenuBar();
				if ( ImGui::BeginMenu( "Add" ) )
				{
					for ( const Pass & pass : graph.getAvailablePasses() )
					{
						if ( ImGui::MenuItem( pass.name.c_str() ) )
						{
							graph.addPass( pass );
						}
					}

					ImGui::EndMenu();
				}
				if ( ImGui::Button( "Build" ) )
				{
					p_renderer->build();
				}
				if ( ImGui::Button( "Clean" ) )
				{
					p_renderer->clean();
				}

				RenderQueue & renderQueue = graph.getRenderQueue();
				for ( const Pass * const pass : renderQueue )
				{
					ImGui::TextUnformatted( pass->name.c_str() );
					ImGui::TextUnformatted( " -> " );
				}
				if ( renderQueue.empty() )
				{
					ImGui::TextUnformatted( "<not built>" );
				}
				else
				{
					ImGui::TextUnformatted( "Output" );
				}
				ImGui::EndMenuBar();

				ImNodes::BeginNodeEditor();

				bool isBuilt = graph.isBuilt();

				// DescPass nodes.
				uint										 id = 0;
				std::map<const Input * const, uint>			 mapIdInput;
				std::map<const Output * const, uint>		 mapIdOutput;
				std::map<const uint, const E_CHANNEL_OUTPUT> mapIdChannelOutput;
				std::map<const uint, const E_CHANNEL_INPUT>	 mapIdChannelInput;
				std::map<const uint, Pass *>				 mapIdDescPass;
				std::map<const uint, Link *>				 mapIdDescLink;
				const Pass *								 passToDelete = nullptr;

				for ( std::unique_ptr<Pass> & pass : graph.getPasses() )
				{
					ImNodes::BeginNode( id++ );
					ImNodes::BeginNodeTitleBar();

					if ( ImGui::Button( "X", ImVec2( 15, 15 ) ) )
					{
						passToDelete = pass.get();
					}

					ImGui::SameLine();
					ImGui::TextUnformatted( pass->name.c_str() );
					ImNodes::EndNodeTitleBar();

					bool isInRenderQueue = graph.isInRenderQueue( pass.get() );

					// Inputs.
					for ( const auto & [ channel, input ] : pass->inputs )
					{
						if ( std::holds_alternative<Attachment>( input.desc ) )
						{
							const Attachment & attachment = std::get<Attachment>( input.desc );
							mapIdInput.emplace( &input, id );
							mapIdChannelInput.emplace( id, channel );
							mapIdDescPass.emplace( id, pass.get() );
							ImNodes::PushAttributeFlag( ImNodesAttributeFlags_EnableLinkDetachWithDragClick );
							ImNodes::BeginInputAttribute( id++ );
							ImGui::Text(
								fmt::format( "{} ({})", input.name, Util::Enum::enumName( attachment.format ) ).c_str()
							);
							ImNodes::EndInputAttribute();
							ImNodes::PopAttributeFlag();
						}
						else if ( std::holds_alternative<Data>( input.desc ) )
						{
							// const Data & data = std::get<Data>( input.desc );
							ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1, 1, 0, 1 ) );
							ImGui::Text( input.name.c_str() );
							ImGui::PopStyleColor();
						}
					}

					// Outputs.
					for ( const auto & [ channel, output ] : pass->outputs )
					{
						const Attachment & attachment = std::get<Attachment>( output.desc );
						mapIdOutput.emplace( &output, id );
						mapIdChannelOutput.emplace( id, channel );
						mapIdDescPass.emplace( id, pass.get() );
						ImNodes::BeginOutputAttribute( id++ );
						ImGui::Text(
							fmt::format( "({}) {}", Util::Enum::enumName( attachment.format ), output.name ).c_str()
						);
						ImNodes::EndOutputAttribute();
					}

					// Programs.
					for ( const Program & program : pass->programs )
					{
						// Uniforms.
						for ( const Uniform & uniform : program.uniforms )
						{
							std::string key		   = pass->name + program.name + uniform.name;
							bool		isEditable = isBuilt && isInRenderQueue;

							// ImGui::Text( uniform.name.c_str() );
							ImGui::SetNextItemWidth( 150 );
							switch ( uniform.type )
							{
							case E_TYPE::BOOL:
							{
								StructUniformValue<bool> descValue
									= std::get<StructUniformValue<bool>>( uniform.value );

								bool value;
								if ( isEditable )
								{
									p_renderer->getUniform<bool>( value, key );
								}
								else
								{
									value = descValue.value;
								}

								if ( ImGui::Checkbox( uniform.name.c_str(), &value ) )
								{
									if ( isEditable )
										p_renderer->setUniform( value, key );
								}

								break;
							}
							case E_TYPE::INT:
							{
								StructUniformValue<int> descValue = std::get<StructUniformValue<int>>( uniform.value );

								int value;
								if ( isEditable )
								{
									p_renderer->getUniform<int>( value, key );
								}
								else
								{
									value = descValue.value;
								}

								if ( descValue.minMax.has_value() )
								{
									StructUniformValue<int>::MinMax & minMax = descValue.minMax.value();
									if ( ImGui::SliderInt( uniform.name.c_str(), &value, minMax.min, minMax.max ) )
									{
										if ( isEditable )
											p_renderer->setUniform( value, key );
									}
								}
								else
								{
									if ( ImGui::DragInt( uniform.name.c_str(), (int *)( &value ) ) )
									{
										if ( isEditable )
											p_renderer->setUniform( value, key );
									}
								}
								break;
							}
							case E_TYPE::UINT:
							{
								StructUniformValue<uint> descValue
									= std::get<StructUniformValue<uint>>( uniform.value );

								uint value;
								if ( isEditable )
								{
									p_renderer->getUniform<uint>( value, key );
								}
								else
								{
									value = descValue.value;
								}

								if ( descValue.minMax.has_value() )
								{
									StructUniformValue<uint>::MinMax & minMax = descValue.minMax.value();
									if ( ImGui::SliderInt(
											 uniform.name.c_str(), (int *)( &value ), minMax.min, minMax.max
										 ) )
									{
										if ( isEditable )
											p_renderer->setUniform( value, key );
									}
								}
								else
								{
									if ( ImGui::DragInt( uniform.name.c_str(), (int *)( &value ) ) )
									{
										if ( isEditable )
											p_renderer->setUniform( value, key );
									}
								}
								break;
							}
							case E_TYPE::FLOAT:
							{
								StructUniformValue<float> descValue
									= std::get<StructUniformValue<float>>( uniform.value );

								float value;
								if ( isEditable )
								{
									p_renderer->getUniform<float>( value, key );
								}
								else
								{
									value = descValue.value;
								}

								if ( descValue.minMax.has_value() )
								{
									StructUniformValue<float>::MinMax & minMax = descValue.minMax.value();
									if ( ImGui::SliderFloat( uniform.name.c_str(), &value, minMax.min, minMax.max ) )
									{
										if ( isEditable )
											p_renderer->setUniform( value, key );
									}
								}
								else
								{
									if ( ImGui::InputFloat( uniform.name.c_str(), &value ) )
									{
										if ( isEditable )
											p_renderer->setUniform( value, key );
									}
								}
								break;
							}
							case E_TYPE::VEC2I:
							{
								StructUniformValue<Vec2i> descValue
									= std::get<StructUniformValue<Vec2i>>( uniform.value );

								Vec2i value;
								if ( isEditable )
								{
									p_renderer->getUniform<Vec2i>( value, key );
								}
								else
								{
									value = descValue.value;
								}

								if ( ImGui::InputInt2( uniform.name.c_str(), (int *)&value ) )
								{
									if ( isEditable )
										p_renderer->setUniform( value, key );
								}
								break;
							}
							case E_TYPE::VEC2F:
							{
								StructUniformValue<Vec2f> descValue
									= std::get<StructUniformValue<Vec2f>>( uniform.value );

								Vec2f value;
								if ( isEditable )
								{
									p_renderer->getUniform<Vec2f>( value, key );
								}
								else
								{
									value = descValue.value;
								}

								if ( ImGui::InputFloat2( uniform.name.c_str(), (float *)&value ) )
								{
									if ( isEditable )
										p_renderer->setUniform( value, key );
								}
								break;
							}
							case E_TYPE::COLOR4:
							{
								StructUniformValue<Util::Color::Rgba> descValue
									= std::get<StructUniformValue<Util::Color::Rgba>>( uniform.value );

								Util::Color::Rgba value;
								if ( isEditable )
								{
									p_renderer->getUniform<Util::Color::Rgba>( value, key );
								}
								else
								{
									value = descValue.value;
								}

								if ( ImGui::ColorEdit4( uniform.name.c_str(), (float *)( &value ) ) )
								{
									if ( isEditable )
										p_renderer->setUniform( value, key );
								}
								break;
							}
							default: throw std::runtime_error( "widget not implemented" ); break;
							}
						}
					}

					ImNodes::EndNode();
				}

				// Final output node.
				ImNodes::PushColorStyle( ImNodesCol_TitleBar, IM_COL32( 133, 78, 27, 255 ) );
				ImNodes::BeginNode( id++ );
				ImNodes::BeginNodeTitleBar();
				ImGui::TextUnformatted( "Ouput" );
				ImNodes::EndNodeTitleBar();
				ImNodes::PushAttributeFlag( ImNodesAttributeFlags_EnableLinkDetachWithDragClick );
				uint idFinalOuput = id;
				ImNodes::BeginInputAttribute( id++ );
				ImGui::Text( "out" );
				ImNodes::EndInputAttribute();
				ImNodes::PopAttributeFlag();

				ImNodes::EndNode();
				ImNodes::PopColorStyle();

				// Links.
				for ( std::unique_ptr<Link> & link : graph.getLinks() )
				{
					mapIdDescLink.emplace( id, link.get() );
					ImNodes::Link(
						id++,
						mapIdOutput[ &( link->src->outputs[ link->channelSrc ] ) ],
						mapIdInput[ &( link->dest->inputs[ link->channelDest ] ) ]
					);
				}

				// Output.
				uint idFinalDescLink = -1;
				if ( graph.getOutput() )
				{
					idFinalDescLink = id;
					ImNodes::Link( id++, mapIdOutput[ graph.getOutput() ], idFinalOuput );
				}

				ImNodes::MiniMap();
				ImNodes::EndNodeEditor();

				// Check new links.
				int newLinkStartId, newLinkEndtId;
				if ( ImNodes::IsLinkCreated( &newLinkStartId, &newLinkEndtId ) )
				{
					// Output.
					if ( newLinkEndtId == idFinalOuput )
					{
						for ( auto & it : mapIdOutput )
						{
							if ( it.second == newLinkStartId )
							{
								graph.setOutput( it.first );
							}
						}
					}

					// Add link.
					else
					{
						graph.addLink(
							mapIdDescPass[ newLinkStartId ],
							mapIdDescPass[ newLinkEndtId ],
							mapIdChannelOutput[ newLinkStartId ],
							mapIdChannelInput[ newLinkEndtId ]
						);
					}
				}

				// Check deleted links.
				int deletedDescLinkId;
				if ( ImNodes::IsLinkDestroyed( &deletedDescLinkId ) )
				{
					// Output.
					if ( deletedDescLinkId == idFinalDescLink )
					{
						graph.setOutput( nullptr );
					}

					// DescLink.
					else
					{
						graph.removeLink( mapIdDescLink[ deletedDescLinkId ] );
					}
				}

				// Check deleted node.
				if ( passToDelete != nullptr )
				{
					p_renderer->clean();
					graph.removePass( passToDelete );
				}
			}
			ImGui::End();
		}

		bool getEvent( SDL_Event & p_event ) const
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

	}; // namespace VTX::Bench
} // namespace VTX::Bench
#endif
