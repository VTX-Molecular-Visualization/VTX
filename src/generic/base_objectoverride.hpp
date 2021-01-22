#ifndef __VTX_BASE_OBJECT_OVERRIDE__
#define __VTX_BASE_OBJECT_OVERRIDE__

#ifdef _MSC_VER
#pragma once
#endif

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
	} // namespace Generic
} // namespace VTX
#endif
