#ifndef __VTX_ACTION_REPRESENTABLE__
#define __VTX_ACTION_REPRESENTABLE__

#include "app/core/action/base_action.hpp"
#include "app/old_app/generic/base_representable.hpp"
#include "app/old_app/model/representation/representation.hpp"
#include "app/old_app/model/selection.hpp"

namespace VTX::Action::Representable
{
	class SetRepresentation : public Core::Action::BaseAction
	{
	  public:
		explicit SetRepresentation( Generic::BaseRepresentable &				  p_representable,
									Model::Representation::Representation * const p_representation ) :
			_representable( &p_representable ),
			_representation( p_representation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit SetRepresentation( const Model::Selection * const			p_selection,
									Model::Representation::Representation * p_representation ) :
			_selection( p_selection ),
			_representation( p_representation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		void execute();

	  private:
		const Model::Selection * const	   _selection	  = nullptr;
		Generic::BaseRepresentable * const _representable = nullptr;

		Model::Representation::Representation * const _representation;
	};
	class RemoveRepresentation : public Core::Action::BaseAction
	{
	  public:
		explicit RemoveRepresentation( Generic::BaseRepresentable & p_representable ) :
			_representable( &p_representable )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		void execute();

	  private:
		Generic::BaseRepresentable * const _representable;
	};
} // namespace VTX::Action::Representable
#endif
