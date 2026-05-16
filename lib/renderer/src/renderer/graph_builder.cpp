#include "renderer/graph_builder.hpp"
#include <util/enum.hpp>
#include <util/exceptions.hpp>

namespace VTX::Renderer
{
	namespace
	{
		void _requirePassType(
			const Desc::Pass &		p_pass,
			const Desc::E_PASS_TYPE p_expected,
			const std::string_view	p_action
		)
		{
			if ( p_pass.type == p_expected )
			{
				return;
			}

			throw GraphicException(
				"Cannot call '{}' in pass '{}': expected pass type {}, got {}",
				p_action,
				p_pass.name,
				Util::Enum::enumName( p_expected ),
				Util::Enum::enumName( p_pass.type )
			);
		}

		void _ensureNoProgramExecution( const Desc::Program & p_program, const std::string_view p_action )
		{
			if ( p_program.drawCall || p_program.dispatch || p_program.dispatchIndirect )
			{
				throw GraphicException(
					"Program '{}': cannot add '{}' because an execution command is already set",
					p_program.name,
					p_action
				);
			}
		}
	} // namespace

	GraphBuilder::GraphBuilder()
	{
		// Create default sampler.
		sampler(
			"Default",
			Desc::E_WRAPPING::CLAMP_TO_EDGE,
			Desc::E_WRAPPING::CLAMP_TO_EDGE,
			Desc::E_FILTERING::NEAREST,
			Desc::E_FILTERING::NEAREST
		);
	}

	GraphBuilder & GraphBuilder::texture(
		const Desc::Key &	 p_name,
		const Desc::E_FORMAT p_format,
		const Desc::Size2D & p_size
	)
	{
		resources.textures[ p_name ] = Desc::Texture { p_format, p_size };
		return *this;
	}

	GraphBuilder & GraphBuilder::sampler(
		const Desc::Key &		p_name,
		const Desc::E_WRAPPING	p_wrapS,
		const Desc::E_WRAPPING	p_wrapT,
		const Desc::E_FILTERING p_minFilter,
		const Desc::E_FILTERING p_magFilter
	)
	{
		resources.samplers[ p_name ] = Desc::Sampler { p_wrapS, p_wrapT, p_minFilter, p_magFilter };
		return *this;
	}

	GraphBuilder & GraphBuilder::vertexLayout(
		const Desc::Key &								   p_name,
		const std::initializer_list<Desc::VertexAttribute> p_attributes
	)
	{
		Desc::VertexLayout layout;
		layout.attributes.assign( p_attributes.begin(), p_attributes.end() );
		resources.vertexStreams[ p_name ] = std::move( layout );
		return *this;
	}

	GraphBuilder & GraphBuilder::vertexLayout( const Desc::Key & p_name, const Desc::VertexLayout & p_layout )
	{
		for ( auto & attr : p_layout.attributes )
		{
			if ( not resources.buffers.contains( attr.name ) )
			{
				buffer( attr.name, Desc::E_BUFFER_USAGE::VERTEX, Desc::E_UPDATE_FREQUENCY::STATIC );
			}
		}

		resources.vertexStreams[ p_name ] = p_layout;
		return *this;
	}

	GraphBuilder & GraphBuilder::buffer(
		const Desc::Key &								p_name,
		const Desc::E_BUFFER_USAGE						p_usage,
		const Desc::E_UPDATE_FREQUENCY					p_frequency,
		const Desc::E_BUFFER_MUTABILITY					p_mutability,
		const Desc::E_BUFFER_ACCESS						p_access,
		const std::optional<uint32_t>					p_binding,
		const std::initializer_list<Desc::UniformValue> p_values
	)
	{
		Desc::Buffer & desc = resources.buffers[ p_name ];
		desc.name			= p_name;
		desc.usage |= p_usage;
		desc.frequency	= p_frequency;
		desc.mutability = p_mutability;
		desc.access		= p_access;

		if ( p_binding )
		{
			desc.binding = *p_binding;
		}

		if ( p_values.size() > 0 )
		{
			desc.values.assign( p_values.begin(), p_values.end() );
		}

		return *this;
	}

	GraphBuilder & GraphBuilder::geometry(
		const Desc::Key &			   p_name,
		const Desc::Key &			   p_vertexLayout,
		const std::optional<Desc::Key> p_indiceBuffer,
		const std::optional<Desc::Key> p_indirectBuffer
	)
	{
		if ( p_indiceBuffer && not resources.buffers.contains( *p_indiceBuffer ) )
		{
			buffer( *p_indiceBuffer, Desc::E_BUFFER_USAGE::INDEX, Desc::E_UPDATE_FREQUENCY::DYNAMIC );
		}
		if ( p_indirectBuffer && not resources.buffers.contains( *p_indirectBuffer ) )
		{
			buffer( *p_indirectBuffer, Desc::E_BUFFER_USAGE::INDIRECT, Desc::E_UPDATE_FREQUENCY::DYNAMIC );
		}

		Desc::Geometry geom;
		geom.vertexLayout			   = p_vertexLayout;
		geom.indiceBuffer			   = p_indiceBuffer;
		geom.indirectBuffer			   = p_indirectBuffer;
		resources.geometries[ p_name ] = std::move( geom );
		return *this;
	}

	GraphBuilder & GraphBuilder::geometry( const Desc::Key & p_name, const Desc::Geometry & p_geometry )
	{
		if ( p_geometry.indiceBuffer && not resources.buffers.contains( *p_geometry.indiceBuffer ) )
		{
			buffer( *p_geometry.indiceBuffer, Desc::E_BUFFER_USAGE::INDEX, Desc::E_UPDATE_FREQUENCY::DYNAMIC );
		}
		if ( p_geometry.indirectBuffer && not resources.buffers.contains( *p_geometry.indirectBuffer ) )
		{
			buffer( *p_geometry.indirectBuffer, Desc::E_BUFFER_USAGE::INDIRECT, Desc::E_UPDATE_FREQUENCY::DYNAMIC );
		}

		resources.geometries[ p_name ] = p_geometry;
		return *this;
	}

	PassBuilder GraphBuilder::pass(
		const Desc::Key &			 p_name,
		const Desc::E_PASS_TYPE		 p_type,
		const Desc::E_PASS_EXECUTION p_execution
	)
	{
		return PassBuilder( *this, p_name, p_type, p_execution );
	}

	PassBuilder GraphBuilder::computePass( const Desc::Key & p_name, const Desc::E_PASS_EXECUTION p_execution )
	{
		return pass( p_name, Desc::E_PASS_TYPE::COMPUTE, p_execution );
	}

	PassBuilder GraphBuilder::externalPass( const Desc::Key & p_name, const Desc::E_PASS_EXECUTION p_execution )
	{
		return pass( p_name, Desc::E_PASS_TYPE::EXTERNAL, p_execution );
	}

	ProgramBuilder::ProgramBuilder( PassBuilder & p_p, Desc::Program & p_prog ) : parent( p_p ), program( p_prog ) {}

	ProgramBuilder & ProgramBuilder::shaders( std::initializer_list<FilePath> p_files )
	{
		program.shaders.emplace<std::vector<FilePath>>( p_files.begin(), p_files.end() );
		return *this;
	}

	ProgramBuilder & ProgramBuilder::shadersDir( const FilePath & p_dir )
	{
		program.shaders.emplace<FilePath>( p_dir );
		return *this;
	}

	ProgramBuilder & ProgramBuilder::uniform( const Desc::UniformValue & p_u )
	{
		program.uniforms.push_back( p_u );
		return *this;
	}

	ProgramBuilder & ProgramBuilder::draw( const Desc::Key & p_geometry, const Desc::E_PRIMITIVE p_primitive )
	{
		return draw( p_geometry, p_primitive, Desc::DrawCall::Indirect {} );
	}

	ProgramBuilder & ProgramBuilder::draw(
		const Desc::Key &					  p_geometry,
		const Desc::E_PRIMITIVE				  p_primitive,
		const Desc::DrawCall::RangesVariant & p_ranges
	)
	{
		_requirePassType( parent.pass, Desc::E_PASS_TYPE::GRAPHICS, "draw" );
		_ensureNoProgramExecution( program, "draw" );

		Desc::DrawCall dc;
		dc.geometry		 = p_geometry;
		dc.primitive	 = p_primitive;
		dc.ranges		 = p_ranges;
		program.drawCall = dc;
		return *this;
	}

	ProgramBuilder & ProgramBuilder::dispatch( const Desc::Dispatch & p_dispatch )
	{
		_requirePassType( parent.pass, Desc::E_PASS_TYPE::COMPUTE, "dispatch" );
		_ensureNoProgramExecution( program, "dispatch" );

		program.dispatch = p_dispatch;
		return *this;
	}

	ProgramBuilder & ProgramBuilder::dispatch(
		const uint32_t p_groupX,
		const uint32_t p_groupY,
		const uint32_t p_groupZ,
		const Desc::E_MEMORY_BARRIER p_barriers
	)
	{
		return dispatch(
			Desc::Dispatch { .groupX = p_groupX, .groupY = p_groupY, .groupZ = p_groupZ, .barriers = p_barriers }
		);
	}

	ProgramBuilder & ProgramBuilder::dispatchIndirect(
		const Desc::Key &			  p_indirectBuffer,
		const uint32_t				  p_offset,
		const Desc::E_MEMORY_BARRIER p_barriers
	)
	{
		_requirePassType( parent.pass, Desc::E_PASS_TYPE::COMPUTE, "dispatchIndirect" );
		_ensureNoProgramExecution( program, "dispatchIndirect" );

		if ( not parent.graph.resources.buffers.contains( p_indirectBuffer ) )
		{
			parent.graph.buffer( p_indirectBuffer, Desc::E_BUFFER_USAGE::INDIRECT, Desc::E_UPDATE_FREQUENCY::DYNAMIC );
		}

		program.dispatchIndirect
			= Desc::DispatchIndirect { .indirectBuffer = p_indirectBuffer, .offset = p_offset, .barriers = p_barriers };
		return *this;
	}

	PassBuilder & ProgramBuilder::endProgram() { return parent; }

	PassBuilder::PassBuilder(
		GraphBuilder &				 p_g,
		const Desc::Key &			 p_name,
		const Desc::E_PASS_TYPE		 p_type,
		const Desc::E_PASS_EXECUTION p_execution
	) : graph( p_g )
	{
		pass.name	   = p_name;
		pass.type	   = p_type;
		pass.execution = p_execution;
	}

	PassBuilder & PassBuilder::type( const Desc::E_PASS_TYPE p_type )
	{
		pass.type = p_type;
		return *this;
	}

	PassBuilder & PassBuilder::execution( const Desc::E_PASS_EXECUTION p_execution )
	{
		pass.execution = p_execution;
		return *this;
	}

	PassBuilder & PassBuilder::in(
		const Desc::E_RESOURCE_TYPE	   p_type,
		const Desc::Key &			   p_primary,
		const std::optional<Desc::Key> p_secondary
	)
	{
		pass.inputs.push_back( { p_type, p_primary, p_secondary } );
		return *this;
	}

	PassBuilder & PassBuilder::in( const Desc::Key & p_texture, const std::optional<Desc::Key> & p_sampler )
	{
		return in( Desc::E_RESOURCE_TYPE::TEXTURE, p_texture, p_sampler ? *p_sampler : DEFAULT_SAMPLER_NAME );
	}

	PassBuilder & PassBuilder::out(
		const Desc::E_RESOURCE_TYPE	   p_type,
		const Desc::Key &			   p_primary,
		const std::optional<Desc::Key> p_secondary
	)
	{
		pass.outputs.push_back( { p_type, p_primary, p_secondary } );
		return *this;
	}

	PassBuilder & PassBuilder::out( const Desc::Key & p_texture )
	{
		return out( Desc::E_RESOURCE_TYPE::TEXTURE, p_texture );
	}

	PassBuilder & PassBuilder::settings( const std::initializer_list<Desc::E_SETTING> p_settings )
	{
		pass.settings.insert( pass.settings.end(), p_settings.begin(), p_settings.end() );
		return *this;
	}

	ProgramBuilder PassBuilder::program( const Desc::Key & p_name )
	{
		if ( pass.type == Desc::E_PASS_TYPE::EXTERNAL )
		{
			throw GraphicException( "Cannot add program '{}' to external pass '{}'", p_name, pass.name );
		}

		pass.programs.emplace_back();
		Desc::Program & prog = pass.programs.back();
		prog.name			 = p_name;
		return ProgramBuilder( *this, prog );
	}

	GraphBuilder & PassBuilder::endPass()
	{
		graph.passes.emplace_back( std::make_unique<Desc::Pass>( std::move( pass ) ) );
		return graph;
	}

} // namespace VTX::Renderer
