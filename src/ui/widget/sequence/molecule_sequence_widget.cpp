#include "molecule_sequence_widget.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "model/category_enum.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "sequence_display_widget.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "ui/contextual_menu.hpp"
#include "ui/widget_factory.hpp"
#include <QScrollBar>
#include <algorithm>

namespace VTX::UI::Widget::Sequence
{
	MoleculeSequenceWidget::MoleculeSequenceWidget( QWidget * p_parent ) : ViewItemWidget( p_parent )
	{
		_registerEvent( Event::Global::SELECTION_CHANGE );
	}

	void MoleculeSequenceWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SELECTION_CHANGE )
			repaintSelection();
	}

	void MoleculeSequenceWidget::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DISPLAY_NAME_CHANGE )
		{
			const Model::Chain * currentChain = _getCurrentChain();
			_updateLabelName( currentChain );
		}
	}

	void MoleculeSequenceWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setContentsMargins( 0, 0, 0, 0 );

		_sequenceLabel = new QLabel( this );
		_sequenceLabel->setAlignment( Qt::AlignmentFlag::AlignTop );
		_sequenceLabel->setFont( Style::SEQUENCE_DISPLAY_FONT() );

		_scrollAreaContent = new QWidget();
		_scrollAreaContent->setContentsMargins( 0, 0, 0, 0 );
		_sequenceLayout = new QHBoxLayout( _scrollAreaContent );
		_sequenceLayout->setSizeConstraint( QLayout::SetMinAndMaxSize );
		_sequenceLayout->setContentsMargins( 0, 0, 0, 0 );
		_sequenceLayout->setSpacing( 5 );
		_sequenceLayout->addStretch( 1000 );

		_scrollArea = new QScrollArea( this );
		_scrollArea->setContentsMargins( 0, 0, 0, 0 );
		_scrollArea->setWidget( _scrollAreaContent );
		_scrollArea->setFrameShape( QFrame::Shape::NoFrame );
		_scrollArea->horizontalScrollBar()->setObjectName( "SequenceScrollbar" );
		_scrollArea->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents );

		QHBoxLayout * const layout = new QHBoxLayout( this );
		layout->setContentsMargins( 0, 0, 0, 0 );

		layout->addWidget( _sequenceLabel );
		layout->addWidget( _scrollArea );
	}

	void MoleculeSequenceWidget::_setupSlots()
	{
		const QScrollBar * const scrollBar = _scrollArea->horizontalScrollBar();
		connect( scrollBar, &QScrollBar::valueChanged, this, &MoleculeSequenceWidget::_onScrollBarValueChanged );
	}

	void MoleculeSequenceWidget::_onScrollBarValueChanged()
	{
		const Model::Chain * currentChain = _getCurrentChain();
		_updateLabelName( currentChain );
	}

	Model::Chain * MoleculeSequenceWidget::_getCurrentChain() const
	{
		const int value = _scrollArea->horizontalScrollBar()->value();

		// Check first visible
		for ( auto it = _chainDisplayWidgets.rbegin(); it != _chainDisplayWidgets.rend(); it++ )
		{
			const QPoint pos = ( *it )->pos();

			if ( pos.x() < value )
			{
				return ( *it )->getModel();
			}
		}

		return _model->getFirstChain();
	}

	void MoleculeSequenceWidget::_initLabelName()
	{
		const Model::Chain * const chain = _model->getFirstChain();
		_updateLabelName( chain );
	}
	void MoleculeSequenceWidget::_updateLabelName( const Model::Chain * const p_currentChainDisplayed ) const
	{
		std::string txt;
		if ( p_currentChainDisplayed == nullptr )
		{
			txt = _model->getDisplayName();
		}
		else
		{
			txt = _model->getDisplayName() + Style::SEQUENCE_CHAIN_NAME_SEPARATOR + p_currentChainDisplayed->getName()
				  + "-" + CATEGORY_ENUM_STR[ int( p_currentChainDisplayed->getCategoryEnum() ) ]
				  + Style::SEQUENCE_CHAIN_NAME_SEPARATOR;
		}

		_sequenceLabel->setText( QString::fromStdString( txt ) );
	}

	void MoleculeSequenceWidget::localize() {}

	void MoleculeSequenceWidget::_clear()
	{
		for ( ChainSequenceWidget * const chain : _chainDisplayWidgets )
		{
			_sequenceLayout->removeWidget( chain );
			delete chain;
		}
		_chainDisplayWidgets.clear();

		for ( QLabel * const chainLabel : _chainLabelWidgets )
		{
			_sequenceLayout->removeWidget( chainLabel );
			delete chainLabel;
		}
		_chainLabelWidgets.clear();
	}

	void MoleculeSequenceWidget::refresh()
	{
		_clear();

		_initLabelName();

		_chainDisplayWidgets = std::vector<ChainSequenceWidget *>();
		_chainDisplayWidgets.reserve( _model->getChainCount() );

		for ( Model::Chain * const chain : _model->getChains() )
		{
			if ( chain == nullptr )
				continue;

			QLabel * const chainNameWidget = new QLabel( this );
			chainNameWidget->setAlignment( Qt::AlignmentFlag::AlignTop );
			chainNameWidget->setFont( Style::SEQUENCE_DISPLAY_FONT() );
			chainNameWidget->setText( QString::fromStdString(
				Style::SEQUENCE_CHAIN_NAME_SEPARATOR + chain->getName() + "-"
				+ CATEGORY_ENUM_STR[ int( chain->getCategoryEnum() ) ] + Style::SEQUENCE_CHAIN_NAME_SEPARATOR ) );
			_chainLabelWidgets.emplace_back( chainNameWidget );

			ChainSequenceWidget * const chainSequenceDisplay
				= WidgetFactory::get().instantiateWidget<ChainSequenceWidget>( this, "chainSequenceWidget" );
			_chainDisplayWidgets.emplace_back( chainSequenceDisplay );

			chainSequenceDisplay->setModel( chain );

			_sequenceLayout->insertWidget( _sequenceLayout->count() - 1, chainNameWidget );
			_sequenceLayout->insertWidget( _sequenceLayout->count() - 1, chainSequenceDisplay );
		}

		_scrollAreaContent->setFixedSize( _sequenceLayout->sizeHint() );
	}

	void MoleculeSequenceWidget::mousePressEvent( QMouseEvent * p_event )
	{
		ViewItemWidget<Model::Molecule>::mousePressEvent( p_event );

		if ( !p_event->buttons().testFlag( Qt::MouseButton::LeftButton ) )
			return;

		_startPressPosition	 = _scrollAreaContent->mapFromGlobal( p_event->globalPos() );
		_startResidueHovered = _getResidueAtPos( _startPressPosition );
		_closestResidueFromStartPosition
			= _getClosestResidue( _startPressPosition, _startPressPosition.x() < _lastDragSelectionPosition.x(), true );
		_selectionModifier = _getSelectionModifier( p_event );

		const ClickModifier clickModifier = _getClickModifier( p_event );

		const bool targetWasSelected = _startResidueHovered != nullptr && _isSelected( _startResidueHovered );
		const bool targetIsOnlyResidueSelected
			= VTX::Selection::SelectionManager::get().getSelectionModel().getResidueSelectedCount() == 1;

		if ( clickModifier == ClickModifier::Clear )
			_clearSelection();

		if ( clickModifier == ClickModifier::TakeFromTo
			 && !( _startResidueHovered == nullptr && _lastResidueHovered == nullptr ) )
		{
			bool				   takeForward = _lastDragSelectionPosition.x() <= _startPressPosition.x();
			Model::Residue * const closestResidueFromPreviousClick = _getClosestResidue(
				_lastDragSelectionPosition, _startPressPosition.x() > _lastDragSelectionPosition.x(), true );

			const Model::Residue * const fromResidue
				= takeForward ? closestResidueFromPreviousClick : _closestResidueFromStartPosition;
			const Model::Residue * const toResidue
				= takeForward ? _closestResidueFromStartPosition : closestResidueFromPreviousClick;

			_getFromTo( *fromResidue, *toResidue, &_frameSelection );

			const bool select = _startResidueHovered == nullptr ? true : !_isSelected( _startResidueHovered );
			_applySelection( select );

			// If we select, we set the start of selection with the first selected residue of the current
			// block
			if ( select )
			{
				Model::Residue * firstResidueOfSelection = closestResidueFromPreviousClick;
				Model::Residue * previousResidue		 = closestResidueFromPreviousClick;
				const bool		 searchForward			 = _lastDragSelectionPosition.x() > _startPressPosition.x();

				while ( previousResidue != nullptr && _isSelected( previousResidue ) )
				{
					firstResidueOfSelection = previousResidue;
					previousResidue			= searchForward ? _getNextResidue( *firstResidueOfSelection, false )
															: _getPreviousResidue( *firstResidueOfSelection, false );
				}

				_startResidueHovered			 = firstResidueOfSelection;
				_closestResidueFromStartPosition = firstResidueOfSelection;
				_startPressPosition				 = _getResiduePos( *firstResidueOfSelection );
			}
		}
		else if ( _startResidueHovered != nullptr )
		{
			if ( targetWasSelected && targetIsOnlyResidueSelected )
				_frameSelection.push_back( _startResidueHovered );
			else
				_applySelection( true, _startResidueHovered );
		}

		_lastDragSelectionPosition = _startPressPosition;
		_lastResidueHovered		   = _startResidueHovered;
	}
	void MoleculeSequenceWidget::mouseMoveEvent( QMouseEvent * p_event )
	{
		ViewItemWidget<Model::Molecule>::mouseMoveEvent( p_event );

		if ( !p_event->buttons().testFlag( Qt::MouseButton::LeftButton ) )
			return;

		const QPoint	 currentMousePos	   = _scrollAreaContent->mapFromGlobal( p_event->globalPos() );
		const bool		 cursorMoveForward	   = currentMousePos.x() >= _lastDragSelectionPosition.x();
		Model::Residue * currentResidueHovered = _getResidueAtPos( currentMousePos );

		const bool cursorInFrontOfStartClick = currentMousePos.x() > _startPressPosition.x();

		bool sameResidueHovered = currentResidueHovered == _lastResidueHovered;

		// If we jump from a nullresidue to another null residue, we check if the nearest residue is the
		// same
		if ( sameResidueHovered && currentResidueHovered == nullptr )
		{
			const Model::Residue * const closestLastResidueHovered
				= _getClosestResidue( _lastDragSelectionPosition, cursorMoveForward, true );
			const Model::Residue * const closestResidueCurrentlyHovered
				= _getClosestResidue( currentMousePos, !cursorInFrontOfStartClick, true );

			sameResidueHovered = closestLastResidueHovered == closestResidueCurrentlyHovered;
		}

		if ( sameResidueHovered )
		{
			_lastDragSelectionPosition = currentMousePos;
			return;
		}

		const bool switchSideFromStartClick = ( _lastDragSelectionPosition.x() < _startPressPosition.x()
												&& currentMousePos.x() >= _startPressPosition.x() )
											  || ( _lastDragSelectionPosition.x() > _startPressPosition.x()
												   && currentMousePos.x() <= _startPressPosition.x() );

		const Model::Residue * closestLastResidueHovered
			= _getClosestResidue( _lastDragSelectionPosition, cursorMoveForward );

		// If the cursor switch side of the first clicked object, we clear the selection
		if ( switchSideFromStartClick )
		{
			const Model::Residue * const startResidue = _getResidueAtPos( _startPressPosition );

			const Model::Residue * closestResidueCurrentlyHovered
				= _getClosestResidue( currentMousePos, !cursorInFrontOfStartClick, true );
			const Model::Residue * closestOfStart
				= _getClosestResidue( _startPressPosition, !cursorInFrontOfStartClick, true );

			const Model::Residue * fromResidue = cursorInFrontOfStartClick ? closestLastResidueHovered : closestOfStart;
			const Model::Residue * toResidue   = cursorInFrontOfStartClick ? closestOfStart : closestLastResidueHovered;

			_getFromTo( *fromResidue, *toResidue, &_frameSelection );
			_applySelection( false );

			if ( !( startResidue == nullptr && currentResidueHovered == nullptr
					&& closestOfStart == closestResidueCurrentlyHovered ) )
			{
				closestOfStart = _getClosestResidue( _startPressPosition, cursorInFrontOfStartClick, true );

				fromResidue = cursorInFrontOfStartClick ? closestOfStart : closestResidueCurrentlyHovered;
				toResidue	= cursorInFrontOfStartClick ? closestResidueCurrentlyHovered : closestOfStart;
				_getFromTo( *fromResidue, *toResidue, &_frameSelection );

				_applySelection( true );
			}
		}
		else
		{
			const bool addToSelection = ( cursorInFrontOfStartClick && cursorMoveForward )
										|| ( !cursorInFrontOfStartClick && !cursorMoveForward );

			const Model::Residue * fromResidue;
			const Model::Residue * toResidue;

			if ( cursorMoveForward )
			{
				if ( addToSelection && _lastResidueHovered != nullptr )
					fromResidue = _getNextResidue( *closestLastResidueHovered, false );
				else
					fromResidue = closestLastResidueHovered;

				if ( currentResidueHovered != nullptr )
					toResidue = currentResidueHovered;
				else
					toResidue = _getClosestResidue( currentMousePos, !cursorInFrontOfStartClick );

				if ( !addToSelection )
					toResidue = toResidue == nullptr ? nullptr : _getPreviousResidue( *toResidue, false );
			}
			else
			{
				fromResidue = _getClosestResidue( currentMousePos, !cursorInFrontOfStartClick );
				if ( !addToSelection )
				{
					if ( fromResidue == nullptr )
						fromResidue = _getClosestResidue( currentMousePos, !cursorInFrontOfStartClick, true );
					else
						fromResidue = _getNextResidue( *fromResidue, false );
				}

				if ( addToSelection && _lastResidueHovered != nullptr )
					toResidue = _getPreviousResidue( *closestLastResidueHovered, false );
				else
					toResidue = closestLastResidueHovered;
			}

			if ( fromResidue != nullptr && toResidue != nullptr )
			{
				_getFromTo( *fromResidue, *toResidue, &_frameSelection );
				_applySelection( addToSelection );
			}
		}

		_lastDragSelectionPosition = currentMousePos;
		_lastResidueHovered		   = currentResidueHovered;
	}
	void MoleculeSequenceWidget::mouseReleaseEvent( QMouseEvent * p_event )
	{
		const QPoint		   globalMousePos  = p_event->globalPos();
		const QPoint		   currentMousePos = _scrollAreaContent->mapFromGlobal( globalMousePos );
		Model::Residue * const residueHovered  = _getResidueAtPos( currentMousePos );

		if ( p_event->button() == Qt::MouseButton::LeftButton )
		{
			// We update these data here if from to click selection is used and no move has been done
			// (_lastDragSelectionPosition and _lastResidueHovered may not be up to date)
			_lastDragSelectionPosition = currentMousePos;
			_lastResidueHovered		   = residueHovered;
			_frameSelection.clear();
		}
		else if ( p_event->button() == Qt::MouseButton::RightButton )
		{
			if ( residueHovered != nullptr )
			{
				Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();
				if ( selection.isResidueSelected( *residueHovered ) )
				{
					UI::ContextualMenu::pop( UI::ContextualMenu::Menu::Selection, &selection, globalMousePos );
				}
				else
				{
					UI::ContextualMenu::pop( UI::ContextualMenu::Menu::Residue, residueHovered, globalMousePos );
				}
			}
		}
	}

	void MoleculeSequenceWidget::_applySelection( const bool p_select, Model::Residue * p_residue )
	{
		_frameSelection.clear();
		_frameSelection.emplace_back( p_residue );
		_applySelection( p_select );
	}
	void MoleculeSequenceWidget::_applySelection( const bool p_select )
	{
		if ( _frameSelection.size() <= 0 )
			return;

		switch ( _selectionModifier )
		{
		case SelectionModifier::ForceSelect: _select( _frameSelection ); break;
		case SelectionModifier::ToggleSelect: _toggleSelect( _frameSelection ); break;
		case SelectionModifier::ForceUnselect: _unselect( _frameSelection, true ); break;
		default:
			if ( p_select )
				_select( _frameSelection );
			else
				_unselect( _frameSelection, false );
			break;
		}

		_frameSelection.clear();
	}

	MoleculeSequenceWidget::SelectionModifier MoleculeSequenceWidget::_getSelectionModifier(
		const QMouseEvent * const p_event ) const
	{
		SelectionModifier res;

		Qt::KeyboardModifiers modifierFlag = p_event->modifiers();

		bool ctrlModifier  = modifierFlag.testFlag( Qt::KeyboardModifier::ControlModifier );
		bool shiftModifier = modifierFlag.testFlag( Qt::KeyboardModifier::ShiftModifier );
		bool altModifier   = modifierFlag.testFlag( Qt::KeyboardModifier::AltModifier );

		if ( shiftModifier && altModifier )
			res = SelectionModifier::ForceSelect;
		else if ( ( ctrlModifier && altModifier )
				  || ( shiftModifier && _isSelected( _closestResidueFromStartPosition ) ) )
			res = SelectionModifier::ForceUnselect;
		else if ( ( ctrlModifier || shiftModifier ) && _isSelected( _closestResidueFromStartPosition ) )
			res = SelectionModifier::ToggleSelect;
		else
			res = SelectionModifier::None;

		return res;
	}
	MoleculeSequenceWidget::ClickModifier MoleculeSequenceWidget::_getClickModifier(
		const QMouseEvent * const p_event ) const
	{
		ClickModifier res;

		Qt::KeyboardModifiers modifierFlag = p_event->modifiers();

		bool ctrlModifier  = modifierFlag.testFlag( Qt::KeyboardModifier::ControlModifier );
		bool shiftModifier = modifierFlag.testFlag( Qt::KeyboardModifier::ShiftModifier );
		bool altModifier   = modifierFlag.testFlag( Qt::KeyboardModifier::AltModifier );

		if ( shiftModifier )
			res = ClickModifier::TakeFromTo;
		else if ( ctrlModifier || altModifier )
			res = ClickModifier::Append;
		else
			res = ClickModifier::Clear;

		return res;
	}

	Model::Residue * const MoleculeSequenceWidget::_getPreviousResidue( const Model::Residue & p_residue,
																		const bool			   p_forceResult ) const
	{
		const Model::Chain * const chain			 = p_residue.getChainPtr();
		const bool				   isFirstOfItsChain = p_residue.getIndex() == chain->getIndexFirstResidue();

		if ( isFirstOfItsChain )
		{
			const Model::Chain * const previousChain = _model->getPreviousChain( chain->getIndex() );
			if ( previousChain != nullptr )
				return _model->getResidue( previousChain->getIndexLastResidue() );
			else
				return p_forceResult ? _model->getResidue( p_residue.getIndex() ) : nullptr;
		}
		else
		{
			return _model->getPreviousResidue( p_residue.getIndex() );
		}
	}
	Model::Residue * const MoleculeSequenceWidget::_getNextResidue( const Model::Residue & p_residue,
																	const bool			   p_forceResult ) const
	{
		const Model::Chain * const chain			= p_residue.getChainPtr();
		const bool				   isLastOfItsChain = p_residue.getIndex() == ( chain->getIndexLastResidue() );
		if ( isLastOfItsChain )
		{
			const Model::Chain * const nextChain = _model->getNextChain( chain->getIndex() );
			if ( nextChain != nullptr )
				return p_forceResult ? _model->getResidue( nextChain->getIndexFirstResidue() ) : nullptr;
			else
				return p_forceResult ? _model->getResidue( p_residue.getIndex() ) : nullptr;
		}
		else
		{
			return _model->getNextResidue( p_residue.getIndex() );
		}
	}
	QPoint MoleculeSequenceWidget::_getResiduePos( const Model::Residue & p_residue ) const
	{
		const int						  chainIndex	 = p_residue.getChainPtr()->getIndex();
		const ChainSequenceWidget * const sequenceWidget = _chainDisplayWidgets[ chainIndex ];
		return sequenceWidget->getResiduePos( p_residue, _scrollAreaContent );
	}

	void MoleculeSequenceWidget::_getFromTo( const Model::Residue &				   p_from,
											 const Model::Residue &				   p_to,
											 std::vector<Model::Residue *> * const _container ) const
	{
		const Model::Residue & startResidue = p_from;
		const Model::Residue & endResidue	= p_to;

		const Model::Chain * const startChain = startResidue.getChainPtr();
		const Model::Chain * const endChain	  = endResidue.getChainPtr();

		for ( uint iChain = startChain->getIndex(); iChain <= endChain->getIndex(); iChain++ )
		{
			const Model::Chain * const currentChain = _model->getChain( iChain );

			if ( currentChain == nullptr )
				continue;

			const uint startResidueIndex
				= ( currentChain == startChain ) ? startResidue.getIndex() : currentChain->getIndexFirstResidue();
			const uint endResidueIndex
				= ( currentChain == endChain ) ? endResidue.getIndex() : currentChain->getIndexLastResidue();

			for ( uint iResidue = startResidueIndex; iResidue <= endResidueIndex; iResidue++ )
			{
				Model::Residue * const currentResidue = _model->getResidue( iResidue );

				if ( currentResidue == nullptr )
					continue;

				_container->emplace_back( currentResidue );
			}
		}
	}
	Model::Residue * const MoleculeSequenceWidget::_getResidueAtPos( const QPoint & p_pos ) const
	{
		Model::Residue * res = nullptr;

		for ( auto it : _chainDisplayWidgets )
		{
			const qreal chainDisplayStartPos = it->pos().x();
			const qreal chainDisplayEndPos	 = chainDisplayStartPos + it->width();

			if ( chainDisplayStartPos <= p_pos.x() && p_pos.x() < chainDisplayEndPos )
			{
				res = it->getResidueAtPos( p_pos );
				break;
			}
		}

		return res;
	}
	Model::Residue * const MoleculeSequenceWidget::_getClosestResidue( const QPoint & p_pos,
																	   const bool	  p_next,
																	   const bool	  p_forceGetValue ) const
	{
		Model::Residue * res = _getResidueAtPos( p_pos );

		if ( res != nullptr )
			return res;
		else if ( p_forceGetValue )
			res = _chainDisplayWidgets[ 0 ]->getFirstResidue();

		for ( auto it : _chainDisplayWidgets )
		{
			const qreal chainDisplayStartPos = it->getSequenceXmin();
			const qreal chainDisplayEndPos	 = it->getSequenceXmax();

			if ( chainDisplayStartPos <= p_pos.x() && p_pos.x() < chainDisplayEndPos )
			{
				res = &( it->getClosestResidueFromPos( p_pos, p_next ) );
				break;
			}
			else if ( chainDisplayStartPos > p_pos.x() )
			{
				if ( p_next )
					res = it->getFirstResidue();

				break;
			}
			else if ( !p_next )
				res = it->getLastResidue();
		}

		return res;
	}

	bool MoleculeSequenceWidget::_isSelected( const Model::Residue * const residue ) const
	{
		return VTX::Selection::SelectionManager::get().getSelectionModel().isResidueSelected( *residue );
	}
	void MoleculeSequenceWidget::_select( std::vector<Model::Residue *> & p_residues ) const
	{
		VTX_ACTION( new Action::Selection::SelectResidue(
			VTX::Selection::SelectionManager::get().getSelectionModel(), p_residues, true ) );
	}
	void MoleculeSequenceWidget::_unselect( std::vector<Model::Residue *> & p_residues, const bool p_checkData ) const
	{
		VTX_ACTION( new Action::Selection::UnselectResidue(
			VTX::Selection::SelectionManager::get().getSelectionModel(), p_residues, p_checkData ) );
	}
	void MoleculeSequenceWidget::_toggleSelect( std::vector<Model::Residue *> & p_residues ) const
	{
		Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();
		for ( std::vector<Model::Residue *>::const_iterator it = p_residues.cbegin(); it != p_residues.cend(); it++ )
		{
			Model::Residue * const residue = *it;

			if ( selection.isResidueSelected( *residue ) )
			{
				VTX_ACTION( new Action::Selection::UnselectResidue(
					VTX::Selection::SelectionManager::get().getSelectionModel(), *residue ) );
			}
			else
			{
				VTX_ACTION( new Action::Selection::SelectResidue(
					VTX::Selection::SelectionManager::get().getSelectionModel(), *residue ) );
			}
		}
	}
	void MoleculeSequenceWidget::_clearSelection() const
	{
		Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();
		if ( !selectionModel.isEmpty() )
		{
			VTX_ACTION( new Action::Selection::ClearSelection( selectionModel ) );
		}
	}

	void MoleculeSequenceWidget::repaintSelection() const
	{
		for ( auto it : _chainDisplayWidgets )
			it->repaintSelection();
	}

} // namespace VTX::UI::Widget::Sequence
