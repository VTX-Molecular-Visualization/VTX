#include "scene_item_widget.hpp"
#include "generic/base_visible.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/logger.hpp"
#include "ui/mime_type.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include <QAbstractItemModel>
#include <QDrag>
#include <stack>

namespace VTX::UI::Widget::Scene
{
	SceneItemWidget::SceneItemWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void SceneItemWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setUniformRowHeights( true );
		setHeaderHidden( true );
		setColumnCount( 1 );
		setSelectionMode( QAbstractItemView::ExtendedSelection );
		setSelectionBehavior( SelectionBehavior::SelectRows );

		setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents );
		setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Minimum );

		setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
		setVerticalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );

		setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
		setEditTriggers( EditTrigger::SelectedClicked );
		setExpandsOnDoubleClick( false );

		_createTopLevelObject();
	}

	void SceneItemWidget::_setupSlots()
	{
		connect( this, &QTreeWidget::itemExpanded, this, &SceneItemWidget::_onItemExpanded );
		connect( this, &QTreeWidget::itemCollapsed, this, &SceneItemWidget::_onItemCollapsed );
	}

	void SceneItemWidget::localize() {}

	QTreeWidgetItem * SceneItemWidget::getLastVisibleItem()
	{
		return itemFromIndex( model()->index( model()->rowCount() - 1, model()->columnCount() - 1 ) );
	}

	void SceneItemWidget::updatePosInSceneHierarchy( const int p_position ) { _refreshSize(); };
	void SceneItemWidget::mousePressEvent( QMouseEvent * p_event )
	{
		QTreeWidget::mousePressEvent( p_event );

		if ( p_event->button() == Qt::LeftButton )
			_dragStartPosition = p_event->pos();

		p_event->accept();
	}

	void SceneItemWidget::dragEnterEvent( QDragEnterEvent * p_event )
	{
		BaseManualWidget::dragEnterEvent( p_event );
		if ( p_event->mimeData()->hasFormat(
				 VTX::UI::MimeType::getQStringMimeType( VTX::UI::MimeType::ApplicationMimeType::SCENE_ITEM ) ) )
			p_event->acceptProposedAction();
	}
	void SceneItemWidget::mouseMoveEvent( QMouseEvent * p_event )
	{
		BaseManualWidget::mouseMoveEvent( p_event );

		if ( !( p_event->buttons() & Qt::LeftButton ) )
			return;

		if ( !_canDragObjectAtPos( _dragStartPosition ) )
			return;

		if ( ( p_event->pos() - _dragStartPosition ).manhattanLength() < QApplication::startDragDistance() )
			return;

		QDrag *		drag	 = new QDrag( this );
		QMimeData * mimeData = _getDataForDrag();
		drag->setMimeData( mimeData );

		drag->exec( Qt::CopyAction | Qt::MoveAction );
	}

	void SceneItemWidget::_onItemExpanded( QTreeWidgetItem * const ) { _refreshSize(); }
	void SceneItemWidget::_onItemCollapsed( QTreeWidgetItem * const ) { _refreshSize(); }

	void SceneItemWidget::_refreshItemVisibility( QTreeWidgetItem * const p_itemWidget, const bool p_visible )
	{
		_enableSignals( false );
		p_itemWidget->setCheckState( 0, Util::UI::getCheckState( p_visible ) );
		_enableSignals( true );
	}

	void SceneItemWidget::_enableSignals( const bool p_enable )
	{
		if ( p_enable )
			_enableSignalCounter--;
		else
			_enableSignalCounter++;

		const bool isEnable = _enableSignalCounter == 0;
		blockSignals( !isEnable );
		setUpdatesEnabled( isEnable );
	}

	void SceneItemWidget::_createTopLevelObject()
	{
		const Model::BaseModel & model = MVC::MvcManager::get().getModel<Model::BaseModel>( getModelID() );

		QTreeWidgetItem * const topLevelItem = new QTreeWidgetItem();
		topLevelItem->setFlags( topLevelItem->flags() | Qt::ItemFlag::ItemIsEditable );

		topLevelItem->setData( 0, MODEL_ID_ROLE, QVariant::fromValue<VTX::Model::ID>( model.getId() ) );
		topLevelItem->setText( 0, QString::fromStdString( model.getDefaultName() ) );
		topLevelItem->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( model.getTypeId() ) );

		_refreshItemVisibility( topLevelItem, true );

		addTopLevelItem( topLevelItem );
	}

	void SceneItemWidget::_refreshSize()
	{
		setMinimumHeight( _getMinimumHeight() );
		setMinimumWidth( sizeHintForColumn( 0 ) );
	}
	int SceneItemWidget::_getMinimumHeight() const
	{
		int nbItemDisplayed = 0;

		QModelIndex ptr		   = indexFromItem( topLevelItem( 0 ) );
		uint		childCount = 0;

		std::stack<std::pair<QModelIndex, int>> stack = std::stack<std::pair<QModelIndex, int>>();
		stack.push( std::pair<QModelIndex, int>( ptr, 0 ) );

		while ( stack.size() > 0 && nbItemDisplayed < 4 )
		{
			if ( !ptr.isValid() )
			{
				stack.pop();

				if ( stack.size() > 1 )
				{
					std::pair<QModelIndex, int> pair = stack.top();
					ptr								 = pair.first.model()->index( pair.second, 0, pair.first.parent() );
				}
			}
			else if ( itemFromIndex( ptr )->isHidden() )
			{
				childCount++;
				ptr = ptr.model()->index( childCount, 0, ptr.parent() );
			}
			else if ( isExpanded( ptr ) )
			{
				nbItemDisplayed++;
				stack.push( std::pair<QModelIndex, int>( ptr, childCount ) );
				ptr		   = ptr.model()->index( 0, 0, ptr );
				childCount = 0;
			}
			else
			{
				nbItemDisplayed++;
				childCount++;
				ptr = ptr.model()->index( childCount, 0, ptr.parent() );
			}
		}

		return nbItemDisplayed > 3 ? 0 : ( rowHeight( model()->index( 0, 0 ) ) * nbItemDisplayed );
	}

} // namespace VTX::UI::Widget::Scene
