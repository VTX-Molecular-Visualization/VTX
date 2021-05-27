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
#include <unordered_set>

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
			_color( p_color )
		{
			_instantiatedRepresentations.emplace( p_representation );
		}

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseRepresentable, T>::value>>
		explicit ChangeColor( const std::unordered_set<T *> & p_representables, const Color::Rgb & p_color ) :
			_color( p_color )
		{
			for ( T * const representable : p_representables )
			{
				if ( representable->hasCustomRepresentation() )
				{
					_instantiatedRepresentations.emplace( representable->getCustomRepresentation() );
				}
				else
				{
					Model::Representation::InstantiatedRepresentation * instantiatedRepresentation
						= VTX::Representation::RepresentationManager::get().instantiateCopy(
							representable->getRepresentation(), *representable );

					representable->computeAllRepresentationData();
					_instantiatedRepresentations.emplace( instantiatedRepresentation );
				}
			}
		}

		void execute()
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( Model::Representation::InstantiatedRepresentation * const representation :
				  _instantiatedRepresentations )
			{
				representation->setColor( _color, false, true );
				molecules.emplace( representation->getTarget()->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				molecule->refreshColors();
			}

			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb														_color;
		std::unordered_set<Model::Representation::InstantiatedRepresentation *> _instantiatedRepresentations
			= std::unordered_set<Model::Representation::InstantiatedRepresentation *>();
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
			_instantiatedRepresentation->setSphereRadius( _radius );
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
			_instantiatedRepresentation->setCylinderRadius( _radius );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float												  _radius;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};
} // namespace VTX::Action::InstantiatedRepresentation
#endif
