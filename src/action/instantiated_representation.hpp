#ifndef __VTX_ACTION_INSTANTIATED_REPRESENTATION__
#define __VTX_ACTION_INSTANTIATED_REPRESENTATION__

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

namespace VTX::Action::InstantiatedRepresentation
{
	class ChangeColorMode : public BaseAction
	{
	  public:
		explicit ChangeColorMode( Model::Representation::InstantiatedRepresentation * const p_representation,
								  const Generic::COLOR_MODE &								p_colorMode ) :
			_instantiatedRepresentation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute()
		{
			_instantiatedRepresentation->setColorMode( _colorMode );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Generic::COLOR_MODE								  _colorMode;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Representation::InstantiatedRepresentation * const p_representation,
							  const Color::Rgb &										p_color ) :
			_instantiatedRepresentation( p_representation ),
			_color( p_color )
		{
		}

		void execute()
		{
			_instantiatedRepresentation->setColor( _color );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb										  _color;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeSecondaryStructureColorMode : public BaseAction
	{
	  public:
		explicit ChangeSecondaryStructureColorMode(
			Model::Representation::InstantiatedRepresentation * const p_representation,
			const Generic::SECONDARY_STRUCTURE_COLOR_MODE &			  p_colorMode ) :
			_instantiatedRepresentation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute()
		{
			_instantiatedRepresentation->setSecondaryStructureColorMode( _colorMode );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE			  _colorMode;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeSphereRadius : public BaseAction
	{
	  public:
		explicit ChangeSphereRadius( Model::Representation::InstantiatedRepresentation * const p_representation,
									 const float											   p_radius ) :
			_instantiatedRepresentation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			switch ( _instantiatedRepresentation->getLinkedRepresentation()->getRepresentationType() )
			{
			case Generic::REPRESENTATION::BALL_AND_STICK:
			case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON:
				_instantiatedRepresentation->setSphereFixedRadius( _radius );
				if ( _radius < _instantiatedRepresentation->getCylinderData()._radius )
					_instantiatedRepresentation->setCylinderRadius( _radius );
				break;

			case Generic::REPRESENTATION::SAS:
			case Generic::REPRESENTATION::VAN_DER_WAALS:
				_instantiatedRepresentation->setSphereAddRadius( _radius );
				break;

			case Generic::REPRESENTATION::STICK:
			case Generic::REPRESENTATION::STICK_AND_CARTOON:
			case Generic::REPRESENTATION::CARTOON:
			case Generic::REPRESENTATION::TRACE: break;
			}

			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float												  _radius;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};
	class ChangeCylinderRadius : public BaseAction
	{
	  public:
		explicit ChangeCylinderRadius( Model::Representation::InstantiatedRepresentation * const p_representation,
									   const float												 p_radius ) :
			_instantiatedRepresentation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			switch ( _instantiatedRepresentation->getLinkedRepresentation()->getRepresentationType() )
			{
			case Generic::REPRESENTATION::BALL_AND_STICK:
			case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON:
				_instantiatedRepresentation->setCylinderRadius( _radius );
				if ( _radius > _instantiatedRepresentation->getSphereData()._radiusFixed )
					_instantiatedRepresentation->setSphereFixedRadius( _radius );
				break;

			case Generic::REPRESENTATION::STICK:
			case Generic::REPRESENTATION::STICK_AND_CARTOON:
			case Generic::REPRESENTATION::TRACE:
				_instantiatedRepresentation->setSphereFixedRadius( _radius );
				_instantiatedRepresentation->setCylinderRadius( _radius );
				break;

			case Generic::REPRESENTATION::VAN_DER_WAALS:
			case Generic::REPRESENTATION::CARTOON:
			case Generic::REPRESENTATION::SAS: break;
			}

			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float												  _radius;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};
} // namespace VTX::Action::InstantiatedRepresentation
#endif
