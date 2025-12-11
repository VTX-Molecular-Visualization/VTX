#include "renderer/graph_builder.hpp"

namespace VTX::Renderer
{

	GraphBuilder & GraphBuilder::texture( const Key & p_name, const E_FORMAT p_format )
	{
		resources.textures[ p_name ] = Texture { p_format };
		return *this;
	}

	GraphBuilder & GraphBuilder::vertexStream(
		const Key &									 p_name,
		const std::initializer_list<VertexAttribute> p_attributes
	)
	{
		VertexLayout layout;
		layout.attributes.assign( p_attributes.begin(), p_attributes.end() );
		resources.vertexStreams[ p_name ] = std::move( layout );
		return *this;
	}

	GraphBuilder & GraphBuilder::uniformBuffer(
		const Key &								  p_name,
		const std::uint32_t						  p_binding,
		const std::initializer_list<UniformValue> p_values
	)
	{
		UniformBuffer desc;
		desc.name	 = p_name;
		desc.binding = p_binding;
		desc.values.assign( p_values.begin(), p_values.end() );
		resources.uniformBuffers[ p_name ] = std::move( desc );
		return *this;
	}

	PassBuilder GraphBuilder::pass( const Key & p_name ) { return PassBuilder( *this, p_name ); }

	ProgramBuilder::ProgramBuilder( PassBuilder & p_p, Program & p_prog ) : parent( p_p ), program( p_prog ) {}

	ProgramBuilder & ProgramBuilder::shaders( std::initializer_list<FilePath> p_files )
	{
		program.shaders.assign( p_files.begin(), p_files.end() );
		return *this;
	}

	ProgramBuilder & ProgramBuilder::draw(
		const Key &		  p_vertexStream,
		const E_PRIMITIVE p_primitive,
		const bool		  p_useIndices
	)
	{
		DrawCall dc;
		dc.vertexStream	 = p_vertexStream;
		dc.primitive	 = p_primitive;
		dc.useIndices	 = p_useIndices;
		program.drawCall = dc;
		return *this;
	}

	ProgramBuilder & ProgramBuilder::uniform( const UniformValue & p_u )
	{
		program.uniforms.push_back( p_u );
		return *this;
	}

	PassBuilder & ProgramBuilder::endProgram() { return parent; }

	PassBuilder::PassBuilder( GraphBuilder & p_g, const Key & p_name ) : graph( p_g ) { pass.name = p_name; }

	/**
	 * @brief in().
	 */
	PassBuilder & PassBuilder::in( const Key & p_resourceName )
	{
		pass.inputs.push_back( p_resourceName );
		return *this;
	}

	/**
	 * @brief out().
	 */
	PassBuilder & PassBuilder::out( const Key & p_resourceName )
	{
		pass.outputs.push_back( p_resourceName );
		return *this;
	}

	/**
	 * @brief program().
	 */
	ProgramBuilder PassBuilder::program( const Key & p_name )
	{
		pass.programs.emplace_back();
		Program & prog = pass.programs.back();
		prog.name	   = p_name;
		return ProgramBuilder( *this, prog );
	}

	/**
	 * @brief callback().
	 */
	PassBuilder & PassBuilder::callback( const RenderFunc p_func )
	{
		pass.customCallback = std::move( p_func );
		return *this;
	}

	/**
	 * @brief endPass().
	 */
	GraphBuilder & PassBuilder::endPass()
	{
		graph.passes.emplace_back( std::make_unique<Pass>( std::move( pass ) ) );
		return graph;
	}

} // namespace VTX::Renderer
