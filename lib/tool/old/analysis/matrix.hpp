#ifndef __VTX_UTIL_MATRIX__
#define __VTX_UTIL_MATRIX__

#include <util/math.hpp>
#include <util/types.hpp>
#include <valarray>
#include <vector>

namespace VTX::Analysis
{
	template<typename T>
	class Matrix
	{
	  public:
		static Matrix<T> getIdentity( const size_t p_size )
		{
			Matrix<T> res = Matrix( p_size, p_size );

			for ( size_t i = 0; i < p_size; i++ )
				res.set( i, i, 1 );

			return res;
		}

		static Matrix<T> transpose( const Matrix<T> & p_mat )
		{
			const size_t rowCount	 = p_mat.getRowCount();
			const size_t columnCount = p_mat.getColumnCount();

			Matrix<T> res( columnCount, rowCount );

			for ( int iRow = 0; iRow < rowCount; iRow++ )
				for ( int iCol = 0; iCol < columnCount; iCol++ )
					res.set( iCol, iRow, p_mat.get( iRow, iCol ) );

			return res;
		}

		static Matrix<T> matMult( const Matrix<T> & p_lhs, const Matrix<T> & p_rhs )
		{
			if ( p_lhs.getColumnCount() != p_rhs.getRowCount() )
				return Matrix<T>();

			const int M = int( p_lhs.getRowCount() );
			const int N = int( p_lhs.getColumnCount() );
			const int K = int( p_rhs.getColumnCount() );

			Matrix<T> res( M, K );

			for ( int i = 0; i < M; i++ )
			{
				for ( int j = 0; j < K; j++ )
				{
					T sum = 0;

					for ( int k = 0; k < N; k++ )
						sum += p_lhs.get( i, k ) * p_rhs.get( k, j );

					res.set( i, j, sum );
				}
			}

			return res;
		}

		static Mat3f toMat3f( const Matrix<float> & p_mat )
		{
			assert( p_mat.getRowCount() == 3 && p_mat.getColumnCount() == 3 );

			Mat3f res = Mat3f();

			for ( int i = 0; i < 3; i++ )
				for ( int j = 0; j < 3; j++ )
					res[ i ][ j ] = p_mat.get( i, j );

			return res;
		}

		// Taken from https://math.nist.gov/tnt/download.html (Eigen)
		static void getSVDThin( const Matrix<T> & p_mat, Matrix<T> & p_U, Matrix<T> & p_V )
		{
			const size_t rowCount = p_mat.getRowCount();
			const size_t colCount = p_mat.getColumnCount();
			const size_t diagSize = rowCount < colCount ? rowCount : colCount;

			Matrix<T> workingMatrix = Matrix<T>( p_mat );
			T		  maxCoeff		= 0;

			for ( int i = 0; i < p_mat.getRowCount(); i++ )
			{
				for ( int j = 0; j < p_mat.getColumnCount(); j++ )
				{
					maxCoeff = std::max( maxCoeff, p_mat.get( i, j ) );
				}
			}

			if ( maxCoeff == 0 )
				maxCoeff = 1;

			for ( int i = 0; i < p_mat.getRowCount(); i++ )
			{
				for ( int j = 0; j < p_mat.getColumnCount(); j++ )
				{
					workingMatrix.set( i, j, p_mat.get( i, j ) / maxCoeff );
				}
			}

			p_U = Matrix<T>::getidentity( diagSize );
			p_V = Matrix<T>::getidentity( diagSize );

			// const float maxDiagEntry = m_workMatrix.cwiseAbs().diagonal().maxCoeff();

			// bool finished = false;
			// while ( !finished )
			//{
			//	finished = true;

			//	// do a sweep: for all index pairs (p,q), perform SVD of the corresponding 2x2 sub-matrix
			//	for ( size_t p = 1; p < diagSize; ++p )
			//	{
			//		for ( size_t q = 0; q < p; ++q )
			//		{
			//			// if this 2x2 sub-matrix is not diagonal already...
			//			// notice that this comparison will evaluate to false if any NaN is involved, ensuring that
			//			// NaN's don't keep us iterating forever. Similarly, small denormal numbers are considered
			//			// zero.
			//			float threshold = EPSILON;
			//			if ( std::abs( m_workMatrix.coeff( p, q ) ) > EPSILON
			//				 || std::abs( m_workMatrix.coeff( q, p ) ) > EPSILON )
			//			{
			//				finished = false;
			//				// perform SVD decomposition of 2x2 sub-matrix corresponding to indices p,q to make it
			//				// diagonal the complex to real operation returns true if the updated 2x2 block is not
			//				// already diagonal
			//				if ( !isDiagonal( m_workMatrix, p, q, maxDiagEntry ) )
			//				{
			//					JacobiRotation<RealScalar> j_left, j_right;
			//					internal::real_2x2_jacobi_svd( m_workMatrix, p, q, &j_left, &j_right );

			//					// accumulate resulting Jacobi rotations
			//					m_workMatrix.applyOnTheLeft( p, q, j_left );
			//					if ( computeU() )
			//						m_matrixU.applyOnTheRight( p, q, j_left.transpose() );

			//					m_workMatrix.applyOnTheRight( p, q, j_right );
			//					if ( computeV() )
			//						m_matrixV.applyOnTheRight( p, q, j_right );

			//					// keep track of the largest diagonal coefficient
			//					maxDiagEntry = numext::maxi<RealScalar>(
			//						maxDiagEntry,
			//						numext::maxi<RealScalar>( abs( m_workMatrix.coeff( p, p ) ),
			//												  abs( m_workMatrix.coeff( q, q ) ) ) );
			//				}
			//			}
			//		}
			//	}
			//}
		}

		// static void real_2x2_jacobi_svd( const Matrix<T> & matrix,
		//								 const int					  p,
		//								 const int					  q,
		//								 Matrix<T> &		  j_left,
		//								 Matrix<T> &		  j_right )
		//{
		//	Matrix<T> m = Matrix<T>( 2, 2 );
		//	m.set( 0, 0, matrix.coeff( p, p ) );
		//	m.set( 0, 1, matrix.coeff( p, q ) );
		//	m.set( 1, 0, matrix.coeff( q, p ) );
		//	m.set( 1, 1, matrix.coeff( q, q ) );
		//	Matrix<RealScalar, 2, 2> m;
		//	m << numext::real( matrix.coeff( p, p ) ), numext::real( matrix.coeff( p, q ) ),
		//		numext::real( matrix.coeff( q, p ) ), numext::real( matrix.coeff( q, q ) );
		//	JacobiRotation<RealScalar> rot1;
		//	RealScalar				   t = m.coeff( 0, 0 ) + m.coeff( 1, 1 );
		//	RealScalar				   d = m.coeff( 1, 0 ) - m.coeff( 0, 1 );

		//	if ( std::abs( d ) < (std::numeric_limits<RealScalar>::min)() )
		//	{
		//		rot1.s() = RealScalar( 0 );
		//		rot1.c() = RealScalar( 1 );
		//	}
		//	else
		//	{
		//		// If d!=0, then t/d cannot overflow because the magnitude of the
		//		// entries forming d are not too small compared to the ones forming t.
		//		RealScalar u   = t / d;
		//		RealScalar tmp = std::sqrt( RealScalar( 1 ) + numext::abs2( u ) );
		//		rot1.s()	   = RealScalar( 1 ) / tmp;
		//		rot1.c()	   = u / tmp;
		//	}
		//	m.applyOnTheLeft( 0, 1, rot1 );
		//	j_right->makeJacobi( m, 0, 1 );
		//	*j_left = rot1 * j_right->transpose();
		//}
		// bool JacobiRotation<Scalar>::makeJacobi( const RealScalar & x, const Scalar & y, const RealScalar & z )
		//{
		//	using std::abs;
		//	using std::sqrt;

		//	RealScalar deno = RealScalar( 2 ) * abs( y );
		//	if ( deno < (std::numeric_limits<RealScalar>::min)() )
		//	{
		//		m_c = Scalar( 1 );
		//		m_s = Scalar( 0 );
		//		return false;
		//	}
		//	else
		//	{
		//		RealScalar tau = ( x - z ) / deno;
		//		RealScalar w   = sqrt( numext::abs2( tau ) + RealScalar( 1 ) );
		//		RealScalar t;
		//		if ( tau > RealScalar( 0 ) )
		//		{
		//			t = RealScalar( 1 ) / ( tau + w );
		//		}
		//		else
		//		{
		//			t = RealScalar( 1 ) / ( tau - w );
		//		}
		//		RealScalar sign_t = t > RealScalar( 0 ) ? RealScalar( 1 ) : RealScalar( -1 );
		//		RealScalar n	  = RealScalar( 1 ) / sqrt( numext::abs2( t ) + RealScalar( 1 ) );
		//		m_s				  = -sign_t * ( numext::conj( y ) / abs( y ) ) * abs( t ) * n;
		//		m_c				  = n;
		//		return true;
		//	}
		//}

		// Taken from https://math.nist.gov/tnt/download.html (JAMA/C++ Linear Algebra Package)
		static void getSVD( const Matrix<T> & p_mat, Matrix<T> & p_U, std::vector<T> & p_sigma, Matrix<T> & p_V )
		{
			const int m = int( p_mat.getRowCount() );
			const int n = int( p_mat.getColumnCount() );

			const int nu = min( m, n );

			p_sigma = std::vector<T>( min( m + 1, n ) );
			p_U		= Matrix<T>( m, nu, 0 );
			p_V		= Matrix<T>( n, n );

			std::vector<T> e( n );
			std::vector<T> work( m );
			Matrix<T>	   A( p_mat );
			const bool	   wantu = true; /* boolean */
			const bool	   wantv = true; /* boolean */
			int			   i = 0, j = 0, k = 0;

			// Reduce A to bidiagonal form, storing the diagonal elements
			// in s and the super-diagonal elements in e.

			const int nct = min( m - 1, n );
			const int nrt = max( 0, min( n - 2, m ) );
			for ( k = 0; k < max( nct, nrt ); k++ )
			{
				if ( k < nct )
				{
					// Compute the transformation for the k-th column and
					// place the k-th diagonal in s[k].
					// Compute 2-norm of k-th column without under/overflow.
					p_sigma[ k ] = 0;
					for ( i = k; i < m; i++ )
					{
						p_sigma[ k ] = hypot( p_sigma[ k ], A.get( i, k ) );
					}
					if ( p_sigma[ k ] != 0.0 )
					{
						if ( A.get( k, k ) < 0.0 )
						{
							p_sigma[ k ] = -p_sigma[ k ];
						}
						for ( i = k; i < m; i++ )
						{
							A.set( i, k, A.get( i, k ) / p_sigma[ k ] );
						}
						A.add( k, k, 1.0 );
					}
					p_sigma[ k ] = -p_sigma[ k ];
				}
				for ( j = k + 1; j < n; j++ )
				{
					if ( ( k < nct ) && ( p_sigma[ k ] != 0.0 ) )
					{
						// Apply the transformation.

						double t = 0;
						for ( i = k; i < m; i++ )
						{
							t += A.get( i, k ) * A.get( i, j );
						}
						t = -t / A.get( k, k );
						for ( i = k; i < m; i++ )
						{
							A.add( i, j, t * A.get( i, k ) );
						}
					}

					// Place the k-th row of A into e for the
					// subsequent calculation of the row transformation.

					e[ j ] = A.get( k, j );
				}
				if ( wantu && ( k < nct ) )
				{
					// Place the transformation in U for subsequent back
					// multiplication.

					for ( i = k; i < m; i++ )
					{
						p_U.set( i, k, A.get( i, k ) );
					}
				}
				if ( k < nrt )
				{
					// Compute the k-th row transformation and place the
					// k-th super-diagonal in e[k].
					// Compute 2-norm without under/overflow.
					e[ k ] = 0;
					for ( i = k + 1; i < n; i++ )
					{
						e[ k ] = hypot( e[ k ], e[ i ] );
					}
					if ( e[ k ] != 0.0 )
					{
						if ( e[ k + 1 ] < 0.0 )
						{
							e[ k ] = -e[ k ];
						}
						for ( i = k + 1; i < n; i++ )
						{
							e[ i ] /= e[ k ];
						}
						e[ k + 1 ] += 1.0;
					}
					e[ k ] = -e[ k ];
					if ( ( k + 1 < m ) & ( e[ k ] != 0.0 ) )
					{
						// Apply the transformation.

						for ( i = k + 1; i < m; i++ )
						{
							work[ i ] = 0.0;
						}
						for ( j = k + 1; j < n; j++ )
						{
							for ( i = k + 1; i < m; i++ )
							{
								work[ i ] += e[ j ] * A.get( i, j );
							}
						}
						for ( j = k + 1; j < n; j++ )
						{
							double t = -e[ j ] / e[ k + 1 ];
							for ( i = k + 1; i < m; i++ )
							{
								A.add( i, j, t * work[ i ] );
							}
						}
					}
					if ( wantv )
					{
						// Place the transformation in V for subsequent
						// back multiplication.

						for ( i = k + 1; i < n; i++ )
						{
							p_V.set( i, k, e[ i ] );
						}
					}
				}
			}

			// Set up the final bidiagonal matrix or order p.

			int p = min( n, m + 1 );
			if ( nct < n )
			{
				p_sigma[ nct ] = A.get( nct, nct );
			}
			if ( m < p )
			{
				p_sigma[ p - 1 ] = 0.0;
			}
			if ( nrt + 1 < p )
			{
				e[ nrt ] = A.get( nrt, p - 1 );
			}
			e[ p - 1 ] = 0.0;

			// If required, generate U.

			if ( wantu )
			{
				for ( j = nct; j < nu; j++ )
				{
					for ( i = 0; i < m; i++ )
					{
						p_U.set( i, j, 0.0 );
					}
					p_U.set( j, j, 1.0 );
				}
				for ( k = nct - 1; k >= 0; k-- )
				{
					if ( p_sigma[ k ] != 0.0 )
					{
						for ( j = k + 1; j < nu; j++ )
						{
							double t = 0;
							for ( i = k; i < m; i++ )
							{
								t += p_U.get( i, k ) * p_U.get( i, j );
							}
							t = -t / p_U.get( k, k );
							for ( i = k; i < m; i++ )
							{
								p_U.add( i, j, t * p_U.get( i, k ) );
							}
						}
						for ( i = k; i < m; i++ )
						{
							p_U.set( i, k, -p_U.get( i, k ) );
						}
						p_U.set( k, k, 1.0 + p_U.get( k, k ) );
						for ( i = 0; i < k - 1; i++ )
						{
							p_U.set( i, k, 0.0 );
						}
					}
					else
					{
						for ( i = 0; i < m; i++ )
						{
							p_U.set( i, k, 0.0 );
						}
						p_U.set( k, k, 1.0 );
					}
				}
			}

			// If required, generate V.

			if ( wantv )
			{
				for ( k = n - 1; k >= 0; k-- )
				{
					if ( ( k < nrt ) & ( e[ k ] != 0.0 ) )
					{
						for ( j = k + 1; j < nu; j++ )
						{
							double t = 0;
							for ( i = k + 1; i < n; i++ )
							{
								t += p_V.get( i, k ) * p_V.get( i, j );
							}
							t = -t / p_V.get( k + 1, k );
							for ( i = k + 1; i < n; i++ )
							{
								p_V.add( i, j, t * p_V.get( i, k ) );
							}
						}
					}
					for ( i = 0; i < n; i++ )
					{
						p_V.set( i, k, 0.0 );
					}
					p_V.set( k, k, 1.0 );
				}
			}

			// Main iteration loop for the singular values.

			int	   pp	= p - 1;
			int	   iter = 0;
			double eps	= pow( 2.0, -52.0 );
			while ( p > 0 )
			{
				int k	 = 0;
				int kase = 0;

				// Here is where a test for too many iterations would go.

				// This section of the program inspects for
				// negligible elements in the s and e arrays.  On
				// completion the variables kase and k are set as follows.

				// kase = 1     if s(p) and e[k-1] are negligible and k<p
				// kase = 2     if s(k) is negligible and k<p
				// kase = 3     if e[k-1] is negligible, k<p, and
				//              s(k), ..., s(p) are not negligible (qr step).
				// kase = 4     if e(p-1) is negligible (convergence).

				for ( k = p - 2; k >= -1; k-- )
				{
					if ( k == -1 )
					{
						break;
					}
					if ( abs( e[ k ] ) <= eps * ( abs( p_sigma[ k ] ) + abs( p_sigma[ k + 1 ] ) ) )
					{
						e[ k ] = 0.0;
						break;
					}
				}
				if ( k == p - 2 )
				{
					kase = 4;
				}
				else
				{
					int ks;
					for ( ks = p - 1; ks >= k; ks-- )
					{
						if ( ks == k )
						{
							break;
						}
						double t = ( ks != p ? abs( e[ ks ] ) : 0. ) + ( ks != k + 1 ? abs( e[ ks - 1 ] ) : 0. );
						if ( abs( p_sigma[ ks ] ) <= eps * t )
						{
							p_sigma[ ks ] = 0.0;
							break;
						}
					}
					if ( ks == k )
					{
						kase = 3;
					}
					else if ( ks == p - 1 )
					{
						kase = 1;
					}
					else
					{
						kase = 2;
						k	 = ks;
					}
				}
				k++;

				// Perform the task indicated by kase.

				switch ( kase )
				{
					// Deflate negligible s(p).

				case 1:
				{
					double f   = e[ p - 2 ];
					e[ p - 2 ] = 0.0;
					for ( j = p - 2; j >= k; j-- )
					{
						double t	 = hypot( p_sigma[ j ], f );
						double cs	 = p_sigma[ j ] / t;
						double sn	 = f / t;
						p_sigma[ j ] = t;
						if ( j != k )
						{
							f		   = -sn * e[ j - 1 ];
							e[ j - 1 ] = cs * e[ j - 1 ];
						}
						if ( wantv )
						{
							for ( i = 0; i < n; i++ )
							{
								t = cs * p_V.get( i, j ) + sn * p_V.get( i, p - 1 );
								p_V.set( i, p - 1, -sn * p_V.get( i, j ) + cs * p_V.get( i, p - 1 ) );
								p_V.set( i, j, t );
							}
						}
					}
				}
				break;

					// Split at negligible s(k).

				case 2:
				{
					double f   = e[ k - 1 ];
					e[ k - 1 ] = 0.0;
					for ( j = k; j < p; j++ )
					{
						double t	 = hypot( p_sigma[ j ], f );
						double cs	 = p_sigma[ j ] / t;
						double sn	 = f / t;
						p_sigma[ j ] = t;
						f			 = -sn * e[ j ];
						e[ j ]		 = cs * e[ j ];
						if ( wantu )
						{
							for ( i = 0; i < m; i++ )
							{
								t = cs * p_U.get( i, j ) + sn * p_U.get( i, k - 1 );
								p_U.set( i, k - 1, -sn * p_U.get( i, j ) + cs * p_U.get( i, k - 1 ) );
								p_U.set( i, j, t );
							}
						}
					}
				}
				break;

					// Perform one qr step.

				case 3:
				{
					// Calculate the shift.

					double scale
						= max( max( max( max( abs( p_sigma[ p - 1 ] ), abs( p_sigma[ p - 2 ] ) ), abs( e[ p - 2 ] ) ),
									abs( p_sigma[ k ] ) ),
							   abs( e[ k ] ) );
					double sp	 = p_sigma[ p - 1 ] / scale;
					double spm1	 = p_sigma[ p - 2 ] / scale;
					double epm1	 = e[ p - 2 ] / scale;
					double sk	 = p_sigma[ k ] / scale;
					double ek	 = e[ k ] / scale;
					double b	 = ( ( spm1 + sp ) * ( spm1 - sp ) + epm1 * epm1 ) / 2.0;
					double c	 = ( sp * epm1 ) * ( sp * epm1 );
					double shift = 0.0;
					if ( ( b != 0.0 ) || ( c != 0.0 ) )
					{
						shift = sqrt( b * b + c );
						if ( b < 0.0 )
						{
							shift = -shift;
						}
						shift = c / ( b + shift );
					}
					double f = ( sk + sp ) * ( sk - sp ) + shift;
					double g = sk * ek;

					// Chase zeros.

					for ( j = k; j < p - 1; j++ )
					{
						double t  = hypot( f, g );
						double cs = f / t;
						double sn = g / t;
						if ( j != k )
						{
							e[ j - 1 ] = t;
						}
						f				 = cs * p_sigma[ j ] + sn * e[ j ];
						e[ j ]			 = cs * e[ j ] - sn * p_sigma[ j ];
						g				 = sn * p_sigma[ j + 1 ];
						p_sigma[ j + 1 ] = cs * p_sigma[ j + 1 ];
						if ( wantv )
						{
							for ( i = 0; i < n; i++ )
							{
								t = cs * p_V.get( i, j ) + sn * p_V.get( i, j + 1 );
								p_V.set( i, j + 1, -sn * p_V.get( i, j ) + cs * p_V.get( i, j + 1 ) );
								p_V.set( i, j, t );
							}
						}
						t				 = hypot( f, g );
						cs				 = f / t;
						sn				 = g / t;
						p_sigma[ j ]	 = t;
						f				 = cs * e[ j ] + sn * p_sigma[ j + 1 ];
						p_sigma[ j + 1 ] = -sn * e[ j ] + cs * p_sigma[ j + 1 ];
						g				 = sn * e[ j + 1 ];
						e[ j + 1 ]		 = cs * e[ j + 1 ];
						if ( wantu && ( j < m - 1 ) )
						{
							for ( i = 0; i < m; i++ )
							{
								t = cs * p_U.get( i, j ) + sn * p_U.get( i, j + 1 );
								p_U.set( i, j + 1, -sn * p_U.get( i, j ) + cs * p_U.get( i, j + 1 ) );
								p_U.set( i, j, t );
							}
						}
					}
					e[ p - 2 ] = f;
					iter	   = iter + 1;
				}
				break;

					// Convergence.

				case 4:
				{
					// Make the singular values positive.

					if ( p_sigma[ k ] <= 0.0 )
					{
						p_sigma[ k ] = ( p_sigma[ k ] < 0.0 ? -p_sigma[ k ] : 0.0 );
						if ( wantv )
						{
							for ( i = 0; i <= pp; i++ )
							{
								p_V.set( i, k, -p_V.get( i, k ) );
							}
						}
					}

					// Order the singular values.

					while ( k < pp )
					{
						if ( p_sigma[ k ] >= p_sigma[ k + 1 ] )
						{
							break;
						}
						double t		 = p_sigma[ k ];
						p_sigma[ k ]	 = p_sigma[ k + 1 ];
						p_sigma[ k + 1 ] = t;
						if ( wantv && ( k < n - 1 ) )
						{
							for ( i = 0; i < n; i++ )
							{
								t = p_V.get( i, k + 1 );
								p_V.set( i, k + 1, p_V.get( i, k ) );
								p_V.set( i, k, t );
							}
						}
						if ( wantu && ( k < m - 1 ) )
						{
							for ( i = 0; i < m; i++ )
							{
								t = p_U.get( i, k + 1 );
								p_U.set( i, k + 1, p_U.get( i, k ) );
								p_U.set( i, k, t );
							}
						}
						k++;
					}
					iter = 0;
					p--;
				}
				break;
				}
			}
		}

		// Taken from https://math.nist.gov/tnt/download.html (JAMA/C++ Linear Algebra Package)
		static void getLUDecomposition( const Matrix<T> & m_mat,
										Matrix<T> &		  p_lu,
										std::vector<T> &  p_pivot,
										int &			  p_pivotSign )
		{
			using Util::Math;

			const int m = int( m_mat.getRowCount() );
			const int n = int( m_mat.getColumnCount() );

			p_lu = Matrix<T>( m_mat );

			p_pivot = std::vector<T>();
			p_pivot.resize( m );

			// Use a "left-looking", dot-product, Crout/Doolittle algorithm.
			for ( int i = 0; i < m; i++ )
			{
				p_pivot[ i ] = i;
			}

			p_pivotSign = 1;

			// Outer loop.

			for ( int j = 0; j < n; j++ )
			{
				// Apply previous transformations.

				for ( int i = 0; i < m; i++ )
				{
					// Most of the time is spent in the following dot product.

					const int kmax = Math::min( i, j );
					T		  s	   = T( 0.0 );
					for ( int k = 0; k < kmax; k++ )
					{
						s += p_lu.get( i, k ) * p_lu.get( k, j );
					}

					p_lu.set( i, j, p_lu.get( i, j ) - s );
				}

				// Find pivot and exchange if necessary.

				int p = j;
				for ( int i = j + 1; i < m; i++ )
				{
					if ( abs( p_lu.get( i, j ) ) > abs( p_lu.get( p, j ) ) )
					{
						p = i;
					}
				}
				if ( p != j )
				{
					int k = 0;
					for ( k = 0; k < n; k++ )
					{
						const double t = p_lu.get( p, k );
						p_lu.set( p, k, p_lu.get( j, k ) );
						p_lu.set( j, k, t );
					}
					k			 = p_pivot[ p ];
					p_pivot[ p ] = p_pivot[ j ];
					p_pivot[ j ] = k;
					p_pivotSign	 = -p_pivotSign;
				}

				// Compute multipliers.

				if ( ( j < m ) && ( p_lu.get( j, j ) != 0.0 ) )
				{
					for ( int i = j + 1; i < m; i++ )
					{
						p_lu.set( i, j, p_lu.get( i, j ) / p_lu.get( j, j ) );
					}
				}
			}
		}

		// Taken from https://math.nist.gov/tnt/download.html (JAMA/C++ Linear Algebra Package)
		static float getDeterminant( const Matrix<T> & m_mat )
		{
			if ( m_mat.getRowCount() != m_mat.getColumnCount() )
			{
				return T( 0 );
			}

			T determinant = 1;
			for ( int i = 0; i < m_mat.getRowCount(); i++ )
			{
				determinant *= m_mat.get( i, i );
			}

			return determinant;
		}

	  public:
		Matrix() : Matrix( 0, 0 ) {}

		Matrix( const size_t p_row, const size_t p_col, const T defaultValue = T() )
		{
			_array		 = std::valarray<T>( defaultValue, p_row * p_col );
			_rowCount	 = p_row;
			_columnCount = p_col;
		}

		Matrix( const Matrix<T> & p_from )
		{
			_array		 = std::valarray<T>( p_from.getRowCount() * p_from.getColumnCount() );
			_rowCount	 = p_from.getRowCount();
			_columnCount = p_from.getColumnCount();

			for ( size_t iRow = 0; iRow < _rowCount; iRow++ )
			{
				for ( size_t iCol = 0; iCol < _columnCount; iCol++ )
				{
					const size_t valArrayIndex = _getArrayIndex( iRow, iCol );
					_array[ valArrayIndex ]	   = p_from._array[ valArrayIndex ];
				}
			}
		}

		const T & get( const size_t p_row, const size_t p_col ) const
		{
			return _array[ _getArrayIndex( p_row, p_col ) ];
		};
		void set( const size_t p_row, const size_t p_col, const T & p_value )
		{
			_array[ _getArrayIndex( p_row, p_col ) ] = p_value;
		};
		void add( const size_t p_row, const size_t p_col, const T & p_value )
		{
			_array[ _getArrayIndex( p_row, p_col ) ] += p_value;
		};

		size_t getRowCount() const { return _rowCount; };
		size_t getColumnCount() const { return _columnCount; };

	  private:
		size_t _getArrayIndex( const size_t p_row, const size_t p_col ) const { return _columnCount * p_row + p_col; }

		std::valarray<T> _array;
		size_t			 _rowCount;
		size_t			 _columnCount;
	};
} // namespace VTX::Analysis

#endif
