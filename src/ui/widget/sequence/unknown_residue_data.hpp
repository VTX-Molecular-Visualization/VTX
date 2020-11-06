#ifndef __VTX_UI_WIDGET_SEQUENCE_UNKNOWN_RESIDUE_DATA__
#define __VTX_UI_WIDGET_SEQUENCE_UNKNOWN_RESIDUE_DATA__

#ifdef _MSC_VER
#pragma once
#endif

#include <QString>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				struct UnknownResidueData
				{
				  public:
					UnknownResidueData( const uint p_residueIndex, const uint p_charIndex, QString & p_displayedSymbol ) :
						residueIndex( p_residueIndex ), charIndex( p_charIndex ), strSize( p_displayedSymbol.length() )
					{
						_selectionStartIndex = 0;
						while ( p_displayedSymbol.length() < (int)_selectionStartIndex && p_displayedSymbol[ _selectionStartIndex ] != ' ' )
							_selectionStartIndex++;

						const int lastSpaceIndex = p_displayedSymbol.indexOf( ' ', _selectionStartIndex );
						_selectionLength		 = lastSpaceIndex == -1 ? ( strSize - _selectionStartIndex ) : ( lastSpaceIndex - _selectionStartIndex );
					};

					void getSelectionData( int * const p_startIndex, int * const p_length ) const
					{
						*p_startIndex = (int)charIndex + _selectionStartIndex;
						*p_length	  = _selectionLength;
					};

					const uint residueIndex;
					const uint charIndex;
					const uint strSize;

				  private:
					int _selectionStartIndex;
					int _selectionLength;
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
