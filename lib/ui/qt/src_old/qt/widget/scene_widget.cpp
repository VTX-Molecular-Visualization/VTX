#include "qt/widget/scene_widget.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/widget/scene/panel.hpp"
#include "qt/widget_factory.hpp"
#include <QVBoxLayout>
#include <QWidget>
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>

namespace VTX::UI::QT::Widget
{
	SceneWidget::SceneWidget()
	{
		using namespace App::Component::Scene;
		using namespace Scene;

		// Add panel.
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		Panel * const sceneWidget = QT::WidgetFactory::get().instantiateWidget<Panel>( mainWindow, "sceneWidget" );

		mainWindow->referencePanel( "SCENE_PANEL_KEY", sceneWidget );
		mainWindow->addDockWidgetAsTabified(
			sceneWidget, Qt::DockWidgetArea::LeftDockWidgetArea, Qt::Orientation::Vertical, true
		);

		// Connect callbacks.
		App::SCENE().onSceneItemAdded += [ sceneWidget ]( const SceneItemComponent & p_item )
		{
			if ( App::MAIN_REGISTRY().hasComponent<App::Component::Chemistry::Molecule>( p_item ) )
			{
				auto & molecule = App::MAIN_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( p_item );

				// Add with concept.
				sceneWidget->getTreeWidget()->addTopLevelData(
					Tree::TreeItemData { p_item.getName(),
										 Tree::WidgetData( p_item.getPersistentSceneID() ),
										 molecule.getChains().size() },
					Tree::LoadFunc(
						[ &p_item, &molecule ]( const uint p_level, const Tree::WidgetData p_data )
							-> std::vector<Tree::TreeItemData>
						{
							std::vector<Tree::TreeItemData> data;

							switch ( p_level )
							{
							case 0: // Load chains.
							{
								Tree::WidgetData index = 0;
								for ( auto & chain : molecule.getChains() )
								{
									data.push_back( Tree::TreeItemData {
										chain->getName(), index++, chain->getResidueCount() } );
								}
							}
							break;

							case 1: // Load residues.
							{
								auto * chain = molecule.getChain( p_data );
								assert( chain );
								for ( size_t index = chain->getIndexFirstResidue();
									  index <= chain->getIndexLastResidue();
									  ++index )
								{
									auto * residue = molecule.getResidue( index );
									data.push_back( Tree::TreeItemData {
										residue->getName(), index, residue->getAtomCount() } );
								}
							}
							break;

							case 2: // Load atoms.
							{
								auto * residue = molecule.getResidue( p_data );
								assert( residue );
								for ( size_t index = residue->getIndexFirstAtom(); index <= residue->getIndexLastAtom();
									  ++index )
								{
									auto * atom = molecule.getAtom( atom_index_t( index ) );
									data.push_back( Tree::TreeItemData { atom->getName(), index, 0 } );
								}
							}
							break;

							default: assert( true ); break;
							}

							return data;
						}
					)
				);
			}
		};

	} // namespace VTX::UI::QT::Widget
} // namespace VTX::UI::QT::Widget