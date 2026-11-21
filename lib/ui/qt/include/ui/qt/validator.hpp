#ifndef VTX_UI_QT_VALIDATOR_HPP
#define VTX_UI_QT_VALIDATOR_HPP

#include <qvalidator.h>
#include <util/constants.hpp>

namespace VTX::UI::QT
{
	// Validator class that enforce uint64 compatible user input
	class QUInt64Validator : public QValidator
	{
	  public:
		QUInt64Validator( uint64_t p_min, uint64_t p_max, QObject * p_parent ); // Allows to set min and max
		QUInt64Validator( QObject * p_parent );									// Min is 0, max is UINT64_MAX

		QValidator::State validate( QString & input, int & pos ) const override;

	  private:
		uint64_t _min = 0;
		uint64_t _max = TypeMax<uint64_t>;
	};
} // namespace VTX::UI::QT

#endif
