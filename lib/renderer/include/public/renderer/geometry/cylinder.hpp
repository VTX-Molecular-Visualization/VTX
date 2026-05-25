#ifndef __VTX_RENDERER_GEOMETRY_CYLINDER__
#define __VTX_RENDERER_GEOMETRY_CYLINDER__

#include "base_geometry.hpp"
#include "renderer/caches.hpp"

namespace VTX::Renderer::Geometry
{

	class Cylinder : public BaseGeometry
	{
	  public:
		inline static const Desc::Key		  VERTEX_LAYOUT_ATOMS		 = "Atoms";
		inline static const Desc::Key		  GEOMETRY_CYLINDERS		 = "Cylinders";
		inline static const Desc::Key		  INDEX_BONDS				 = "Index.Bonds";
		inline static const Desc::Key		  INDIRECT_CYLINDERS		 = "Indirect.Cylinders";
		inline static constexpr Desc::Binding BINDING_INDIRECT_CYLINDERS = 21;

		Cylinder()
		{
			vertexLayout   = VERTEX_LAYOUT_ATOMS;
			indiceBuffer   = INDEX_BONDS;
			indirectBuffer = INDIRECT_CYLINDERS;
		}

		void clear()
		{
			BaseGeometry::clear();
			_construction.clear();
		}

		void construct( const Desc::Handle p_handle, const Cache::System & p_data )
		{
			const Index count = p_data.data.getBondCount() * 2;

			_addRange( p_handle, count, p_data.data.getAtomCount() );

			auto & indiceBuffer = _indices( p_handle );
			indiceBuffer		= p_data.data.bondPairAtomIndexes;

			_construction.emplace( p_handle, _Construction { &p_data.data.bondPairAtomIndexes } );
		}

		void setVisibility( const Desc::Handle p_handle, const Util::Math::BitSet & p_visibility )
		{
			auto &					   indiceBuffer = _indices( p_handle );
			const std::vector<Index> & bonds		= *_construction[ p_handle ].bonds;

			indiceBuffer.clear();
			for ( Index i = 0; i < bonds.size(); i += 2 )
			{
				if ( p_visibility.test( bonds[ i ] ) && p_visibility.test( bonds[ i + 1 ] ) )
				{
					indiceBuffer.emplace_back( bonds[ i ] );
					indiceBuffer.emplace_back( bonds[ i + 1 ] );
				}
			}
		}

	  private:
		struct _Construction
		{
			const std::vector<Index> * bonds;
		};

		std::unordered_map<Desc::Handle, _Construction> _construction;
	};
} // namespace VTX::Renderer::Geometry

#endif
