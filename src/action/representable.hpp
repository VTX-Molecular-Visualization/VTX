#ifndef __VTX_ACTION_REPRESENTABLE__
#define __VTX_ACTION_REPRESENTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "representation/representation_manager.hpp"
#include "util/molecule.hpp"

namespace VTX
{
	namespace Action
	{
		class RepresentableAddRepresentation : public BaseAction
		{
		  public:
			explicit RepresentableAddRepresentation( Generic::BaseRepresentable &				 p_representable,
													 Model::Molecule &							 p_molecule,
													 Model::Representation::BaseRepresentation * p_representation ) :
				_representable( p_representable ),
				_molecule( p_molecule ), _representation( p_representation )
			{
			}

			void execute()
			{
				Representation::RepresentationManager::get().toggleRepresentation( _representation, &_representable );
				Util::Molecule::refreshRepresentationState( _molecule );
			};

		  private:
			Generic::BaseRepresentable &				_representable;
			Model::Molecule &							_molecule;
			Model::Representation::BaseRepresentation * _representation;
		};

		class RepresentableRemoveRepresentation : public BaseAction
		{
		  public:
			explicit RepresentableRemoveRepresentation( Generic::BaseRepresentable &				p_representable,
														Model::Molecule &							p_molecule,
														Model::Representation::BaseRepresentation * p_representation ) :
				_representable( p_representable ),
				_molecule( p_molecule ), _representation( p_representation )
			{
			}

			void execute()
			{
				Representation::RepresentationManager::get().removeRepresentation( _representation );
				Util::Molecule::refreshRepresentationState( _molecule );
			};

		  private:
			Generic::BaseRepresentable &				_representable;
			Model::Molecule &							_molecule;
			Model::Representation::BaseRepresentation * _representation;
		};
	} // namespace Action
} // namespace VTX
#endif
