#ifndef __VTX_UI_WIDGET_SEQUENCE_DISPLAY_WIDGET__
#define __VTX_UI_WIDGET_SEQUENCE_DISPLAY_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "model/residue.hpp"
#include "sequence_chain_data.hpp"
#include <QFontMetricsF>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QString>
#include <list>
#include <vector>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				class SequenceDisplayWidget : public QLabel
				{
					Q_OBJECT

				  public:
					SequenceDisplayWidget( QWidget * p_parent = nullptr );
					~SequenceDisplayWidget();

					void setupSequence( const SequenceChainData * p_molecule );

					Model::Residue * const getResidueAtPos( const QPoint & p_pos );
					Model::Residue * const getClosestResidueFromPos( const QPoint & p_pos, const bool p_takeForward );

					void updateSelection( const std::vector<Model::Residue *> & p_selection )
					{
						_moleculeSelection = &p_selection;
						repaint();
					};
					QPoint getResiduePos( const Model::Residue & p_residue, const QWidget * const p_widgetSpace ) const;
					qreal  getSize() const { return _chainData->getCharCount() * _fontMetrics->averageCharWidth(); };

				  protected:
					void mouseDoubleClickEvent( QMouseEvent * p_event ) override;

					virtual void paintEvent( QPaintEvent * ) override;

				  private:
					QFontMetricsF * _fontMetrics;
					int *			_charIndexPaintCache	= new int();
					int *			_symbolLengthPaintCache = new int();

					const SequenceChainData * _chainData = nullptr;

					const std::vector<Model::Residue *> * _moleculeSelection = nullptr;

					uint				   _getCharIndex( const uint p_residueIndex ) const;
					Model::Residue &	   _getResidue( const uint p_localResidueIndex ) const;
					Model::Residue * const _getResidueFromLocaleXPos( const int p_localeXPos ) const;
					uint				   _getLocalResidueIndexFromResidue( const Model::Residue & p_globalResIndex ) const;
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
