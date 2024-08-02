#ifndef __VTX_UI_QT_GATEWAYS_BASE__
#define __VTX_UI_QT_GATEWAYS_BASE__

#include <QtCore/qobject.h>

namespace VTX::UI::QT::Gateways
{
	class BaseGw
	{
	  public:
		BaseGw() {};
		virtual ~BaseGw() = default;
		virtual void registerResource( QObject * qObject ) = 0;

	  protected:
		QObject * _resource;
	};
} // namespace VTX::UI::QT::Gateways

#endif