#ifndef __VTX_UI_QT_CONCEPTS__
#define __VTX_UI_QT_CONCEPTS__

#include <QWidget>
#include <concepts>

namespace VTX::UI::QT
{
	template<class W>
	concept QTWidgetConcept = std::derived_from<W, QWidget>;

} // namespace VTX::UI::QT

#endif
