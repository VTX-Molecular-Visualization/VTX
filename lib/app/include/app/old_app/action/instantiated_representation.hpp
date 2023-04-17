#ifndef __VTX_ACTION_INSTANTIATED_REPRESENTATION__
#define __VTX_ACTION_INSTANTIATED_REPRESENTATION__

#include "base_action.hpp"
#include "app/old_app/model/molecule.hpp"
#include "app/old_app/model/representation/instantiated_representation.hpp"
#include "app/old_app/model/representation/representation.hpp"
#include "app/old_app/model/secondary_structure.hpp"
#include "app/old_app/model/selection.hpp"
#include "app/old_app/mvc/mvc_manager.hpp"
#include "app/old_app/generic/base_colorable.hpp"
#include "app/old_app/generic/base_representable.hpp"
#include "app/old_app/representation/representation_manager.hpp"
#include "app/old_app/vtx_app.hpp"
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
							  const Color::Rgba &										p_color ) :
			_color( p_color )
		{
			_instantiatedRepresentations.emplace( p_representation );
		}

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseRepresentable, T>::value>>
		explicit ChangeColor( const std::unordered_set<T *> & p_representables, const Color::Rgba & p_color ) :
			_color( p_color )
		{
			for ( T * const representable : p_representables )
			{
				if ( !representable->hasCustomRepresentation() )
				{
					Model::Representation::InstantiatedRepresentation * instantiatedRepresentation
						= VTX::Representation::RepresentationManager::get().instantiateCopy(
							representable->getRepresentation(), *representable, true, false );
				}

				_instantiatedRepresentations.emplace( representable->getCustomRepresentation() );
			}
		}

		void execute()
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( Model::Representation::InstantiatedRepresentation * const representation :
				  _instantiatedRepresentations )
			{
				representation->setColor( _color, false, true );
				molecules.emplace( representation->getConstTarget()->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				molecule->refreshColors();
			}

			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgba														_color;
		std::unordered_set<Model::Representation::InstantiatedRepresentation *> _instantiatedRepresentations
			= std::unordered_set<Model::Representation::InstantiatedRepresentation *>();
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

	class ChangeCylindeColorBlendingMode : public BaseAction
	{
	  public:
		explicit ChangeCylindeColorBlendingMode(
			Model::Representation::InstantiatedRepresentation * const p_representation,
			const Generic::COLOR_BLENDING_MODE &					  p_mode ) :
			_instantiatedRepresentation( p_representation ),
			_mode( p_mode )
		{
		}

		void execute()
		{
			_instantiatedRepresentation->setCylinderColorBlendingMode( _mode );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Generic::COLOR_BLENDING_MODE						  _mode;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeRibbonColorMode : public BaseAction
	{
	  public:
		explicit ChangeRibbonColorMode( Model::Representation::InstantiatedRepresentation * const p_representation,
										const Generic::SECONDARY_STRUCTURE_COLOR_MODE &			  p_colorMode ) :
			_instantiatedRepresentation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute()
		{
			_instantiatedRepresentation->setRibbonColorMode( _colorMode );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE			  _colorMode;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeRibbonColorBlendingMode : public BaseAction
	{
	  public:
		explicit ChangeRibbonColorBlendingMode(
			Model::Representation::InstantiatedRepresentation * const p_representation,
			const Generic::COLOR_BLENDING_MODE &					  p_mode ) :
			_instantiatedRepresentation( p_representation ),
			_mode( p_mode )
		{
		}

		void execute()
		{
			_instantiatedRepresentation->setRibbonColorBlendingMode( _mode );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Generic::COLOR_BLENDING_MODE						  _mode;
		Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};
} // namespace VTX::Action::InstantiatedRepresentation
#endif
