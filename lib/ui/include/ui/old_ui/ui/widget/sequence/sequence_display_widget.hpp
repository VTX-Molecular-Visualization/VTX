#ifndef __VTX_UI_WIDGET_SEQUENCE_DISPLAY_WIDGET__
#define __VTX_UI_WIDGET_SEQUENCE_DISPLAY_WIDGET__

#include "sequence_chain_data.hpp"
#include <QFontMetricsF>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QString>
#include <app/component/chemistry/_fwd.hpp>
#include <list>
#include <vector>

namespace VTX::UI::Widget::Sequence
{
	class SequenceDisplayWidget : public QLabel
	{
		Q_OBJECT

	  public:
		SequenceDisplayWidget( QWidget * p_parent = nullptr );
		~SequenceDisplayWidget();

		void setupSequence( const SequenceChainData * p_molecule );

		App::Component::Chemistry::Residue * const getResidueAtPos( const QPoint & p_pos );
		App::Component::Chemistry::Residue * const getClosestResidueFromPos( const QPoint & p_pos,
																			 const bool		p_takeForward );

		void updateSelection( const std::vector<App::Component::Chemistry::Residue *> & p_selection )
		{
			_moleculeSelection = &p_selection;
			update();
		};
		QPoint getResiduePos( const App::Component::Chemistry::Residue & p_residue,
							  const QWidget * const						 p_widgetSpace ) const;
		qreal  getSize() const { return _chainData->getCharCount() * _charSizeFromMetrics; };

	  protected:
		void mouseDoubleClickEvent( QMouseEvent * p_event ) override;

		virtual void paintEvent( QPaintEvent * ) override;

	  private:
		QFontMetricsF * _fontMetrics			= nullptr;
		double			_charSizeFromMetrics	= 0.0;
		int *			_charIndexPaintCache	= new int();
		int *			_symbolLengthPaintCache = new int();

		const SequenceChainData * _chainData = nullptr;

		const std::vector<App::Component::Chemistry::Residue *> * _moleculeSelection = nullptr;

		uint									   _getCharIndex( const uint p_residueIndex ) const;
		App::Component::Chemistry::Residue &	   _getResidue( const uint p_localResidueIndex ) const;
		App::Component::Chemistry::Residue * const _getResidueFromLocaleXPos( const int p_localeXPos ) const;
		uint _getLocalResidueIndexFromResidue( const App::Component::Chemistry::Residue & p_globalResIndex ) const;
	};
} // namespace VTX::UI::Widget::Sequence

#endif
