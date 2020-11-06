#ifndef __VTX_UI_WIDGET_SEQUENCE_DISPLAY_WIDGET__
#define __VTX_UI_WIDGET_SEQUENCE_DISPLAY_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "model/residue.hpp"
#include "unknown_residue_data.hpp"
#include <QFontMetricsF>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QString>
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

					void setupSequence( const Model::Chain & p_molecule );

					Model::Residue & getResidueAtPos( const QPoint & p_pos );

					const std::vector<UnknownResidueData> & getUnknownResiduesPositions() const { return _positionUnknownResidues; }

					void updateSelection( const std::vector<Model::Residue *> & p_selection )
					{
						_moleculeSelection = &p_selection;
						repaint();
					};

				  protected:
					void mouseDoubleClickEvent( QMouseEvent * ev ) override;

					virtual void paintEvent( QPaintEvent * ) override;

				  private:
					QFontMetricsF * _fontMetrics;
					int *			_charIndexPaintCache	= new int();
					int *			_symbolLengthPaintCache = new int();

					const Model::Chain *			_chain					 = nullptr;
					std::vector<UnknownResidueData> _positionUnknownResidues = std::vector<UnknownResidueData>();

					const std::vector<Model::Residue *> * _moleculeSelection = nullptr;

					uint			 _getCharIndex( const uint p_residueIndex ) const;
					uint			 _getLocalResidueIndex( const uint p_charIndex ) const;
					Model::Residue & _getResidue( const uint p_localResidueIndex ) const;
					uint			 _getResidueIndexFromLocaleXPos( const int p_localeXPos ) const;
					uint			 _getLocalResidueIndexFromResidue( const Model::Residue & p_globalResIndex ) const;

					bool _checkUnknownResidue( const uint p_localResidueIndex, const UnknownResidueData *& p_unknownResidueData ) const;

					void _getResidueHighlightData( uint p_localResidueIndex, int * const p_charIndex, int * const p_length ) const;
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
