#ifndef __VTX_CORE_STRUCT_DEFINE__
#define __VTX_CORE_STRUCT_DEFINE__

#include "core/chemdb/atom.hpp"
#include <concepts>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	using ProgramManager = int;
	using VertexArray	 = int;

	template<typename T>
	concept ConceptPass = requires( T &				 p_pass,
									const size_t	 p_width,
									const size_t	 p_height,
									ProgramManager & p_pm,
									VertexArray &	 p_vao ) {
							  {
								  p_pass.init( p_width, p_height, p_pm )
								  } -> std::same_as<void>;
							  {
								  p_pass.resize( p_width, p_height )
								  } -> std::same_as<void>;
							  {
								  p_pass.render( p_vao )
								  } -> std::same_as<void>;
						  };

	template<ConceptPass P>
	class SharedPass : public P
	{
	  public:
		SharedPass() = default;
		SharedPass( const P & p_base ) : P( p_base ) {}

		void init( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
		{
			VTX_INFO( "Base Init" );
			P::init( p_width, p_height, p_pm );
		}

		void resize( const size_t p_width, const size_t p_height )
		{
			VTX_INFO( "Base resize" );
			P::resize( p_width, p_height );
		}

		void render( VertexArray & p_vao )
		{
			VTX_INFO( "Base render" );
			P::render( p_vao );
		}
	};

	class Shading
	{
	  public:
		Shading() = default;
		// Shading( const Shading & ) = default;

		void init( const size_t p_width, const size_t p_height, ProgramManager & p_pm ) { VTX_INFO( "Derived init" ); }
		void resize( const size_t p_width, const size_t p_height ) { VTX_INFO( "Derived resize" ); }
		void render( VertexArray & p_vao ) { VTX_INFO( "Derived render" ); }
	};

	// template<typename T>
	// concept ConceptBasePass = ConceptPass<T> && std::derived_from<SharedPass<T>, T>;

	template<ConceptPass Pass>
	void launchProgram( Pass & p_pass )
	{
		ProgramManager pm = 0;
		static_cast<SharedPass<Pass> &>( p_pass ).init( 10, 10, pm );
	}

	template<class T>
	concept VisibleConcept = requires( T & p_obj, bool p_visibleState ) {
								 {
									 p_obj.isVisible()
									 } -> std::same_as<bool>;

								 {
									 p_obj.setVisible( p_visibleState )
								 };
							 };
	;
	template<class T>
	concept BaseModelConcept = requires( T & p_obj ) {
								   {
									   p_obj.getId()
									   } -> std::convertible_to<uint>;
							   };

	template<class T>
	concept VisibleModelConcept = BaseModelConcept<T> && VisibleConcept<T>;

	// template<class T>
	// concept AtomConcept = requires( T & p_atom ) {};

	template<VisibleConcept T>
	void setVisible( T & p_item, const bool p_visible )
	{
		p_item.setVisible( p_visible );
	}

	// template<class A>
	// concept AtomConcept = requires( A & p_atom ) {
	//						  const std::string &		   name	  = p_atom.getName();
	//						  const uint				   id	  = p_atom.getID();
	//						  const ChemDB::Atom::SYMBOL & symbol = p_atom.getSymbol();
	//					  };

} // namespace VTX::Core::Struct
#endif
