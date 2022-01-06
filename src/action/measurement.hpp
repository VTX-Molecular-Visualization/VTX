#ifndef __VTX_ACTION_MEASUREMENT__
#define __VTX_ACTION_MEASUREMENT__

#include "action/base_action.hpp"
#include "model/atom.hpp"
#include "model/molecule.hpp"

namespace VTX::Action::Measurement
{
	class Distance : public BaseAction
	{
	  public:
		explicit Distance( const Model::Atom & p_firstAtom, const Model::Atom & p_secondAtom ) :
			_firstAtom( p_firstAtom ), _secondAtom( p_secondAtom )
		{
		}

		virtual void execute() override
		{
			const float distance
				= Util::Math::distance( _firstAtom.getWorldPosition(), _secondAtom.getWorldPosition() );

			VTX_INFO( "Distance between " + _firstAtom.getName() + " and " + _secondAtom.getName() + " : "
					  + std::to_string( distance ) + "Å" );
		}

	  private:
		const Model::Atom & _firstAtom;
		const Model::Atom & _secondAtom;
	};
} // namespace VTX::Action::Measurement
#endif
