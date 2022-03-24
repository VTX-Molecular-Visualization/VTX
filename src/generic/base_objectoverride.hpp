#ifndef __VTX_BASE_OBJECT_OVERRIDE__
#define __VTX_BASE_OBJECT_OVERRIDE__

namespace VTX
{
	namespace Generic
	{
		class BaseObjectOverride
		{
		  private:
			class OverridedParameter
			{
			  public:
				OverridedParameter() {};
				virtual ~OverridedParameter() {};
			};

			template<typename T>
			class TOverridedParameter : public OverridedParameter
			{
			  public:
				TOverridedParameter( T * const p_value ) : _value( p_value ) {};
				~TOverridedParameter() override { delete _value; };

			  private:
				T * const _value;
			};

		  public:
			~BaseObjectOverride()
			{
				for ( OverridedParameter * item : _data )
					delete item;

				_data.clear();
			}

		  protected:
			template<typename T>
			T * overrideParameter( const T & param )
			{
				T * overridedParam = new T( param );
				_data.emplace_back( new TOverridedParameter<T>( overridedParam ) );

				return overridedParam;
			}

		  private:
			std::vector<OverridedParameter *> _data = std::vector<OverridedParameter *>();
		};

		template<typename T>
		class OverridableParameter final
		{
		  private:
			inline static T defaultValue = T();

		  public:
			OverridableParameter( const T * const p_source ) : _source( p_source ) {};
			OverridableParameter( const T & p_source ) : _source( &p_source ) {};
			OverridableParameter() : _source( &defaultValue ) {};

			~OverridableParameter()
			{
				if ( _override != nullptr )
					delete _override;
			};

			const bool isValid() { return _source != nullptr; }

			const T & getValue() const { return _override != nullptr ? *_override : *_source; }
			T &		  getValue()
			{
				if ( _override == nullptr )
					_override = new T( *_source );

				return *_override;
			}

			void setValue( T & p_value )
			{
				if ( _override == nullptr )
					_override = new T( p_value );
				else
					*_override = p_value;
			}
			void setValue( const T & p_value )
			{
				if ( _override == nullptr )
					_override = new T( p_value );
				else
					*_override = p_value;
			}
			bool isOverrided() const { return _override != nullptr; }

			void resetSource( const T * const p_sourceValue )
			{
				_source	  = p_sourceValue;
				_override = nullptr;
			}

			OverridableParameter<T> & operator=( const OverridableParameter<T> & p_source )
			{
				_source	  = p_source._source;
				_override = p_source._override;

				return *this;
			}

		  private:
			const T * _source	= nullptr;
			T *		  _override = nullptr;
		};

	} // namespace Generic
} // namespace VTX
#endif
