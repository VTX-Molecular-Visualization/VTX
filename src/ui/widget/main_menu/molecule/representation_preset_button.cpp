#include "representation_preset_button.hpp"
#include "action/action_manager.hpp"
#include "action/molecule.hpp"
#include "action/representable.hpp"
#include "action/selection.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "representation/representation_manager.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "ui/dialog.hpp"
#include "util/solvent_excluded_surface.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Molecule
{
	void RepresentationPresetButton::_setupUi( const QString & p_name )
	{
		MenuToolButtonWidget::_setupUi( p_name );
		setMaximumWidth( Style::MAIN_MENU_MAX_BUTTON_PRESET_WIDTH );
	}

	void RepresentationPresetButton::_setupSlots()
	{
		MenuToolButtonWidget::_setupSlots();
		connect( this, &QToolButton::clicked, this, &RepresentationPresetButton::_onButtonClicked );
	}

	void RepresentationPresetButton::_onButtonClicked()
	{
		Model::Representation::Representation * representation
			= Model::Representation::RepresentationLibrary::get().getRepresentation( _id );
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		if ( selection.getMoleculesMap().size() > 0 )
		{
			_applyRepresentationOnSelection();
		}
		else
		{
			_applyRepresentationOnScene();
		}
	};

	void RepresentationPresetButton::_applyRepresentationOnSelection()
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		bool	   reallyApplyPreset = true;
		const bool isTryingToApplySES
			= Model::Representation::RepresentationLibrary::get().getRepresentation( _id )->getRepresentationType()
			  == Generic::REPRESENTATION::SES;

		bool isBigSES = false;
		if ( isTryingToApplySES )
		{
			CATEGORY_ENUM categoryEnum;
			for ( const Model::Selection::PairMoleculeIds & moleculeData : selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );
				if ( moleculeData.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
				{
					for ( const Model::Category * const category : molecule.getCategories() )
					{
						categoryEnum = category->getCategoryEnum();

						if ( category->getChains().empty() )
							continue;

						if ( categoryEnum == CATEGORY_ENUM::POLYMER || categoryEnum == CATEGORY_ENUM::CARBOHYDRATE )
						{
							if ( category->getMolecule()->hasSolventExcludedSurface( categoryEnum ) )
								continue;

							if ( Util::SolventExcludedSurface::checkSESMemory( *category ) )
							{
								isBigSES = true;
								break;
							}
						}
					}
				}
				else
				{
					for ( const Model::Selection::PairChainIds & chainData : moleculeData.second )
					{
						Model::Chain * const chain = molecule.getChain( chainData.first );
						if ( chainData.second.getFullySelectedChildCount() == chain->getRealResidueCount() )
						{
							Model::Category * category = chain->getMoleculePtr()->getCategoryFromChain( *chain );
							categoryEnum			   = category->getCategoryEnum();
							if ( categoryEnum == CATEGORY_ENUM::POLYMER || categoryEnum == CATEGORY_ENUM::CARBOHYDRATE )
							{
								if ( category->getMolecule()->hasSolventExcludedSurface( categoryEnum ) == false
									 && Util::SolventExcludedSurface::checkSESMemory( *category ) )
								{
									isBigSES = true;
									break;
								}
							}
						}
						else
						{
							for ( const Model::Selection::PairResidueIds & residueData : chainData.second )
							{
								Model::Residue * const residue = molecule.getResidue( residueData.first );
								Model::Category *	   category
									= residue->getMoleculePtr()->getCategoryFromChain( *residue->getChainPtr() );
								categoryEnum = category->getCategoryEnum();

								if ( category->getMolecule()->hasSolventExcludedSurface( categoryEnum ) )
								{
									// All in the same category.
									break;
								}

								if ( categoryEnum == CATEGORY_ENUM::POLYMER
									 || categoryEnum == CATEGORY_ENUM::CARBOHYDRATE )
								{
									if ( Util::SolventExcludedSurface::checkSESMemory( *category ) )
									{
										isBigSES = true;
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		if ( isTryingToApplySES && isBigSES )
		{
			reallyApplyPreset = Dialog::bigSESComputationWarning();
		}

		if ( reallyApplyPreset )
		{
			VTX_ACTION( new Action::Selection::ChangeRepresentationPreset( selection, _id ) );
		}
	}

	void RepresentationPresetButton::_applyRepresentationOnScene()
	{
		std::unordered_set<Model::Molecule *>  targetedMolecules = std::unordered_set<Model::Molecule *>();
		Object3D::Scene::MapMoleculePtrFloat & mapMolFloat		 = VTXApp::get().getScene().getMolecules();
		for ( const Object3D::Scene::PairMoleculePtrFloat & pair : mapMolFloat )
		{
			targetedMolecules.emplace( pair.first );
		}

		bool	   reallyApplyPreset = true;
		const bool isTryingToApplySES
			= Model::Representation::RepresentationLibrary::get().getRepresentation( _id )->getRepresentationType()
			  == Generic::REPRESENTATION::SES;

		bool isBigSES = false;

		if ( isTryingToApplySES )
		{
			for ( const Model::Molecule * const molecule : targetedMolecules )
			{
				for ( const Model::Category * const category : molecule->getCategories() )
				{
					const CATEGORY_ENUM categoryEnum = category->getCategoryEnum();
					if ( categoryEnum == CATEGORY_ENUM::POLYMER || categoryEnum == CATEGORY_ENUM::CARBOHYDRATE )
					{
						if ( category->getMolecule()->hasSolventExcludedSurface( categoryEnum ) == false
							 && Util::SolventExcludedSurface::checkSESMemory( *category ) )
						{
							isBigSES = true;
							break;
						}
					}
				}

				if ( isBigSES )
					break;
			}
		}

		if ( isTryingToApplySES && isBigSES )
		{
			reallyApplyPreset = Dialog::bigSESComputationWarning();
		}

		if ( reallyApplyPreset )
		{
			VTX_ACTION( new Action::Molecule::ChangeRepresentationPreset( targetedMolecules, _id ) );
		}
	}

} // namespace VTX::UI::Widget::MainMenu::Molecule
