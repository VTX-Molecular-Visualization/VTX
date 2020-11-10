#ifndef __VTX_UI_WIDGET_MOLECULE_SEQUENCE_WIDGET__
#define __VTX_UI_WIDGET_MOLECULE_SEQUENCE_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "chain_sequence_widget.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "ui/widget/view_item_widget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QScrollArea>
#include <QWidget>
#include <vector>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				class MoleculeSequenceWidget : public ViewItemWidget<Model::Molecule>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void receiveEvent( const Event::VTXEvent & p_event ) override;
					void refresh() override;
					void localize() override;

				  protected:
					MoleculeSequenceWidget( QWidget * p_parent );
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

					void mousePressEvent( QMouseEvent * ev ) override;
					void mouseMoveEvent( QMouseEvent * ev ) override;
					void mouseReleaseEvent( QMouseEvent * ev ) override;

					void _onScrollBarValueChanged();
					void _updateLabelName( const Model::Chain & p_currentChainDisplayed );

				  private:
					QLabel *						   _sequenceLabel	  = nullptr;
					QWidget *						   _scrollAreaContent = nullptr;
					QScrollArea *					   _scrollArea		  = nullptr;
					std::vector<ChainSequenceWidget *> _chainDisplayWidgets;
					QHBoxLayout *					   _sequenceLayout = nullptr;

					std::vector<Model::Residue *> _selection	  = std::vector<Model::Residue *>();
					std::vector<Model::Residue *> _frameSelection = std::vector<Model::Residue *>();
					QPoint						  _startPressPosition;
					QPoint						  _lastDragSelectionPosition;
					Model::Residue *			  _lastResidueHovered = nullptr;

					int					   _compareResiduePos( const Model::Residue & p_lhs, const Model::Residue & p_rhs ) const;
					void				   _getFromTo( const Model::Residue & p_from, const Model::Residue & p_to, std::vector<Model::Residue *> * const _container ) const;
					Model::Residue * const _getResidueAtPos( const QPoint & p_pos ) const;
					Model::Residue * const _getClosestResidue( const QPoint & p_pos, const bool p_next, const bool p_forceGetValue = false ) const;

					bool				   _isSelected( const Model::Residue * const residue ) const;
					void				   _select( const std::vector<Model::Residue *> & p_residues );
					void				   _select( Model::Residue * p_residue );
					void				   _unselect( const std::vector<Model::Residue *> & p_residues );
					void				   _unselect( const Model::Residue * const p_residue );
					Model::Residue * const _getPreviousResidue( const Model::Residue & p_residue ) const;
					Model::Residue * const _getNextResidue( const Model::Residue & p_residue ) const;

					void _clearSelection();
					void _repaintSelection() const;
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
