#ifndef __VTX_ACTION_REPRESENTATION__
#define __VTX_ACTION_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

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

namespace VTX::Action::Representation
{
	class ChangeColorMode : public BaseAction
	{
	  public:
		explicit ChangeColorMode( Model::Representation::BaseRepresentation * const p_representation,
								  const Generic::COLOR_MODE &						p_colorMode ) :
			_representation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute()
		{
			_representation->setColorMode( _colorMode );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Generic::COLOR_MODE						  _colorMode;
		Model::Representation::BaseRepresentation * const _representation;
	};

	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Representation::BaseRepresentation * const p_representation,
							  const Color::Rgb &								p_color ) :
			_representation( p_representation ),
			_color( p_color )
		{
		}
		void execute()
		{
			_representation->setColor( _color );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb								  _color;
		Model::Representation::BaseRepresentation * const _representation;
	};

	class ChangeSecondaryStructureColorMode : public BaseAction
	{
	  public:
		explicit ChangeSecondaryStructureColorMode( Model::Representation::BaseRepresentation * const p_representation,
													Generic::SECONDARY_STRUCTURE_COLOR_MODE &		  p_colorMode ) :
			_representation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute()
		{
			_representation->setSecondaryStructureColorMode( _colorMode );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE	  _colorMode;
		Model::Representation::BaseRepresentation * const _representation;
	};

	class ChangeSphereRadius : public BaseAction
	{
	  public:
		explicit ChangeSphereRadius( Model::Representation::BaseRepresentation * const p_representation,
									 const float									   p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			_representation->getSphereData()._radiusFixed = _radius;
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float										  _radius;
		Model::Representation::BaseRepresentation * const _representation;
	};
	class ChangeSphereRadiusAdd : public BaseAction
	{
	  public:
		explicit ChangeSphereRadiusAdd( Model::Representation::BaseRepresentation * const p_representation,
										const float										  p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			_representation->getSphereData()._radiusAdd = _radius;
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float										  _radius;
		Model::Representation::BaseRepresentation * const _representation;
	};
	class ChangeCylinderRadius : public BaseAction
	{
	  public:
		explicit ChangeCylinderRadius( Model::Representation::BaseRepresentation * const p_representation,
									   const float										 p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			_representation->getCylinderData()._radius = _radius;
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float										  _radius;
		Model::Representation::BaseRepresentation * const _representation;
	};
} // namespace VTX::Action::Representation
#endif
