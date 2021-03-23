#ifndef __VTX_UI_MULTI_DATA_FIELD__
#define __VTX_UI_MULTI_DATA_FIELD__

#ifdef _MSC_VER
#pragma once
#endif

#include <QComboBox>
#include <QLabel>
#include <QWidget>
#include <type_traits>

namespace VTX::UI
{
	class MultiDataField
	{
	  public:
		enum class State
		{
			Uninitialized,
			Identical,
			Different
		};

		MultiDataField() {};
		virtual void resetState() { _state = State::Uninitialized; }
		virtual bool hasIdenticalData() const { return _state == State::Identical; }
		virtual bool hasDifferentData() const { return _state == State::Different; }

	  protected:
		virtual void _displayDifferentsDataFeedback() = 0;

		State _state = State::Uninitialized;
	};

	template<typename T>
	class TMultiDataField : public MultiDataField
	{
	  public:
		TMultiDataField() : MultiDataField() {};
		virtual void updateWithNewValue( const T & p_value ) = 0;
	};

	template<typename T>
	class TMultiDataFieldEquatable : public TMultiDataField<T>
	{
	  public:
		TMultiDataFieldEquatable() : TMultiDataField() {};
		virtual void updateWithNewValue( const T & p_value )
		{
			switch ( _state )
			{
			case State::Uninitialized:
				_setSingleValue( p_value );
				_state = State::Identical;
				break;

			case State::Identical:
				if ( !_isEquals( _getValue(), p_value ) )
				{
					_state = State::Different;
					_displayDifferentsDataFeedback();
				}
				break;

			case State::Different: break;
			}
		}

	  protected:
		virtual const T & _getValue() const					   = 0;
		virtual void	  _setSingleValue( const T & p_value ) = 0;

		virtual bool _isEquals( const T & p_lhs, const T & p_rhs ) const { return p_lhs == p_rhs; }
	};

} // namespace VTX::UI

#endif
