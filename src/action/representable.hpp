#ifndef __VTX_ACTION_REPRESENTABLE__
#define __VTX_ACTION_REPRESENTABLE__

#include "base_action.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/secondary_structure.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "representation/representation_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Action
{
	class RepresentableSetRepresentation : public BaseAction
	{
	  public:
		explicit RepresentableSetRepresentation( Generic::BaseRepresentable &				   p_representable,
												 Model::Representation::Representation * const p_representation ) :
			_representable( &p_representable ),
			_representation( p_representation )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit RepresentableSetRepresentation( const Model::Selection * const			 p_selection,
												 Model::Representation::Representation * p_representation ) :
			_selection( p_selection ),
			_representation( p_representation )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		void execute()
		{
			if ( _selection != nullptr )
			{
				Representation::RepresentationManager::get().instantiateRepresentations( _representation, *_selection );
			}
			else
			{
				Representation::RepresentationManager::get().instantiateRepresentation( _representation,
																						*_representable );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Model::Selection * const	   _selection	  = nullptr;
		Generic::BaseRepresentable * const _representable = nullptr;

		Model::Representation::Representation * const _representation;
	};
	class RepresentableRemoveRepresentation : public BaseAction
	{
	  public:
		explicit RepresentableRemoveRepresentation( Generic::BaseRepresentable & p_representable ) :
			_representable( &p_representable )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		void execute()
		{
			Representation::RepresentationManager::get().removeInstantiatedRepresentation( *_representable );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		Generic::BaseRepresentable * const _representable;
	};
} // namespace VTX::Action
#endif
