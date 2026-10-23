#ifndef __VTX_UI_QT_VALIDATOR__
#define __VTX_UI_QT_VALIDATOR__

#include <qvalidator.h>

namespace VTX::UI::QT
{
	// Validator class that enforce uint64 compatible user input
	class QUInt64Validator : public QValidator
	{
		uint64_t _min = 0;
		uint64_t _max = 0xffffffffffffffff;

	  public:
		QUInt64Validator( uint64_t p_min, uint64_t p_max, QObject * p_parent ); // Allows to set min and max
		QUInt64Validator( QObject * p_parent );									// Min is 0, max is UINT64_MAX

		QValidator::State validate( QString & input, int & pos ) const override;
	};
} // namespace VTX::UI::QT

#endif
