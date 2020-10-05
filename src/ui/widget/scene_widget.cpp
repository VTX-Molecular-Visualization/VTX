#include "scene_widget.hpp"
#include "base_widget.hpp"
#include "model/molecule.hpp"
#include "view/ui/widget/molecule_structure.hpp"
#include "object3d/scene.hpp"
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

			void SceneWidget::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::MOLECULE_ADDED )
				{
					const Event::VTXEventPtr<Model::Molecule> & event = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
					_verticalLayout->addWidget( (VTX::View::UI::Widget::MoleculeStructure *)( event.ptr->getItem( ID::View::UI_MOLECULE_STRUCTURE ) ) );
				}
				else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
				{
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
