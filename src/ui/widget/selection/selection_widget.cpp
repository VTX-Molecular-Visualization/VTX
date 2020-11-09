#include "selection_widget.hpp"
#include "style.hpp"
#include "vtx_app.hpp"
#include <QVBoxLayout>
#include <QtGlobal>
#include <string>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Selection
			{
				SelectionWidget::SelectionWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) { _registerEvent( Event::Global::SELECTION_CHANGE ); }

				void SelectionWidget::receiveEvent( const Event::VTXEvent & p_event )
				{
					if ( p_event.name == Event::Global::SELECTION_CHANGE ) {}
				}

				void SelectionWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					QWidget * const		centralWidget = new QWidget( this );
					QVBoxLayout * const layout		  = new QVBoxLayout( centralWidget );

					_selectionTypeComboBox = new QComboBox( this );

					for ( int i = 0; i < (int)VTX::Selection::SelectionType::COUNT; i++ )
						_selectionTypeComboBox->addItem( "" );

					_selectionTypeComboBox->setCurrentIndex( (int)Style::SELECTION_WINDOW_DEFAULT_SELECTION );

					layout->addWidget( _selectionTypeComboBox );
					layout->addStretch( 100 );

					setWidget( centralWidget );
				}

				void SelectionWidget::_setupSlots()
				{
					connect( _selectionTypeComboBox, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, &SelectionWidget::_selectionTypeChangedAction );
				}

				void SelectionWidget::_selectionTypeChangedAction( const int p_newIndex ) { VTX_INFO( std::to_string( p_newIndex ) + " selected." ); }

				void SelectionWidget::localize()
				{
					_populateItemList();

					this->setWindowTitle( "Selection" );
					// this->setWindowTitle( QCoreApplication::translate( "SceneWidget", "Scene", nullptr ) );
				}

				void SelectionWidget::_populateItemList()
				{
					for ( int i = 0; i < (int)VTX::Selection::SelectionType::COUNT; i++ )
					{
						QString								txt;
						const VTX::Selection::SelectionType selectionType = (VTX::Selection::SelectionType)i;

						switch ( selectionType )
						{
						case VTX::Selection::SelectionType::ATOM: txt = "Atom"; break;
						//case VTX::Selection::SelectionType::BOND: txt = "Bond"; break;
						case VTX::Selection::SelectionType::CHAINS: txt = "Chain"; break;
						case VTX::Selection::SelectionType::MOLECULE: txt = "Molecule"; break;
						case VTX::Selection::SelectionType::RESIDUE: txt = "Residue"; break;

						default:
							VTX_WARNING( "Selection " + std::to_string( i ) + " not managed in SelectionWidget::getSelectionTypeText." );
							txt = "<unknown>";
							break;
						}

						_selectionTypeComboBox->setItemText( i, txt );
					}
				}
			} // namespace Selection
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
