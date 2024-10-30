#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "user_interface.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include <core/chemdb/color.hpp>
#include <imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imnodes/imnodes.h>
#include <util/image.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::Bench
{

	UserInterface::UserInterface( const size_t p_width, const size_t p_height )
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
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, int( Renderer::Renderer::BUFFER_COUNT == 2 ) );
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

		SDL_EventState( SDL_DROPFILE, SDL_ENABLE );

		// Init ImGui.
		if ( not IMGUI_CHECKVERSION() )
		{
			throw std::runtime_error( "IMGUI_CHECKVERSION() failed" );
		}

		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		if ( not ImGui_ImplSDL2_InitForOpenGL( _window, _glContext ) )
		{
			throw std::runtime_error( "ImGui_ImplSDL2_InitForOpenGL failed" );
		}
		if ( not ImGui_ImplOpenGL3_Init( "#version 450 core" ) )
		{
			throw std::runtime_error( "ImGui_ImplOpenGL3_Init failed" );
		}

		// ImGui plugins.
		ImNodes::CreateContext();
	}

	UserInterface::~UserInterface()
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

	void UserInterface::draw( Camera * const p_camera, Scene * const p_scene, Renderer::Renderer * const p_renderer )
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// Menu bar.
		_drawMenuBar( p_camera, p_renderer, p_scene );

		if ( _drawUi )
		{
			// Camera.
			_drawCamera( p_camera );

			// Scene.
			_drawRenderer( p_renderer );

			// Times.
			_drawDurations( p_renderer );

			// Scene.
			_drawScene( p_scene, p_renderer );

			// Node editor.
			_drawNodeEditor( p_renderer );
		}

		// Render.
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
		SDL_GL_SwapWindow( _window );
	}

	void UserInterface::_drawMenuBar(
		Camera * const			   p_camera,
		Renderer::Renderer * const p_renderer,
		Scene * const			   p_scene
	)
	{
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
			if ( ImGui::BeginMenu( "Color layout" ) )
			{
				if ( ImGui::MenuItem( "JMol" ) )
				{
					p_scene->setColorLayout( VTX::Core::ChemDB::Color::COLOR_LAYOUT_JMOL );
				}
				if ( ImGui::MenuItem( "Random" ) )
				{
					VTX::Core::Struct::ColorLayout colorLayout;
					std::generate(
						colorLayout.layout.begin(), colorLayout.layout.end(), [] { return Util::Color::Rgba::random(); }
					);
					p_scene->setColorLayout( colorLayout );
				}
				if ( ImGui::MenuItem( "Random pastel" ) )
				{
					VTX::Core::Struct::ColorLayout colorLayout;
					std::generate(
						colorLayout.layout.begin(),
						colorLayout.layout.end(),
						[] { return Util::Color::Rgba::randomPastel(); }
					);
					p_scene->setColorLayout( colorLayout );
				}

				ImGui::EndMenu();
			}
			if ( ImGui::BeginMenu( "Export" ) )
			{
				auto snapshotFunc = [ & ]( const size_t p_width, const size_t p_height )
				{
					std::vector<uchar> image;
					p_renderer->snapshot(
						image, p_width, p_height, p_camera->getFov(), p_camera->getNear(), p_camera->getFar()
					);

					Util::Image::write( "snapshot", Util::Image::E_FORMAT::PNG, p_width, p_height, image.data() );
				};

				if ( ImGui::MenuItem( "800x600" ) )
				{
					snapshotFunc( 800, 600 );
				}
				if ( ImGui::MenuItem( "1920x1080" ) )
				{
					snapshotFunc( 1920, 1080 );
				}
				if ( ImGui::MenuItem( "2560x1440" ) )
				{
					snapshotFunc( 2560, 1440 );
				}
				if ( ImGui::MenuItem( "3840x2160" ) )
				{
					snapshotFunc( 3840, 2160 );
				}

				ImGui::EndMenu();
			}

			ImGui::Checkbox( "UI", &_drawUi );
			if ( ImGui::Checkbox( "Vsync", &_vsync ) )
			{
				setVSync( _vsync );
			}

			ImGui::Checkbox( "Timers", &p_renderer->logDurations );

			ImGui::Checkbox( "Force update", &p_renderer->forceUpdate );

			ImGui::Text( fmt::format( "{} FPS", int( ImGui::GetIO().Framerate ) ).c_str() );

			ImGui::EndMainMenuBar();
		}
	}

	void UserInterface::_drawCamera( Camera * const p_camera ) const
	{
		if ( ImGui::Begin( "Camera" ) )
		{
			float cameraNear		  = p_camera->getNear();
			float cameraFar			  = p_camera->getFar();
			float fov				  = p_camera->getFov();
			float velocityTranslation = p_camera->getVelocityTranslation();
			float velocityRotation	  = p_camera->getVelocityRotation();
			float velocityZoom		  = p_camera->getVelocityZoom();
			// static bool	 isPerspective = true;

			// ImGui::Checkbox( "Perspective", &isPerspective );
			if ( ImGui::SliderFloat( "Near", &cameraNear, Camera::NEAR_MIN, Camera::NEAR_MAX ) )
			{
				p_camera->setNear( cameraNear );
			}
			if ( ImGui::SliderFloat( "Far", &cameraFar, Camera::FAR_MIN, Camera::FAR_MAX ) )
			{
				p_camera->setFar( cameraFar );
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

	void UserInterface::_drawRenderer( Renderer::Renderer * const p_renderer )
	{
		if ( ImGui::Begin( "Renderer" ) && p_renderer->hasContext() )
		{
			size_t sizeAtoms = 0, sizeBonds = 0, sizeRibbons = 0, sizeVoxels = 0;
			for ( auto count : p_renderer->drawRangeSpheres.counts )
			{
				sizeAtoms += count;
			}
			for ( auto count : p_renderer->drawRangeCylinders.counts )
			{
				sizeBonds += count;
			}
			for ( auto count : p_renderer->drawRangeRibbons.counts )
			{
				sizeRibbons += count;
			}
			for ( auto count : p_renderer->drawRangeVoxels.counts )
			{
				sizeVoxels += count;
			}

			ImGui::Checkbox( fmt::format( "{} atoms", sizeAtoms ).c_str(), &p_renderer->showAtoms );
			ImGui::Checkbox( fmt::format( "{} bonds", sizeBonds ).c_str(), &p_renderer->showBonds );
			ImGui::Checkbox( fmt::format( "{} ribbons", sizeRibbons ).c_str(), &p_renderer->showRibbons );
			ImGui::Checkbox( fmt::format( "{} voxels", sizeVoxels ).c_str(), &p_renderer->showVoxels );

			ImGui::Text( fmt::format( "{}x{}", p_renderer->width, p_renderer->height ).c_str() );
			ImGui::Text( fmt::format( "{} FPS", int( ImGui::GetIO().Framerate ) ).c_str() );

			const Renderer::StructInfos & infos = p_renderer->getInfos();
			ImGui::ProgressBar(
				float( ( infos.gpuMemoryInfoTotalAvailable - infos.gpuMemoryInfoCurrentAvailable ) )
					/ infos.gpuMemoryInfoTotalAvailable,
				ImVec2( 0.f, 0.f ),
				fmt::format(
					"{} / {}",
					Util::String::memSizeToStr(
						infos.gpuMemoryInfoTotalAvailable - infos.gpuMemoryInfoCurrentAvailable
					),
					Util::String::memSizeToStr( infos.gpuMemoryInfoTotalAvailable )
				)
					.c_str()
			);

			ImGui::Text( fmt::format(
							 "Buffers: {} ({})",
							 Util::String::memSizeToStr( infos.currentSizeBuffers ),
							 infos.currentCountBuffers
			)
							 .c_str() );
			ImGui::Text( fmt::format(
							 "Textures: {} ({})",
							 Util::String::memSizeToStr( infos.currentSizeTextures ),
							 infos.currentCountTextures
			)
							 .c_str() );
			ImGui::Text( fmt::format( "CPU cache: {}", Util::String::memSizeToStr( infos.currentSizeCPUCache ) ).c_str()
			);
		}
		ImGui::End();
	}

	void UserInterface::_drawDurations( Renderer::Renderer * const p_renderer ) const
	{
		using namespace Renderer;

		if ( p_renderer->logDurations )
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

	void UserInterface::_drawScene( Scene * const p_scene, Renderer::Renderer * const p_renderer )
	{
		/*
		static const uint64_t sdlFrequency	= SDL_GetPerformanceFrequency();
		static uint64_t		  lastTime		= 0;
		const uint64_t		  now			= SDL_GetPerformanceCounter();
		const float			  deltaTime		= float( double( now - lastTime ) / sdlFrequency );
		lastTime							= now;
		*/

		if ( ImGui::Begin( "Scene" ) )
		{
			ImGui::Checkbox( "Update", &( p_scene->isUpdate ) );
			ImGui::SameLine();
			if ( ImGui::Button( "X all" ) )
			{
				p_scene->removeAllSystems( p_renderer );
			}

			size_t idSystem = 0;
			int	   toDelete	  = -1;

			for ( auto & proxySystem : p_scene->getProxiesSystems() )
			{
				// Display transform.
				if ( ImGui::TreeNode( fmt::format( "System ({})", idSystem ).c_str() ) )
				{
					// Display transform.
					Mat4f transform = *proxySystem->transform;
					ImGui::Text( "Transform" );
					ImGui::InputFloat4( "", &transform[ 0 ][ 0 ] );
					ImGui::InputFloat4( "", &transform[ 1 ][ 0 ] );
					ImGui::InputFloat4( "", &transform[ 2 ][ 0 ] );
					ImGui::InputFloat4( "", &transform[ 3 ][ 0 ] );

					if ( ImGui::Button( "S" ) )
					{
						proxySystem->onSelect( true );
					}
					ImGui::SameLine();
					if ( ImGui::Button( "US" ) )
					{
						proxySystem->onSelect( false );
					}
					ImGui::SameLine();
					if ( ImGui::Button( "UV" ) )
					{
						proxySystem->onVisible( false );
					}
					ImGui::SameLine();
					if ( ImGui::Button( "V" ) )
					{
						proxySystem->onVisible( true );
					}
					ImGui::SameLine();
					if ( ImGui::Button( "Rep" ) )
					{
						proxySystem->onRepresentation( rand() % 3 );
					}
					ImGui::SameLine();
					if ( ImGui::Button( "X" ) )
					{
						// Don't remove from proxy directly, remove from scene before (after loop).
						toDelete = int( idSystem );
					}

					ImGui::TreePop();
				}
				idSystem++;
			}

			if ( toDelete != -1 )
			{
				p_scene->removeSystem( toDelete );
			}
		}
		ImGui::End();
	}

	void UserInterface::_drawUniforms() const {}

	void UserInterface::_drawNodeEditor( Renderer::Renderer * const p_renderer ) const
	{
		using namespace Renderer;
		auto & graph = p_renderer->getRenderGraph();

		static bool isOpen = true;
		if ( ImGui::Begin( "Render graph", &isOpen, ImGuiWindowFlags_MenuBar ) )
		{
			ImGui::BeginMenuBar();
			if ( ImGui::BeginMenu( "Add" ) )
			{
				for ( const Pass * const pass : p_renderer->getAvailablePasses() )
				{
					if ( ImGui::MenuItem( pass->name.c_str() ) )
					{
						graph.addPass( *pass );
					}
				}

				ImGui::EndMenu();
			}
			if ( ImGui::Button( "Build" ) )
			{
				try
				{
					p_renderer->build();
				}
				catch ( const std::exception & e )
				{
					VTX_ERROR( "{}", e.what() );
				}
			}
			if ( ImGui::Button( "X" ) )
			{
				p_renderer->clean();
			}

			RenderQueue & renderQueue = graph.getRenderQueue();
			for ( const Pass * const pass : renderQueue )
			{
				if ( pass != nullptr )
				{
					ImGui::TextUnformatted( pass->name.c_str() );
				}
				else
				{
					ImGui::TextUnformatted( " [deleted] " );
				}
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

			bool isBuilt = p_renderer->hasContext();

			// DescPass nodes.
			uint								   id = 0;
			std::map<const Input * const, uint>	   mapIdInput;
			std::map<const Output * const, uint>   mapIdOutput;
			std::map<const uint, const E_CHAN_OUT> mapIdChannelOutput;
			std::map<const uint, const E_CHAN_IN>  mapIdChannelInput;
			std::map<const uint, Pass *>		   mapIdDescPass;
			std::map<const uint, Link *>		   mapIdDescLink;
			const Pass *						   passToDelete = nullptr;

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
							_drawWidget<bool>( p_renderer, uniform, key, isEditable );
							break;
						}
						case E_TYPE::BYTE:
						{
							_drawWidget<char>( p_renderer, uniform, key, isEditable );
							break;
						}
						case E_TYPE::UBYTE:
						{
							_drawWidget<uchar>( p_renderer, uniform, key, isEditable );
							break;
						}
						case E_TYPE::SHORT:
						{
							_drawWidget<short>( p_renderer, uniform, key, isEditable );
							break;
						}
						case E_TYPE::USHORT:
						{
							_drawWidget<ushort>( p_renderer, uniform, key, isEditable );
							break;
						}
						case E_TYPE::INT:
						{
							_drawWidget<int>( p_renderer, uniform, key, isEditable );
							break;
						}
						case E_TYPE::UINT:
						{
							_drawWidget<uint>( p_renderer, uniform, key, isEditable );
							break;
						}
						case E_TYPE::FLOAT:
						{
							_drawWidget<float>( p_renderer, uniform, key, isEditable );
							break;
						}
						case E_TYPE::VEC2I:
						{
							_drawWidget<Vec2i>( p_renderer, uniform, key, isEditable );
							break;
						}
						case E_TYPE::VEC2F:
						{
							_drawWidget<Vec2f>( p_renderer, uniform, key, isEditable );
							break;
						}
						case E_TYPE::COLOR4:
						{
							_drawWidget<Util::Color::Rgba>( p_renderer, uniform, key, isEditable );
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
				graph.removePass( passToDelete );
				passToDelete = nullptr;
			}
		}
		ImGui::End();
	}

} // namespace VTX::Bench
