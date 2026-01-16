#include "renderer/graph_builder.hpp"

namespace VTX::Renderer
{

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

	GraphBuilder & GraphBuilder::shaderBuffer(
		const Desc::Key &								p_name,
		const Desc::E_SHADER_BUFFER_KIND				p_role,
		const Desc::E_BUFFER_MUTABILITY					p_mutability,
		const Desc::E_BUFFER_ACCESS						p_access,
		const Desc::E_UPDATE_FREQUENCY					p_frequency,
		const uint32_t									p_binding, // will be removed later
		const std::initializer_list<Desc::UniformValue> p_values
	)
	{
		Desc::BufferShader desc;
		desc.name		= p_name;
		desc.role		= p_role;
		desc.mutability = p_mutability;
		desc.access		= p_access;
		desc.frequency	= p_frequency;
		desc.binding	= p_binding;
		desc.values.assign( p_values.begin(), p_values.end() );
		resources.shaderBuffers[ p_name ] = std::move( desc );
		return *this;
	}

	VTX::Renderer::GraphBuilder & VTX::Renderer::GraphBuilder::pipelineBuffer(
		const Desc::Key &				   p_name,
		const Desc::E_PIPELINE_BUFFER_KIND p_kind,
		const Desc::E_UPDATE_FREQUENCY	   p_frequency
	)
	{
		Desc::BufferPipeline db;
		db.name								= p_name;
		db.kind								= p_kind;
		db.frequency						= p_frequency;
		resources.pipelineBuffers[ p_name ] = std::move( db );
		return *this;
	}

	GraphBuilder & GraphBuilder::geometry(
		const Desc::Key & p_name,
		const Desc::Key & p_vertexLayout,
		// const std::unordered_map<Key, Key> & p_overrides,
		const std::optional<Desc::Key> p_indexBuffer
	)
	{
		Desc::Geometry geom;
		geom.vertexLayout = p_vertexLayout;
		geom.indexBuffer  = p_indexBuffer;
		// geom.overrides	 = p_overrides;
		resources.geometries[ p_name ] = std::move( geom );
		return *this;
	}

	PassBuilder GraphBuilder::pass( const Desc::Key & p_name ) { return PassBuilder( *this, p_name ); }

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

	ProgramBuilder & ProgramBuilder::draw(
		const Desc::Key &					  p_geometry,
		const Desc::E_PRIMITIVE				  p_primitive,
		const Desc::DrawCall::RangeArrays *	  p_vertexRanges,
		const Desc::DrawCall::RangeElements * p_indexRanges
	)
	{
		Desc::DrawCall dc;
		dc.geometry		 = p_geometry;
		dc.primitive	 = p_primitive;
		dc.vertexRanges	 = p_vertexRanges;
		dc.indexRanges	 = p_indexRanges;
		program.drawCall = dc;
		return *this;
	}

	PassBuilder & ProgramBuilder::endProgram() { return parent; }

	PassBuilder::PassBuilder( GraphBuilder & p_g, const Desc::Key & p_name ) : graph( p_g ) { pass.name = p_name; }

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

	PassBuilder & PassBuilder::settings( const std::initializer_list<Desc::Setting> p_settings )
	{
		pass.settings.insert( pass.settings.end(), p_settings.begin(), p_settings.end() );
		return *this;
	}

	ProgramBuilder PassBuilder::program( const Desc::Key & p_name )
	{
		pass.programs.emplace_back();
		Desc::Program & prog = pass.programs.back();
		prog.name			 = p_name;
		return ProgramBuilder( *this, prog );
	}

	PassBuilder & PassBuilder::callback( const Desc::RenderFunc p_func )
	{
		pass.customCallback = std::move( p_func );
		return *this;
	}

	GraphBuilder & PassBuilder::endPass()
	{
		graph.passes.emplace_back( std::make_unique<Desc::Pass>( std::move( pass ) ) );
		return graph;
	}

} // namespace VTX::Renderer
