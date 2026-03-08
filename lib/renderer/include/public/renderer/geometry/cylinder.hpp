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

		void construct( const Desc::Handle p_handle, const SystemData & p_data )
		{
			const Index count = p_data.data.getBondCount() * 2;

			_addRange( p_handle, count, p_data.data.getAtomCount() );

			auto & indiceBuffer = _indices( p_handle );
			indiceBuffer		= p_data.data.bondPairAtomIndexes;

			_data.emplace( p_handle, _Data { &p_data.data.bondPairAtomIndexes } );
		}

		void setVisibility( const Desc::Handle p_handle, const IndexRangeList & p_ranges )
		{
			auto &					   indiceBuffer = _indices( p_handle );
			const std::vector<Index> & bonds		= *_data[ p_handle ].bonds;

			indiceBuffer.clear();
			if ( p_ranges.isEmpty() )
			{
				return;
			}

			std::vector<bool> visible( p_ranges.getLast() + 1, false );
			for ( Index i : p_ranges )
			{
				visible[ i ] = true;
			}

			for ( Index i = 0; i < bonds.size(); i += 2 )
			{
				if ( bonds[ i ] >= visible.size() || bonds[ i + 1 ] >= visible.size() )
				{
					continue;
				}

				if ( visible[ bonds[ i ] ] && visible[ bonds[ i + 1 ] ] )
				{
					indiceBuffer.emplace_back( bonds[ i ] );
					indiceBuffer.emplace_back( bonds[ i + 1 ] );
				}
			}
		}

	  private:
		struct _Data
		{
			const std::vector<Index> * bonds;
		};
		std::unordered_map<Desc::Handle, _Data> _data;
	};
} // namespace VTX::Renderer::Geometry

#endif
