#ifndef __VTX_UI_WIDGET_MOLECULE_SEQUENCE_WIDGET__
#define __VTX_UI_WIDGET_MOLECULE_SEQUENCE_WIDGET__

#include "chain_sequence_widget.hpp"
#include "ui/old_ui/ui/widget/view_item_widget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QScrollArea>
#include <QWidget>
#include <app/event/vtx_event.hpp>
#include <app/model/molecule.hpp>
#include <vector>

namespace VTX::Model
{
	class Chain;
	class Residue;
} // namespace VTX::Model
namespace VTX::UI::Widget::Sequence
{
	class MoleculeSequenceWidget : public ViewItemWidget<Model::Molecule>
	{
		VTX_WIDGET

	  private:
		enum class SelectionModifier
		{
			None,
			ToggleSelect,
			ForceUnselect,
			ForceSelect,
		};
		enum class ClickModifier
		{
			Clear,
			Append,
			TakeFromTo,
		};

	  public:
		void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;
		void refresh() override;
		void localize() override;
		void repaintSelection() const;

		void notify( const VTX::Event::VTXEvent * const p_event ) override;

	  protected:
		MoleculeSequenceWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void mousePressEvent( QMouseEvent * p_event ) override;
		void mouseMoveEvent( QMouseEvent * p_event ) override;
		void mouseReleaseEvent( QMouseEvent * p_event ) override;

		void _onScrollBarValueChanged();
		void _initLabelName();
		void _updateLabelName( const Model::Chain * const p_currentChainDisplayed ) const;
		void _clear();

		Model::Chain * _getCurrentChain() const;

	  private:
		QLabel *						   _sequenceLabel	  = nullptr;
		QWidget *						   _scrollAreaContent = nullptr;
		QScrollArea *					   _scrollArea		  = nullptr;
		std::vector<ChainSequenceWidget *> _chainDisplayWidgets;
		std::vector<QLabel *>			   _chainLabelWidgets = std::vector<QLabel *>();
		QHBoxLayout *					   _sequenceLayout	  = nullptr;

		std::vector<Model::Residue *> _frameSelection = std::vector<Model::Residue *>();
		QPoint						  _startPressPosition;
		Model::Residue *			  _startResidueHovered			   = nullptr;
		const Model::Residue *		  _closestResidueFromStartPosition = nullptr;
		QPoint						  _lastDragSelectionPosition;
		Model::Residue *			  _lastResidueHovered = nullptr;
		SelectionModifier			  _selectionModifier  = SelectionModifier::None;

		void				   _getFromTo( const Model::Residue &				 p_from,
										   const Model::Residue &				 p_to,
										   std::vector<Model::Residue *> * const _container ) const;
		Model::Residue * const _getResidueAtPos( const QPoint & p_pos ) const;
		Model::Residue * const _getClosestResidue( const QPoint & p_pos,
												   const bool	  p_next,
												   const bool	  p_forceGetValue = false ) const;
		Model::Residue * const _getPreviousResidue( const Model::Residue & p_residue, const bool p_forceResult ) const;
		Model::Residue * const _getNextResidue( const Model::Residue & p_residue, const bool p_forceResult ) const;
		QPoint				   _getResiduePos( const Model::Residue & p_residue ) const;
		SelectionModifier	   _getSelectionModifier( const QMouseEvent * const p_event ) const;
		ClickModifier		   _getClickModifier( const QMouseEvent * const p_event ) const;
		void				   _applySelection( const bool p_select, Model::Residue * p_residue );
		void				   _applySelection( const bool p_select );

		bool _isSelected( const Model::Residue * const residue ) const;
		void _select( std::vector<Model::Residue *> & p_residues ) const;
		void _toggleSelect( std::vector<Model::Residue *> & p_residues ) const;
		void _unselect( std::vector<Model::Residue *> & p_residues, const bool p_checkData ) const;
		void _clearSelection() const;
	};
} // namespace VTX::UI::Widget::Sequence
#endif
