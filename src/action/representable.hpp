#ifndef __VTX_ACTION_REPRESENTABLE__
#define __VTX_ACTION_REPRESENTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/selection.hpp"
#include "representation/representation_manager.hpp"

namespace VTX
{
	namespace Action
	{
		class RepresentableAddRepresentation : public BaseAction
		{
		  public:
			explicit RepresentableAddRepresentation( Generic::BaseRepresentable & p_representable, Model::Representation::BaseRepresentation * p_representation ) :
				_representable( &p_representable ), _representation( p_representation )
			{
			}
			explicit RepresentableAddRepresentation( const Model::Selection * const p_selection, Model::Representation::BaseRepresentation * p_representation ) :
				_selection( p_selection ), _representation( p_representation )
			{
			}

			void execute()
			{
				if ( _selection != nullptr )
					Representation::RepresentationManager::get().instantiateRepresentation( _representation, _selection );
				else
					Representation::RepresentationManager::get().instantiateRepresentation( _representation, _representable );
			};

		  private:
			const Model::Selection * const	   _selection	  = nullptr;
			Generic::BaseRepresentable * const _representable = nullptr;

			Model::Representation::BaseRepresentation * _representation;
		};

		class RepresentableRemoveRepresentation : public BaseAction
		{
		  public:
			explicit RepresentableRemoveRepresentation( Generic::BaseRepresentable & p_representable, Model::Representation::BaseRepresentation * p_representation ) :
				_representable( &p_representable ), _representation( p_representation )
			{
			}

			void execute() { Representation::RepresentationManager::get().removeRepresentation( _representation, _representable ); };

		  private:
			Generic::BaseRepresentable * const			_representable;
			Model::Representation::BaseRepresentation * _representation;
		};

		class ChangeRepresentationColorMode
		{
		  public:
			explicit ChangeRepresentationColorMode( Model::Representation::BaseRepresentation * const p_representation, const Generic::COLOR_MODE & p_colorMode ) :
				_representation( p_representation ), _colorMode( p_colorMode )
			{
			}

			void execute() { _representation->setColorMode( _colorMode ); };

		  private:
			const Generic::COLOR_MODE						  _colorMode;
			Model::Representation::BaseRepresentation * const _representation;
		};
	} // namespace Action
} // namespace VTX
#endif
