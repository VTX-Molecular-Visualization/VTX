#ifndef __VTX_ACTION_REPRESENTABLE__
#define __VTX_ACTION_REPRESENTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "representation/representation_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Action
{
	class RepresentableSetRepresentation : public BaseAction
	{
	  public:
		explicit RepresentableSetRepresentation( Generic::BaseRepresentable &					   p_representable,
												 Model::Representation::BaseRepresentation * const p_representation ) :
			_representable( &p_representable ),
			_representation( p_representation )
		{
		}
		explicit RepresentableSetRepresentation( const Model::Selection * const				 p_selection,
												 Model::Representation::BaseRepresentation * p_representation ) :
			_selection( p_selection ),
			_representation( p_representation )
		{
		}

		void execute()
		{
			if ( _selection != nullptr )
			{
				Representation::RepresentationManager::get().instantiateRepresentation( _representation, *_selection );
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

		Model::Representation::BaseRepresentation * const _representation;
	};
	class RepresentableRemoveRepresentation : public BaseAction
	{
	  public:
		explicit RepresentableRemoveRepresentation( Generic::BaseRepresentable & p_representable ) :
			_representable( &p_representable )
		{
		}

		void execute()
		{
			_representable->removeRepresentation();
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		Generic::BaseRepresentable * const _representable;
	};
	class ChangeRepresentationPreset : public BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Generic::BaseRepresentable &					   p_representable,
											 Model::Representation::BaseRepresentation * const p_representation ) :
			_representable( &p_representable ),
			_representation( p_representation )
		{
		}
		explicit ChangeRepresentationPreset( const Model::Selection * const					   p_selection,
											 Model::Representation::BaseRepresentation * const p_representation ) :
			_selection( p_selection ),
			_representation( p_representation )
		{
		}

		void execute()
		{
			if ( _selection != nullptr ) {}
			else
			{
				Model::Representation::InstantiatedRepresentation * const instantiatedRepresentation
					= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
						_representation );
				_representable->setRepresentation( instantiatedRepresentation );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Model::Selection * const	   _selection	  = nullptr;
		Generic::BaseRepresentable * const _representable = nullptr;

		Model::Representation::BaseRepresentation * const _representation;
	};

	class ChangeRepresentationColorMode : public BaseAction
	{
	  public:
		explicit ChangeRepresentationColorMode( Model::Representation::BaseRepresentation * const p_representation,
												const Generic::COLOR_MODE &						  p_colorMode ) :
			_representation( p_representation ),
			_colorMode( p_colorMode )
		{
		}
		explicit ChangeRepresentationColorMode(
			Model::Representation::InstantiatedRepresentation * const p_representation,
			const Generic::COLOR_MODE &								  p_colorMode ) :
			_instantiatedRepresentation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute()
		{
			if ( _representation != nullptr )
				_representation->setColorMode( _colorMode );
			else
				_instantiatedRepresentation->setColorMode( _colorMode );

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Generic::COLOR_MODE								  _colorMode;
		Model::Representation::BaseRepresentation * const		  _representation			  = nullptr;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation = nullptr;
	};
	class ChangeRepresentationColor : public BaseAction
	{
	  public:
		explicit ChangeRepresentationColor( Model::Representation::BaseRepresentation * const p_representation,
											const Color::Rgb &								  p_color ) :
			_representation( p_representation ),
			_color( p_color )
		{
		}
		explicit ChangeRepresentationColor( Model::Representation::InstantiatedRepresentation * const p_representation,
											const Color::Rgb &										  p_color ) :
			_instantiatedRepresentation( p_representation ),
			_color( p_color )
		{
		}

		void execute()
		{
			if ( _representation != nullptr )
				_representation->setColor( _color );
			else
				_instantiatedRepresentation->setColor( _color );

			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb										  _color;
		Model::Representation::BaseRepresentation * const		  _representation			  = nullptr;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation = nullptr;
	};

	class ChangeRepresentationSphereRadius : public BaseAction
	{
	  public:
		explicit ChangeRepresentationSphereRadius( Model::Representation::BaseRepresentation * const p_representation,
												   const float										 p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}
		explicit ChangeRepresentationSphereRadius(
			Model::Representation::InstantiatedRepresentation * const p_representation,
			const float												  p_radius ) :
			_instantiatedRepresentation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			if ( _representation != nullptr )
				_representation->getSphereData()._radiusFixed = _radius;
			else
				_instantiatedRepresentation->setSphereFixedRadius( _radius );

			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float												  _radius;
		Model::Representation::BaseRepresentation * const		  _representation			  = nullptr;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation = nullptr;
	};
	class ChangeRepresentationSphereRadiusAdd : public BaseAction
	{
	  public:
		explicit ChangeRepresentationSphereRadiusAdd(
			Model::Representation::BaseRepresentation * const p_representation,
			const float										  p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}
		explicit ChangeRepresentationSphereRadiusAdd(
			Model::Representation::InstantiatedRepresentation * const p_representation,
			const float												  p_radius ) :
			_instantiatedRepresentation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			if ( _representation != nullptr )
				_representation->getSphereData()._radiusAdd = _radius;
			else
				_instantiatedRepresentation->setSphereAddRadius( _radius );

			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float												  _radius;
		Model::Representation::BaseRepresentation * const		  _representation			  = nullptr;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation = nullptr;
	};
	class ChangeRepresentationCylinderRadius : public BaseAction
	{
	  public:
		explicit ChangeRepresentationCylinderRadius( Model::Representation::BaseRepresentation * const p_representation,
													 const float									   p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}
		explicit ChangeRepresentationCylinderRadius(
			Model::Representation::InstantiatedRepresentation * const p_representation,
			const float												  p_radius ) :
			_instantiatedRepresentation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			if ( _representation != nullptr )
				_representation->getCylinderData()._radius = _radius;
			else
				_instantiatedRepresentation->setCylinderRadius( _radius );

			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float												  _radius;
		Model::Representation::BaseRepresentation * const		  _representation			  = nullptr;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation = nullptr;
	};
	class ChangeRepresentationCylinderAndSphereRadius : public BaseAction
	{
	  public:
		explicit ChangeRepresentationCylinderAndSphereRadius(
			Model::Representation::BaseRepresentation * const p_representation,
			const float										  p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}
		explicit ChangeRepresentationCylinderAndSphereRadius(
			Model::Representation::InstantiatedRepresentation * const p_representation,
			const float												  p_radius ) :
			_instantiatedRepresentation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			if ( _representation != nullptr )
			{
				_representation->getSphereData()._radiusFixed = _radius;
				_representation->getCylinderData()._radius	  = _radius;
			}
			else
			{
				_instantiatedRepresentation->setSphereFixedRadius( _radius );
				_instantiatedRepresentation->setCylinderRadius( _radius );
			}

			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float												  _radius;
		Model::Representation::BaseRepresentation * const		  _representation			  = nullptr;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation = nullptr;
	};
} // namespace VTX::Action
#endif
