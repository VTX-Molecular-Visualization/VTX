#ifndef __VTX_UI_WIDGET_CUSTOM_QT_MUTLI_DATA_FIELD__
#define __VTX_UI_WIDGET_CUSTOM_QT_MUTLI_DATA_FIELD__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/multi_data_field.hpp"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class QLabelMultiField : public QLabel, public TMultiDataFieldEquatable<std::string>
	{
	  public:
		QLabelMultiField( QWidget * const p_parent = nullptr ) :
			QLabel( p_parent ), TMultiDataFieldEquatable<std::string>() {};

		void resetState() override
		{
			TMultiDataField::resetState();
			_value = "";
			setText( "" );
		}

	  protected:
		const std::string & _getValue() const override { return _value; }
		void				_setSingleValue( const std::string & p_value ) override
		{
			setText( QString::fromStdString( p_value ) );
			_value = p_value;
		}
		void _displayDifferentsDataFeedback() override { setText( "-" ); }

	  private:
		std::string _value;
	};

	class QPushButtonMultiField : public QPushButton, public TMultiDataFieldEquatable<std::string>
	{
	  public:
		QPushButtonMultiField( QWidget * const p_parent = nullptr ) :
			QPushButton( p_parent ), TMultiDataFieldEquatable<std::string>() {};

		void resetState() override
		{
			TMultiDataField::resetState();
			_value = "";
			setText( "" );
		}

	  protected:
		const std::string & _getValue() const override { return _value; }
		void				_setSingleValue( const std::string & p_value ) override
		{
			setText( QString::fromStdString( p_value ) );
			_value = p_value;
		}
		void _displayDifferentsDataFeedback() override { setText( "-" ); }

	  private:
		std::string _value;
	};

	class QComboBoxMultiField : public QComboBox, public TMultiDataFieldEquatable<int>
	{
	  public:
		QComboBoxMultiField( QWidget * const p_parent = nullptr ) :
			QComboBox( p_parent ), TMultiDataFieldEquatable<int>() {};

		void resetState() override
		{
			TMultiDataFieldEquatable::resetState();
			_index = -1;
		}

	  protected:
		const int & _getValue() const override { return _index; }
		void		_setSingleValue( const int & p_value ) override
		{
			setCurrentIndex( p_value );
			_index = p_value;
		}

		void _displayDifferentsDataFeedback() override
		{
			setCurrentIndex( -1 );
			setCurrentText( "-" );
		}

	  private:
		int _index = -1;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif