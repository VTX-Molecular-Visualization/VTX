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
#include <app/old/component/chemistry/_fwd.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/core/event/vtx_event.hpp>
#include <vector>

namespace VTX::UI::Widget::Sequence
{
	class MoleculeSequenceWidget : public ViewItemWidget<App::Old::Component::Chemistry::Molecule>
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
		void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override;
		void refresh() override;
		void localize() override;
		void repaintSelection() const;

		void notify( const VTX::App::Old::Core::Event::VTXEvent * const p_event ) override;

	  protected:
		MoleculeSequenceWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void mousePressEvent( QMouseEvent * p_event ) override;
		void mouseMoveEvent( QMouseEvent * p_event ) override;
		void mouseReleaseEvent( QMouseEvent * p_event ) override;

		void _onScrollBarValueChanged();
		void _initLabelName();
		void _updateLabelName( const App::Old::Component::Chemistry::Chain * const p_currentChainDisplayed ) const;
		void _clear();

		App::Old::Component::Chemistry::Chain * _getCurrentChain() const;

	  private:
		QLabel *						   _sequenceLabel	  = nullptr;
		QWidget *						   _scrollAreaContent = nullptr;
		QScrollArea *					   _scrollArea		  = nullptr;
		std::vector<ChainSequenceWidget *> _chainDisplayWidgets;
		std::vector<QLabel *>			   _chainLabelWidgets = std::vector<QLabel *>();
		QHBoxLayout *					   _sequenceLayout	  = nullptr;

		std::vector<App::Old::Component::Chemistry::Residue *> _frameSelection
			= std::vector<App::Old::Component::Chemistry::Residue *>();
		QPoint									   _startPressPosition;
		App::Old::Component::Chemistry::Residue *	   _startResidueHovered				= nullptr;
		const App::Old::Component::Chemistry::Residue * _closestResidueFromStartPosition = nullptr;
		QPoint									   _lastDragSelectionPosition;
		App::Old::Component::Chemistry::Residue *	   _lastResidueHovered = nullptr;
		SelectionModifier						   _selectionModifier  = SelectionModifier::None;

		void									   _getFromTo( const App::Old::Component::Chemistry::Residue &				 p_from,
															   const App::Old::Component::Chemistry::Residue &				 p_to,
															   std::vector<App::Old::Component::Chemistry::Residue *> * const _container ) const;
		App::Old::Component::Chemistry::Residue * const _getResidueAtPos( const QPoint & p_pos ) const;
		App::Old::Component::Chemistry::Residue * const _getClosestResidue( const QPoint & p_pos,
																	   const bool	  p_next,
																	   const bool	  p_forceGetValue = false ) const;
		App::Old::Component::Chemistry::Residue * const _getPreviousResidue(
			const App::Old::Component::Chemistry::Residue & p_residue,
			const bool								   p_forceResult ) const;
		App::Old::Component::Chemistry::Residue * const _getNextResidue(
			const App::Old::Component::Chemistry::Residue & p_residue,
			const bool								   p_forceResult ) const;
		QPoint			  _getResiduePos( const App::Old::Component::Chemistry::Residue & p_residue ) const;
		SelectionModifier _getSelectionModifier( const QMouseEvent * const p_event ) const;
		ClickModifier	  _getClickModifier( const QMouseEvent * const p_event ) const;
		void			  _applySelection( const bool p_select, App::Old::Component::Chemistry::Residue * p_residue );
		void			  _applySelection( const bool p_select );

		bool _isSelected( const App::Old::Component::Chemistry::Residue * const residue ) const;
		void _select( std::vector<App::Old::Component::Chemistry::Residue *> & p_residues ) const;
		void _toggleSelect( std::vector<App::Old::Component::Chemistry::Residue *> & p_residues ) const;
		void _unselect( std::vector<App::Old::Component::Chemistry::Residue *> & p_residues, const bool p_checkData ) const;
		void _clearSelection() const;
	};
} // namespace VTX::UI::Widget::Sequence
#endif
