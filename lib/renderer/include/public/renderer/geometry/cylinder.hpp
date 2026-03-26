#ifndef __VTX_RENDERER_GEOMETRY_CYLINDER__
#define __VTX_RENDERER_GEOMETRY_CYLINDER__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	class Cylinder : public BaseGeometry
	{
	  public:
		Cylinder()
		{
			vertexLayout   = "Atoms";
			indiceBuffer   = "Index.Bonds";
			indirectBuffer = "Indirect.Cylinders";
		}

		void clear()
		{
			BaseGeometry::clear();
			_construction.clear();
		}

		void construct( const Desc::Handle p_handle, const SystemData & p_data )
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
