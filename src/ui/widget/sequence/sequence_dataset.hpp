#ifndef __VTX_UI_WIDGET_SEQUENCE_DATASET__
#define __VTX_UI_WIDGET_SEQUENCE_DATASET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "style.hpp"
#include "unknown_residue_data.hpp"
#include "util/ui.hpp"
#include <QFontMetricsF>
#include <QPoint>
#include <QString>
#include <list>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				class SequenceDisplayDataset
				{
				  public:
					SequenceDisplayDataset( const uint p_startIndexChar, const uint p_endIndexChar ) : _startIndexChar( p_startIndexChar ), _endIndexChar( p_endIndexChar ) {};
					virtual const QString & getString() const { return _str; };
					virtual int				getStringSize() const { return _str.size(); };

					virtual const void appendToScale( QString & p_scale, bool p_startBloc ) const {};
					virtual const bool isFinishingBlock( bool p_startBlock ) const { return false; };

					bool isCharIndexInScope( const uint p_charIndex ) { return !_isDecoration && _startIndexChar <= p_charIndex && p_charIndex <= _endIndexChar; }
					virtual Model::Residue * const getResidueAtCharIndex( const uint p_charIndex ) { return nullptr; };

					virtual bool				   isResidueInScope( const uint p_residueIndex ) const { return false; };
					virtual uint				   getCharIndexOfResidue( const uint p_residueIndex ) const { return 0; };
					virtual Model::Residue * const getFirstResidue( const uint p_charIndex ) const { return nullptr; };
					virtual Model::Residue * const getLastResidue( const uint p_charIndex ) const { return nullptr; };

					virtual uint getPaintCharIndex( const uint p_charIndex ) const { return getCharIndexOfResidue( p_charIndex ); };
					virtual uint getPaintLength( const uint p_charIndex ) const { return 0; };
					uint		 getLastCharIndex() const { return _endIndexChar; };

				  protected:
					uint	_startIndexChar;
					uint	_endIndexChar;
					bool	_isDecoration = false;
					QString _str;

					inline uint _drawInScale( QString & p_scale, const std::string & p_label, const uint p_charIndex, const bool p_center ) const
					{
						// const std::string indexTxt	   = std::to_string( firstResidueIndex + i );
						const uint indexTxtSize = (uint)p_label.size();
						const uint indexOffset	= p_center ? ( indexTxtSize - 1 ) / 2 : 0;

						for ( uint j = 0; j < indexTxtSize; j++ )
						{
							const uint index = p_charIndex + j - indexOffset;
							p_scale[ index ] = p_label[ j ];
						}

						return p_charIndex - indexOffset + indexTxtSize - 1;
					}
				};
				class SequenceDisplayDataset_Residue : public SequenceDisplayDataset
				{
				  public:
					SequenceDisplayDataset_Residue( const Model::Chain & p_chain, const uint p_startIndexChar, const uint p_startResidueIndex, const uint p_endResidueIndex );
					const void appendToScale( QString & p_scale, const bool p_startBloc ) const override;

					Model::Residue * const getResidueAtCharIndex( const uint p_charIndex ) override;
					bool				   isResidueInScope( const uint p_residueIndex ) const override;
					uint				   getCharIndexOfResidue( const uint p_residueIndex ) const override;

					Model::Residue * const getFirstResidue( const uint p_charIndex ) const override
					{
						return &( _linkedChain.getMoleculePtr()->getResidue( _linkedChain.getIndexFirstResidue() + _startResidueIndex ) );
					};
					Model::Residue * const getLastResidue( const uint p_charIndex ) const override
					{
						return &( _linkedChain.getMoleculePtr()->getResidue( _linkedChain.getIndexFirstResidue() + _endResidueIndex ) );
					};

					uint getPaintLength( const uint p_charIndex ) const override { return 1; };

				  private:
					const Model::Chain & _linkedChain;
					uint				 _startResidueIndex;
					uint				 _endResidueIndex;
				};
				class SequenceDisplayDataset_MissingResidue : public SequenceDisplayDataset
				{
				  public:
					SequenceDisplayDataset_MissingResidue( const uint p_startIndexChar, const uint p_startResidueIndexInOriginalChain, const uint p_size ) :
						SequenceDisplayDataset( p_startIndexChar, p_startIndexChar + p_size - 1 ), _startResidueIndexInOriginalChain( p_startResidueIndexInOriginalChain ),
						_size( p_size ), _isTooLong( p_size > Style::SEQUENCE_MAX_MISSING_RESIDUE_BEFORE_COLLAPSE )
					{
						if ( _isTooLong )
						{
							// Use of unbreakable space (U+00A0) instead of normal space because rich text in QLabel collapse consecutive space and break scale alignment
							_str		  = QString( " " );
							_endIndexChar = _startIndexChar;
						}
						else
						{
							_str = QString( p_size, '-' );
						}
					};
					const void appendToScale( QString & p_scale, const bool p_startBloc ) const override;

					const bool isFinishingBlock( bool p_startBlock ) const override { return _isTooLong; };

				  private:
					const bool _isTooLong;
					const uint _startResidueIndexInOriginalChain;
					const uint _size;
				};
				class SequenceDisplayDataset_UnknownResidue : public SequenceDisplayDataset
				{
				  public:
					SequenceDisplayDataset_UnknownResidue( Model::Residue & p_residue,
														   const bool		p_spaceBefore,
														   const bool		p_spaceAfter,
														   const uint		p_startIndexChar,
														   const uint		p_residueIndex );

					const void			   appendToScale( QString & p_scale, bool p_startBloc ) const override;
					Model::Residue * const SequenceDisplayDataset_UnknownResidue::getResidueAtCharIndex( const uint p_charIndex ) override;

					virtual bool isResidueInScope( const uint p_residueIndex ) const
					{
						return p_residueIndex == ( _residue->getIndex() - _residue->getChainPtr()->getIndexFirstResidue() );
					};
					virtual uint getCharIndexOfResidue( const uint p_residueIndex ) const { return _startIndexChar; };

					Model::Residue * const getFirstResidue( const uint p_charIndex ) const override { return _residue; };
					Model::Residue * const getLastResidue( const uint p_charIndex ) const override { return _residue; };

					uint getPaintLength( const uint p_charIndex ) const override { return 3; };
					uint getPaintCharIndex( const uint p_charIndex ) const override { return _startIndexChar + _scaleIndexPosition - 1; };

				  private:
					Model::Residue * const _residue;
					uint				   _residueIndex;
					uint				   _scaleIndexPosition;
				};
				class SequenceDisplayDataset_HtmlTag : public SequenceDisplayDataset
				{
				  public:
					SequenceDisplayDataset_HtmlTag( const uint p_charIndex ) : SequenceDisplayDataset( p_charIndex, p_charIndex ) { _isDecoration = true; };
					int getStringSize() const override { return 0; };
				};
				class SequenceDisplayDataset_HtmlColorTag : public SequenceDisplayDataset_HtmlTag
				{
				  public:
					SequenceDisplayDataset_HtmlColorTag( const uint p_charIndex, const Color::Rgb & p_color );
					const bool isFinishingBlock( bool p_startBlock ) const override { return p_startBlock; };
				};
				class SequenceDisplayDataset_EndHtmlColorTag : public SequenceDisplayDataset_HtmlTag
				{
				  public:
					SequenceDisplayDataset_EndHtmlColorTag( const uint p_charIndex ) : SequenceDisplayDataset_HtmlTag( p_charIndex )
					{
						_str = QString();
						Util::UI::appendEndColorHtmlTag( _str );
					};
					virtual const QString & getString() const override { return _str; };
					const bool				isFinishingBlock( bool p_startBlock ) const override { return p_startBlock; };
				};

			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
