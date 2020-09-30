#include "molecule_structure.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				MoleculeStructure::MoleculeStructure( Model::Molecule * const p_model, QWidget * p_parent ) : BaseWidget( p_parent ), View::BaseView<Model::Molecule>( p_model )
				{
					setupUi( this );
					// items.append( new QTreeWidgetItem( static_cast<QTreeWidget *>( nullptr ), QStringList( QString( "item: %1" ).arg( i ) ) ) );
				}

				void MoleculeStructure::notify( const Event::VTX_EVENT_MODEL & p_event )
				{
					std::cout << "OUAISOUAISOUAIOS" << std::endl;
					if ( p_event == Event::VTX_EVENT_MODEL::INIT )
					{
						setColumnCount( 1 );
						QTreeWidgetItem * itemMolecule = new QTreeWidgetItem( this, QStringList( QString::fromStdString( _model->getName() ) ) );
						insertTopLevelItem( 0, itemMolecule );
					}
				}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
