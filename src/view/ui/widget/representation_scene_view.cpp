#include "representation_scene_view.hpp"
#include "representation/representation_manager.hpp"
#include "style.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				void RepresentationSceneView::notify( const Event::VTXEvent * const p_event ) {}

				void RepresentationSceneView::_setupUi( const QString & p_name )
				{
					setObjectName( "sceneTree" );
					setUniformRowHeights( true );
					setHeaderHidden( true );
					setColumnCount( 1 );
					setSelectionMode( QAbstractItemView::MultiSelection );

					setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents );
					setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Minimum );

					setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
					setVerticalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );

					setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
					_contextMenu = new QMenu( parentWidget() );
					_contextMenu->addAction( "Delete", this, &RepresentationSceneView::_deleteAction, QKeySequence::Delete );

					QTreeWidgetItem * const representationView = new QTreeWidgetItem();

					representationView->setText( 0, QString::fromStdString( _model->getName() ) );
					representationView->setIcon( 0, VTX::Style::IconConst::get().REPRESENTATION_SYMBOL );
					representationView->setCheckState( 0, Qt::CheckState::Checked );

					addTopLevelItem( representationView );

					setMinimumHeight( rowHeight( model()->index( 0, 0 ) ) );
					setMinimumWidth( sizeHintForColumn( 0 ) );
				}
				void RepresentationSceneView::_setupSlots()
				{
					connect( this, &QTreeWidget::customContextMenuRequested, this, &RepresentationSceneView::_onCustomContextMenuCalled );
				}
				void RepresentationSceneView::localize() {}

				void RepresentationSceneView::_onCustomContextMenuCalled( const QPoint & p_clicPos ) { _contextMenu->popup( mapToGlobal( p_clicPos ) ); }

				void RepresentationSceneView::setTarget( Generic::BaseRepresentable & p_renderable )
				{
					if ( _representable != nullptr )
						Representation::RepresentationManager::get().removeRepresentation( _model, _representable );

					_representable = &p_renderable;

					if ( _representable != nullptr )
						Representation::RepresentationManager::get().addRepresentation( _model, _representable );
				}

				void RepresentationSceneView::_deleteAction() { Representation::RepresentationManager::get().removeRepresentation( _model ); }

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
