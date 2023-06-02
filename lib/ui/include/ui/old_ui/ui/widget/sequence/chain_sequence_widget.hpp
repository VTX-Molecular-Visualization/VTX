#ifndef __VTX_UI_WIDGET_CHAIN_SEQUENCE_WIDGET__
#define __VTX_UI_WIDGET_CHAIN_SEQUENCE_WIDGET__

#include "sequence_chain_data.hpp"
#include "sequence_display_widget.hpp"
#include "ui/old_ui/ui/widget/view_item_widget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QPoint>
#include <QString>
#include <app/component/chemistry/_fwd.hpp>

namespace VTX::UI::Widget::Sequence
{
	class ChainSequenceWidget : public ViewItemWidget<App::Component::Chemistry::Chain>
	{
		VTX_WIDGET

	  public:
		~ChainSequenceWidget();

		void refresh() override;
		void localize() override;

		App::Component::Chemistry::Residue * const getResidueAtPos( const QPoint & p_pos ) const;
		App::Component::Chemistry::Residue * const getFirstResidue() const;
		App::Component::Chemistry::Residue * const getLastResidue() const;

		QPoint getResiduePos( const App::Component::Chemistry::Residue & p_residue,
							  const QWidget * const						 p_widgetSpace ) const
		{
			return _sequenceDisplayWidget->getResiduePos( p_residue, p_widgetSpace );
		}

		App::Component::Chemistry::Residue & getClosestResidueFromPos( const QPoint & p_pos,
																	   const bool	  p_takeForward ) const;
		int getSequenceXmin() const { return pos().x() + _sequenceDisplayWidget->pos().x(); };
		int getSequenceXmax() const
		{
			return pos().x() + _sequenceDisplayWidget->pos().x() + _sequenceDisplayWidget->getSize() - 1;
		};

		void repaintSelection() const { _sequenceDisplayWidget->update(); };

	  protected:
		ChainSequenceWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QLabel *				_scaleWidget		   = nullptr;
		SequenceDisplayWidget * _sequenceDisplayWidget = nullptr;
		QVBoxLayout *			_layout				   = nullptr;

		SequenceChainData * _chainData = nullptr;
	};
} // namespace VTX::UI::Widget::Sequence
#endif
