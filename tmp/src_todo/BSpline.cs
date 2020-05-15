using UnityEngine;
using System.Collections;

public class BSpline {
	private static int MAX_BEZIER_ORDER = 10; // Maximum curve order.
	
	// Control points
	private float[][] bSplineCPoints;
	
	// Parameters
	bool lookup;
	
	// Auxiliary arrays used in the calculations
	float[][] m3;
	float[] TVector, DTVector;

	// Point and tangent vectors
	float[] pt, tg;
	
	private static float[][] BSplineMatrix = new float[][] {
		new float[] {-1f/6f,	1f/2.0f,	-1f/2f,		1f/6f},
		new float[] { 1f/2f,	-1f,		1f/2f,		0f},
		new float[] {-1f/2f,	0f,			1f/2f,		0f},
		new float[] { 1f/6f,	2f/3f,		1f/6f,		0f}
	};
	
	// The element(i, n) of this array contains the binomial coefficient
	// C(i, n) = n!/(i!(n-i)!)
	private static int[][] BinomialCoefTable = new int[][] {
		new int[] {1, 1, 1, 1,  1,  1,  1,  1,   1,   1},
		new int[] {1, 2, 3, 4,  5,  6,  7,  8,   9,  10},
		new int[] {0, 1, 3, 6, 10, 15, 21, 28,  36,  45},
		new int[] {0, 0, 1, 4, 10, 20, 35, 56,  84, 120},
		new int[] {0, 0, 0, 1,  5, 15, 35, 70, 126, 210},
		new int[] {0, 0, 0, 0,  1,  6, 21, 56, 126, 252},
		new int[] {0, 0, 0, 0,  0,  1,  7, 28,  84, 210},
		new int[] {0, 0, 0, 0,  0,  0,  1,  8,  36, 120},
		new int[] {0, 0, 0, 0,  0,  0,  0,  1,   9,  45},
		new int[] {0, 0, 0, 0,  0,  0,  0,  0,   1,  10},
		new int[] {0, 0, 0, 0,  0,  0,  0,  0,   0,   1}
	};
	
	// The element of this(i, j) of this table contains(i/10)^(3-j).
	private static float[][] TVectorTable = new float[][] {
		//   t^3,  t^2, t^1, t^0
		new float[] {    0f,    0f,   0f,   1f}, // t = 0.0
		new float[] {0.001f, 0.01f, 0.1f,   1f}, // t = 0.1
		new float[] {0.008f, 0.04f, 0.2f,   1f}, // t = 0.2
		new float[] {0.027f, 0.09f, 0.3f,   1f}, // t = 0.3
		new float[] {0.064f, 0.16f, 0.4f,   1f}, // t = 0.4
		new float[] {0.125f, 0.25f, 0.5f,   1f}, // t = 0.5
		new float[] {0.216f, 0.36f, 0.6f,   1f}, // t = 0.6
		new float[] {0.343f, 0.49f, 0.7f,   1f}, // t = 0.7
		new float[] {0.512f, 0.64f, 0.8f,   1f}, // t = 0.8
		new float[] {0.729f, 0.81f, 0.9f,   1f}, // t = 0.9
		new float[] {    1f,    1f,   1f,   1f}  // t = 1.0
	};
	



	// The element of this(i, j) of this table contains(3-j)*(i/10)^(2-j) if
	// j < 3, 0 otherwise.
	private static float[][] DTVectorTable = new float[][] { 
		// 3t^2,  2t^1, t^0
		new float[] {   0f,     0f,   1f, 0f}, // t = 0.0
		new float[] {0.03f,   0.2f,   1f, 0f}, // t = 0.1
		new float[] {0.12f,   0.4f,   1f, 0f}, // t = 0.2
		new float[] {0.27f,   0.6f,   1f, 0f}, // t = 0.3
		new float[] {0.48f,   0.8f,   1f, 0f}, // t = 0.4
		new float[] {0.75f,   1.0f,   1f, 0f}, // t = 0.5
		new float[] {1.08f,   1.2f,   1f, 0f}, // t = 0.6
		new float[] {1.47f,   1.4f,   1f, 0f}, // t = 0.7
		new float[] {1.92f,   1.6f,   1f, 0f}, // t = 0.8
		new float[] {2.43f,   1.8f,   1f, 0f}, // t = 0.9
		new float[] {   3f,     2f,   1f, 0f}  // t = 1.0
	};
	
	private int Factorial(int n) {
		if(n <= 0)
			return 1;
		else
			return n * Factorial(n-1);
	}
	
	// Gives n!/(i!(n-i)!).
	private int BinomialCoef(int i, int n) {
		if((i <= MAX_BEZIER_ORDER) && (n <= MAX_BEZIER_ORDER))
			return BinomialCoefTable[i][n-1];
		else
			return (int) (Factorial(n) / (Factorial(i) * Factorial(n-i) ) );
	}
	
	// Evaluates the Berstein polinomial(i, n) at u.
	private float BersteinPol(int i, int n, float u) {
		return BinomialCoef(i,n) * Mathf.Pow(u,i) * Mathf.Pow(1-u, n-i);
	}
	
	// The derivative of the Berstein polinomial.
	private float DBersteinPol(int i, int n, float u) {
		float s1, s2;
		if(i == 0)
			s1 = 0;
		else
			s1 = i * Mathf.Pow(u, i-1) * Mathf.Pow(1-u, n-i);
		
		if(n == i)
			s2 = 0;
		else
			s2 = -(n-i) * Mathf.Pow(u,i) * Mathf.Pow(1-u, n-i-1);
		return BinomialCoef(i,n) * (s1 + s2);
	}
	
	// Sets lookup table use
	private void InitParameters(bool t) {
		TVector = new float[4];
		DTVector = new float[4];
		pt = new float[3];
		tg = new float[3];  
		lookup = t;
		
		bSplineCPoints = new float[4][]; // should init 3 columns too
		for(int i=0; i<4; i++)
			bSplineCPoints[i] = new float[3];
		
		m3 = new float[4][]; // should init 3 columns too
		for(int i=0; i<4; i++)
			m3[i] = new float[3];
	}
	
	public BSpline() {
		InitParameters(true);
	}
	
	public BSpline(bool t) {
		InitParameters(t);
	}
	
	// Updates the temporal matrix used in 3rd order calculations
	public void UpdateMatrix3() {
		float s;
		int i, j, k;
		for(i=0; i<4; i++) {
			for(j=0; j<3; j++) {
				s = 0;
				for(k=0; k<4; k++)
					s += BSplineMatrix[i][k] * bSplineCPoints[k][j];
				m3[i][j] = s;
			}
		}
	}
	
	// Sets n-th control point
	public void SetCPoint(int n, Vector3 p) {
		bSplineCPoints[n][0] = p.x;
		bSplineCPoints[n][1] = p.y;
		bSplineCPoints[n][2] = p.z;
		UpdateMatrix3();
	}
	
	// Gets the n-th control point (puts it in p)
	public void GetCPoint(int n, out Vector3 p) {
		p.x = bSplineCPoints[n][0];
		p.y = bSplineCPoints[n][1];
		p.z = bSplineCPoints[n][2];
	}
	
	// Replaces the current B-spline control points(0, 1, 2) with(1, 2, 3). This
	// is used when a new spline is to be joined to the recently drawn.
	public void ShiftBSplineCPoints() {
		for (int i=0; i<3; i++) {
			bSplineCPoints[0][i] = bSplineCPoints[1][i];
			bSplineCPoints[1][i] = bSplineCPoints[2][i];
			bSplineCPoints[2][i] = bSplineCPoints[3][i];
		}
		UpdateMatrix3();
	}



	public void CopyCPoints(int n_source, int n_dest) {
		for (int i=0; i<3; i++)
			bSplineCPoints[n_dest][i] = bSplineCPoints[n_source][i];
	}
	
	  // Gives the point on the cubic spline corresponding to t/10(using the lookup table).
	private void BSplinePointI(int t) {
		// Q(u) = TVectorTable[u] * BSplineMatrix * BSplineCPoints
		float s;
		int j, k;
		
		for(j=0; j<3; j++) {
			s = 0;
			for(k=0; k<4; k++){
				s += TVectorTable[t][k] * m3[k][j];
			}
			pt[j] = s;
		}

	}
	
	// Calculates the point on the cubic spline corresponding to the parameter value t in [0, 1].
	private void BSplinePoint(float t) {
		// Q(u) = UVector * BSplineMatrix * BSplineCPoints
		float s;
		int i, j, k;
		
		for(i=0; i<4; i++)
			TVector[i] = Mathf.Pow(t, 3-i);
		
		for(j=0; j<3; j++) {
			s = 0;
			for(k=0; k<4; k++)
				s += TVector[k] * m3[k][j];
			pt[j] = s;
		}
	}
	
	// Calculates the tangent vector of the spline at t.
	private void BSplineTangent(float t) {
		// Q(u) = DTVector * BSplineMatrix * BSplineCPoints
		float s;
		int i, j, k;
		
		for(i=0; i<4; i++) {
			if (i < 3)
				DTVector[i] = (3 - i) * Mathf.Pow(t, 2-i);
			else
				DTVector[i] = 0f;
		}
		
		for(j=0; j<3; j++) {
			s = 0;
			for(k=0; k<4; k++)
				s += DTVector[k] * m3[k][j];
			tg[j] = s;
		}
	}
	
	// Calulates the tangent vector of the spline at t/10.
	private void BSplineTangentI(int t) {
	// Q(u) = DTVectorTable[u] * BSplineMatrix * BSplineCPoints
		float s;
		int j, k;
		
		for(j=0; j<3; j++) {
			s = 0;
			for(k=0; k<4; k++)
				s += DTVectorTable[t][k] * m3[k][j];
			tg[j] = s;
		}
	}



	
	private void EvalPoint(float t) {
		if(lookup)
			BSplinePointI( (int) (10f * t) );
		else
			BSplinePoint(t);
	}
	
	private void EvalTangent(float t) {
		if(lookup)
			BSplineTangentI( (int) (10f * t) );
		else
			BSplineTangent(t);
	}
	
	public void Feval(float t, out Vector3 p) {
		EvalPoint(t);
		p.x = pt[0];
		p.y = pt[1];
		p.z = pt[2];
	}

	public void Feval2(float t, out Vector3 p) {
		EvalPoint(t);
		p.x = Mathf.Pow((1-t), 3)*m3[0][0] + (3*t*Mathf.Pow((1-t),2)*m3[1][0]) + (3*Mathf.Pow(t,2)*((1-t) *m3[2][0])) + (Mathf.Pow(t,3)*m3[3][0]);
		p.y = Mathf.Pow((1-t), 3)*m3[0][1] + (3*t*Mathf.Pow((1-t),2)*m3[1][1]) + (3*Mathf.Pow(t,2)*((1-t) *m3[2][1])) + (Mathf.Pow(t,3)*m3[3][1]);
		p.z = Mathf.Pow((1-t), 3)*m3[0][2] + (3*t*Mathf.Pow((1-t),2)*m3[1][2]) + (3*Mathf.Pow(t,2)*((1-t) *m3[2][2])) + (Mathf.Pow(t,3)*m3[3][2]);
	}

	
	public void Deval(float t, out Vector3 d) {
		EvalTangent(t);
		d.x = tg[0];
		d.y = tg[1];
		d.z = tg[2];
	}
	
	private float FevalX(float t) {
		EvalPoint(t);
		return pt[0];
	}
	
	private float FevalY(float t) {
		EvalPoint(t);
		return pt[1];
	}
	
	private float FevalZ(float t) {
		EvalPoint(t);
		return pt[2];
	}
	
	private float DevalX(float t) {
		EvalTangent(t);
		return tg[0];
	}
	
	private float DevalY(float t) {
		EvalTangent(t);
		return tg[1];
	}
	
	private float DevalZ(float t) {
		EvalTangent(t);
		return tg[2];
	}
	
	
}
