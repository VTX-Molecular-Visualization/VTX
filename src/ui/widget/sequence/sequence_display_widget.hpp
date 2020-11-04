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
#include <QPointF>
#include <QString>
#include <unordered_set>
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

					const std::unordered_set<uint> & getSelection() const { return _selection; };
					std::unordered_set<uint> &		 getSelection() { return _selection; };

					const std::vector<UnknownResidueData> & getUnknownResiduesPositions() const { return _positionUnknownResidues; }

				  signals:
					void selectionChanged();

				  protected:
					void mousePressEvent( QMouseEvent * ev ) override;
					void mouseReleaseEvent( QMouseEvent * ev ) override;
					void mouseDoubleClickEvent( QMouseEvent * ev ) override;

					virtual void paintEvent( QPaintEvent * ) override;

				  private:
					QFontMetricsF * _fontMetrics;
					QString			_chainPlainText;
					int *			_charIndexPaintCache	= new int();
					int *			_symbolLengthPaintCache = new int();

					QPointF _startPressPosition;

					const Model::Chain *			_chain					 = nullptr;
					std::unordered_set<uint>		_selection				 = std::unordered_set<uint>();
					std::vector<UnknownResidueData> _positionUnknownResidues = std::vector<UnknownResidueData>();

					uint			 _getCharIndex( const uint p_residueIndex ) const;
					uint			 _getLocalResidueIndex( const uint p_charIndex ) const;
					Model::Residue & _getResidue( const uint p_localResidueIndex ) const;
					uint			 _getResidueIndexFromMousePos( const QPointF & p_position ) const;
					bool			 _checkUnknownResidue( const uint p_localResidueIndex, const UnknownResidueData *& p_unknownResidueData ) const;

					void _getUnderlineData( uint p_localResidueIndex, int * const p_charIndex, int * const p_length ) const;
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
