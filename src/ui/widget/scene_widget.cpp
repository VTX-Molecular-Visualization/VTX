#include "scene_widget.hpp"
#include "base_widget.hpp"
#include "model/molecule.hpp"
#include "object3d/scene.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/molecule_structure.hpp"
#include "vtx_app.hpp"
#include <QListWidget>
#include <QTreeWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			SceneWidget::SceneWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
			{
				_registerEvent( Event::Global::MOLECULE_ADDED );
				_registerEvent( Event::Global::MOLECULE_REMOVED );
			}

			SceneWidget::~SceneWidget()
			{
				delete _verticalLayout;
				delete _verticalLayoutWidget;
			}

			void SceneWidget::addItem( QWidget * p_item ) { _verticalLayout->addWidget( p_item ); }
			void SceneWidget::removeItem( QWidget * p_item )
			{
				_verticalLayout->removeWidget( p_item );
				delete p_item;
			}

			void SceneWidget ::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::MOLECULE_ADDED )
				{
					const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

					View::UI::Widget::MoleculeStructure * moleculeWidget = new View::UI::Widget::MoleculeStructure( castedEvent.ptr, this );
					castedEvent.ptr->addItem( ID::View::UI_MOLECULE_STRUCTURE, moleculeWidget );

					addItem( moleculeWidget );
				}
				else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
				{
					const Event::VTXEventPtr<Model::Molecule> & castedEvent	   = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
					View::UI::Widget::MoleculeStructure *		moleculeWidget = castedEvent.ptr->getItem<View::UI::Widget::MoleculeStructure>( ID::View::UI_MOLECULE_STRUCTURE );

					removeItem( moleculeWidget );
				}
			}

			void SceneWidget::_setupUi( const QString & p_name )
			{
				BaseManualWidget::_setupUi( p_name );

				_verticalLayoutWidget = new QWidget();
				_verticalLayoutWidget->setObjectName( QString::fromUtf8( "verticalLayoutWidget" ) );
				_verticalLayout = new QVBoxLayout( _verticalLayoutWidget );
				_verticalLayout->setObjectName( QString::fromUtf8( "verticalLayout" ) );

				setWidget( _verticalLayoutWidget );
			}

			void SceneWidget::_setupSlots() {}
			void SceneWidget::localize()
			{
				this->setWindowTitle( "Scene" );
				// this->setWindowTitle( QCoreApplication::translate( "SceneWidget", "Scene", nullptr ) );
			}
		} // namespace Widget
	}	  // namespace UI

} // namespace VTX
