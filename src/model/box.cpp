#include "box.hpp"
#include "mvc/mvc_manager.hpp"
#include "view/d3/box.hpp"

namespace VTX::Model
{
	Box::Box( const Math::AABB & p_aabb ) : BaseModel3D( VTX::ID::Model::MODEL_BOX ) { _aabb = p_aabb; }

	void Box::_fillBuffer()
	{
		_indices = std::vector<uint>( { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 } );
		_buffer->setIndices( _indices );

		refresh();
	}

	void Box::refresh()
	{
		const Vec3f & min = _aabb.getMin();
		const Vec3f & max = _aabb.getMax();
		_corners		  = std::vector<Vec3f>( { min,
										  Vec3f( max.x, min.y, min.z ),
										  Vec3f( max.x, max.y, min.z ),
										  Vec3f( min.x, max.y, min.z ),
										  Vec3f( min.x, min.y, max.z ),
										  Vec3f( max.x, min.y, max.z ),
										  max,
										  Vec3f( min.x, max.y, max.z ) } );

		_buffer->setCorners( _corners );
	}

	void Box::_instantiate3DViews()
	{
		_addRenderable( MVC::MvcManager::get().instantiateView<View::D3::Box>( this, VTX::ID::View::D3_BOX ) );
	}

} // namespace VTX::Model
