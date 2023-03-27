#ifndef __VTX_UI_LAYOUT_ATTRIBUTE_LIST__
#define __VTX_UI_LAYOUT_ATTRIBUTE_LIST__

#include "style.hpp"
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QWidget>

namespace VTX::UI::Layout
{
	class AttributeListLayout : public QGridLayout
	{
	  private:
		inline static const Qt::Alignment DEFAULT_ALIGNMENT
			= ( Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop );
		inline static const Qt::Alignment DEFAULT_SECTION_TITLE_ALIGNMENT = ( Qt::AlignmentFlag::AlignLeft );

	  public:
		AttributeListLayout( QWidget * const p_parent = nullptr );
		~AttributeListLayout() = default;

		void addAttribute( QWidget * const p_item, const QString & p_label );
		void addAttribute( QWidget * const p_item, QLabel * const p_label );
		void addAttribute( QWidget * const p_item, const Qt::Alignment & p_alignment = DEFAULT_ALIGNMENT );

		void addSpace( const int p_space = Style::DATA_GRID_DEFAULT_V_SPACER );

		void setAttributeVisibility( QWidget * const p_item, const bool p_visible ) const;
		void enableAttribute( QWidget * const p_item, const bool p_enable ) const;

		void startAttributeSection( const QString & p_title );
		void startAttributeSection( QWidget * const p_titleWidget );
		void startAttributeSection( const QString & p_title, QWidget * const p_titleWidget );
		void finishAttributeSection( const bool p_addSpacer = true );

		void setEndSectionSpacerSize( const int p_endSectionSpacerSize )
		{
			_endSectionSpacerSize = p_endSectionSpacerSize;
		};
		void setIndentSize( const int p_indentSize ) { _indentSize = p_indentSize; };

	  protected:
		int _findWidgetRow( QWidget * const p_item ) const;

	  private:
		int _currentRow	   = 0;
		int _currentIndent = 0;

		int _endSectionSpacerSize = Style::ATTRIBUTE_LIST_LAYOUT_BIG_SECTION_SPACER;
		int _indentSize			  = Style::DATA_GRID_INDENT_SIZE;
	};
} // namespace VTX::UI::Layout
#endif
