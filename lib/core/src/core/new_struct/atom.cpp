#include "core/new_struct/atom.hpp"
#include "core/new_struct/chain.hpp"
#include "core/new_struct/molecule.hpp"
#include "core/new_struct/residue.hpp"

namespace VTX::Core::Struct
{
	const Vec3f & Atom::getLocalPosition() const
	{
		return getLocalPosition( moleculePtr->trajectory.currentFrameIndex );
	}
	const Vec3f & Atom::getLocalPosition( const size_t & p_frameIndex ) const
	{
		return moleculePtr->trajectory.frames[ p_frameIndex ][ p_frameIndex ];
	}
	const Vec3f Atom::getWorldPosition() const
	{
		return Vec3f( Vec4f( getLocalPosition(), 0 ) * moleculePtr->gpuStruct.transform );
	}
	const Vec3f Atom::getWorldPosition( const size_t & p_frameIndex ) const
	{
		return Vec3f( Vec4f( getLocalPosition( p_frameIndex ), 0 ) * moleculePtr->gpuStruct.transform );
	}

	const bool Atom::isVisible() { return moleculePtr->gpuStruct.atomVisibilities[ internalIndex ]; }
	const void Atom::setVisible( const bool p_visible )
	{
		moleculePtr->gpuStruct.atomVisibilities[ internalIndex ] = p_visible;
	}
} // namespace VTX::Core::Struct
