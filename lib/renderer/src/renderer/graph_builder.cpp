#include "renderer/graph_builder.hpp"

namespace VTX::Renderer
{

	GraphBuilder::GraphBuilder()
	{
		// Create default sampler.
		sampler(
			"Default", E_WRAPPING::CLAMP_TO_EDGE, E_WRAPPING::CLAMP_TO_EDGE, E_FILTERING::NEAREST, E_FILTERING::NEAREST
		);
	}

	GraphBuilder & GraphBuilder::texture( const Key & p_name, const E_FORMAT p_format, const Size2D & p_size )
	{
		resources.textures[ p_name ] = Texture { p_format, p_size };
		return *this;
	}

	GraphBuilder & GraphBuilder::sampler(
		const Key &		  p_name,
		const E_WRAPPING  p_wrapS,
		const E_WRAPPING  p_wrapT,
		const E_FILTERING p_minFilter,
		const E_FILTERING p_magFilter
	)
	{
		resources.samplers[ p_name ] = Sampler { p_wrapS, p_wrapT, p_minFilter, p_magFilter };
		return *this;
	}

	GraphBuilder & GraphBuilder::vertexLayout(
		const Key &									 p_name,
		const std::initializer_list<VertexAttribute> p_attributes
	)
	{
		VertexLayout layout;
		layout.attributes.assign( p_attributes.begin(), p_attributes.end() );
		resources.vertexStreams[ p_name ] = std::move( layout );
		return *this;
	}

	GraphBuilder & GraphBuilder::bufferLayout(
		const Key &								  p_name,
		const E_BUFFER_ROLE						  p_role,
		const E_BUFFER_ACCESS					  p_access,
		const E_UPDATE_FREQUENCY				  p_frequency,
		const uint32_t							  p_binding,
		const std::initializer_list<UniformValue> p_values
	)
	{
		BufferLayout desc;
		desc.role	   = p_role;
		desc.access	   = p_access;
		desc.frequency = p_frequency;
		desc.binding   = p_binding;
		desc.values.assign( p_values.begin(), p_values.end() );
		resources.buffers[ p_name ] = std::move( desc );
		return *this;
	}

	VTX::Renderer::GraphBuilder & VTX::Renderer::GraphBuilder::bufferData(
		const Key &				 p_name,
		const E_DATA_BUFFER_KIND p_kind,
		const E_UPDATE_FREQUENCY p_frequency
	)
	{
		BufferData db;
		db.kind							= p_kind;
		db.frequency					= p_frequency;
		resources.dataBuffers[ p_name ] = std::move( db );
		return *this;
	}

	GraphBuilder & GraphBuilder::geometry(
		const Key & p_name,
		const Key & p_vertexLayout,
		// const std::unordered_map<Key, Key> & p_overrides,
		const std::optional<Key> p_indexBuffer
	)
	{
		Geometry geom;
		geom.vertexLayout = p_vertexLayout;
		geom.indexBuffer  = p_indexBuffer;
		// geom.overrides	 = p_overrides;
		resources.geometries[ p_name ] = std::move( geom );
		return *this;
	}

	PassBuilder GraphBuilder::pass( const Key & p_name ) { return PassBuilder( *this, p_name ); }

	ProgramBuilder::ProgramBuilder( PassBuilder & p_p, Program & p_prog ) : parent( p_p ), program( p_prog ) {}

	ProgramBuilder & ProgramBuilder::shaders( std::initializer_list<FilePath> p_files )
	{
		program.shaders.assign( p_files.begin(), p_files.end() );
		return *this;
	}

	ProgramBuilder & ProgramBuilder::uniform( const UniformValue & p_u )
	{
		program.uniforms.push_back( p_u );
		return *this;
	}

	ProgramBuilder & ProgramBuilder::draw(
		const Key &		  p_geometry,
		const E_PRIMITIVE p_primitive,
		const uint32_t	  p_vertexCount,
		const uint32_t	  p_indexCount
	)
	{
		DrawCall dc;
		dc.geometry		 = p_geometry;
		dc.primitive	 = p_primitive;
		dc.vertexCount	 = p_vertexCount;
		dc.indexCount	 = p_indexCount;
		program.drawCall = dc;
		return *this;
	}

	PassBuilder & ProgramBuilder::endProgram() { return parent; }

	PassBuilder::PassBuilder( GraphBuilder & p_g, const Key & p_name ) : graph( p_g ) { pass.name = p_name; }

	PassBuilder & PassBuilder::in(
		const E_RESOURCE_TYPE	 p_type,
		const Key &				 p_primary,
		const std::optional<Key> p_secondary
	)
	{
		pass.inputs.push_back( { p_type, p_primary, p_secondary } );
		return *this;
	}

	PassBuilder & PassBuilder::out(
		const E_RESOURCE_TYPE	 p_type,
		const Key &				 p_primary,
		const std::optional<Key> p_secondary
	)
	{
		pass.outputs.push_back( { p_type, p_primary, p_secondary } );
		return *this;
	}

	ProgramBuilder PassBuilder::program( const Key & p_name )
	{
		pass.programs.emplace_back();
		Program & prog = pass.programs.back();
		prog.name	   = p_name;
		return ProgramBuilder( *this, prog );
	}

	PassBuilder & PassBuilder::callback( const RenderFunc p_func )
	{
		pass.customCallback = std::move( p_func );
		return *this;
	}

	GraphBuilder & PassBuilder::endPass()
	{
		graph.passes.emplace_back( std::make_unique<Pass>( std::move( pass ) ) );
		return graph;
	}

} // namespace VTX::Renderer
