#ifndef __VTX_UTIL_FMT_GLM__
#define __VTX_UTIL_FMT_GLM__

#include "util/types.hpp"
#include <fmt/format.h>

// Taken from https://github.com/fmtlib/fmt/issues/3306
template<glm::length_t L, typename T>
struct fmt::formatter<glm::vec<L, T>> : fmt::formatter<T>
{
	auto format( const glm::vec<L, T> & p_vec, fmt::format_context & p_ctx ) const
	{
		static_assert( L > 0 && L < 5, "the vec length are not supported!" );

		auto out = std::copy_n( "[ ", 2, p_ctx.out() );
		for ( glm::length_t l = 0; l < L - 1; ++l )
		{
			out = formatter<T>::format( p_vec[ l ], p_ctx );
			out = std::copy_n( ", ", 2, out );
			p_ctx.advance_to( out );
		}
		out = formatter<T>::format( p_vec[ L - 1 ], p_ctx );
		return std::copy_n( " ]", 2, out );
	}
};

template<glm::length_t R, glm::length_t C, typename T>
struct fmt::formatter<glm::mat<R, C, T>> : fmt::formatter<T>
{
	auto format( const glm::mat<R, C, T> & p_mat, fmt::format_context & p_ctx ) const
	{
		static_assert( R > 2 && R < 5 && C > 2 && C < 5, "the mat length is not supported!" );

		auto out = std::copy_n( "[ ", 2, p_ctx.out() );
		for ( glm::length_t r = 0; r < R; ++r )
		{
			for ( glm::length_t c = 0; c < C - 1; ++c )
			{
				out = formatter<T>::format( p_mat[ r ][ c ], p_ctx );
				out = std::copy_n( ", ", 2, out );
				p_ctx.advance_to( out );
			}

			out = formatter<T>::format( p_mat[ r ][ C - 1 ], p_ctx );

			if ( r < R - 1 )
				std::copy_n( " | ", 3, out );
		}

		return std::copy_n( " ]", 2, out );
	}
};

template<typename T>
struct fmt::formatter<glm::qua<T>> : fmt::formatter<T>
{
	auto format( const glm::qua<T> & p_qua, fmt::format_context & p_ctx ) const
	{
		auto out = std::copy_n( "[ ", 2, p_ctx.out() );
		for ( glm::length_t l = 0; l < 3; ++l )
		{
			out = formatter<T>::format( p_qua[ l ], p_ctx );
			out = std::copy_n( ", ", 2, out );
			p_ctx.advance_to( out );
		}

		out = formatter<T>::format( p_qua[ 3 ], p_ctx );
		return std::copy_n( " ]", 2, out );
	}
};

#endif
