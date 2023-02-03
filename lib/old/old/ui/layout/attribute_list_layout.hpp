#ifndef __VTX_UI_LAYOUT_ATTRIBUTE_LIST__
#define __VTX_UI_LAYOUT_ATTRIBUTE_LIST__

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

	  public:
		AttributeListLayout( QWidget * const p_parent = nullptr );
		~AttributeListLayout() = default;

		void addAttribute( QWidget * const p_item, const QString & p_label );
		void addAttribute( QWidget * const p_item, QLabel * const p_label );
		void addAttribute( QWidget * const p_item, const Qt::Alignment & p_alignment = DEFAULT_ALIGNMENT );

		void setAttributeVisibility( QWidget * const p_item, const bool p_visible ) const;

		void startAttributeSection( const QString & p_title );
		void finishAttributeSection( const bool p_addSpacer );

	  protected:
		int _findWidgetRow( QWidget * const p_item ) const;

	  private:
		int _currentRow = 0;
	};
} // namespace VTX::UI::Layout
#endif
