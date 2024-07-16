#ifndef __VTX_TOOL_TOOLS_MDPREP_GATEWAY_ENGINE_JOB_MANAGER__
#define __VTX_TOOL_TOOLS_MDPREP_GATEWAY_ENGINE_JOB_MANAGER__

#include "tools/mdprep/gateway/shared.hpp"
#include <functional>
#include <memory>
#include <util/concepts.hpp>
namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificFormData;
	struct MdParameters;

	// Class responsible for taking care of the task related to an abstract MD engine from the UI perspective
	class EngineJobManager
	{
	  public:
		template<typename T>
			requires( not VTX::SameUnalteredType<EngineJobManager, T> )
		EngineJobManager( T && p_ ) : _ptr( new _wrapper<T>( std::forward<T>( p_ ) ) )
		{
		}

		// Check weither the system is supported by current engine and settings and send results via callback
		inline void checkSystem(
			const MdParameters &		   p_1,
			const EngineSpecificFormData & p_2,
			SystemReportCallback		   p_3
		) const noexcept
		{
			_ptr->checkSystem( p_1, p_2, std::move( p_3 ) );
		}

		// Start preparation of the system and feed the callback with job progression
		inline void startPreparation(
			const MdParameters &		   p_1,
			const EngineSpecificFormData & p_2,
			JobUpdateCallback			   p_3
		) noexcept
		{
			_ptr->startPreparation( p_1, p_2, std::move( p_3 ) );
		}

	  private:
		struct _interface
		{
			~_interface() = default;
			virtual void checkSystem(
				const MdParameters &		   p_1,
				const EngineSpecificFormData & p_2,
				SystemReportCallback		   p_3
			) const noexcept
				= 0;
			virtual void startPreparation(
				const MdParameters &		   p_1,
				const EngineSpecificFormData & p_2,
				JobUpdateCallback			   p_3
			) noexcept
				= 0;
		};
		template<typename T>
		struct _wrapper final : public _interface
		{
			T _obj;

			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}
			virtual void checkSystem(
				const MdParameters &		   p_1,
				const EngineSpecificFormData & p_2,
				SystemReportCallback		   p_3
			) const noexcept override
			{
				_obj.checkSystem( p_1, p_2, std::move( p_3 ) );
			}
			virtual void startPreparation(
				const MdParameters &		   p_1,
				const EngineSpecificFormData & p_2,
				JobUpdateCallback			   p_3
			) noexcept override
			{
				_obj.startPreparation( p_1, p_2, std::move( p_3 ) );
			}
		};
		std::unique_ptr<_interface> _ptr = nullptr;
	};
} // namespace VTX::Tool::Mdprep::Gateway

#endif
