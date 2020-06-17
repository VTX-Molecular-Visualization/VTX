
#include "matrix.h"

const float powersOfTen[] = {1e0f, 1e1f, 1e2f, 1e3f, 1e4f, 1e5f, 1e6f, 1e7f, 1e8f, 1e9f, 1e10f, 1e11f, 1e12f, 1e13f, 1e14f, 1e15f, 1e16f};

static inline float RoundPlaces(float a, int p){
    float s = powersOfTen[p];
    return roundf(a*s) / s;
}
void RoundPlaces(v3 &v, int p){
    v.x = RoundPlaces(v.x, p);
    v.y = RoundPlaces(v.y, p);
    v.z = RoundPlaces(v.z, p);
}


void spline(v3 *&result, const v3 &vec1, const v3 &vec2, const v3 &vec3, const v3 &vec4, int n) {
    float n1 = (float)(n);
    float n2 = (n1 * n1);
    float n3 = (n1 * n1 * n1);
    QSMatrix<float> s(4,4,0.0f);
        s(0,0) = 6 / n3;
        s(1,0) = 6 / n3; s(1,1) = 2 / n2;
        s(2,0) = 1 / n3; s(2,1) = 1 / n2; s(2,2) =  1 / n1;
        s(3,3) = 1.0f;

    float oos = 1.0f / 6.0f;
    QSMatrix<float> b(4,4,0.0f);
        b(0,0) = -1 * oos; b(0,1) = 3 * oos; b(0,2) = -3 * oos; b(0,3) = 1 * oos;
        b(1,0) = 3 * oos; b(1,1) = -6 * oos; b(1,2) = 3 * oos;
        b(2,0) = -3 * oos;  b(2,2) = 3 * oos;
        b(3,0)= oos; b(3,1) = 4 * oos; b(3,2) = oos;

    QSMatrix<float> g(4,4,1.0f);
        g(0,0) = vec1.x; g(0,1) =  vec1.y;  g(0,2) =  vec1.z;
        g(1,0) = vec2.x; g(1,1) =  vec2.y;  g(1,2) =  vec2.z;
        g(2,0) = vec3.x; g(2,1) =  vec3.y;  g(2,2) =  vec3.z;
        g(3,0) = vec4.x; g(3,1) =  vec4.y;  g(3,2) =  vec4.z;

    QSMatrix<float> m = s * b * g;

    v3 v = v3(m(3,0) / m(3,3), m(3,1) / m(3,3), m(3,2) / m(3,3));
    RoundPlaces(v,10);
    result[0] = v;

    for(int k=0;k<n;k++){
        m(3,0) = m(3,0) + m(2,0);
        m(3,1) = m(3,1) + m(2,1);
        m(3,2) = m(3,2) + m(2,2);
        m(3,3) = m(3,3) + m(2,3);
        m(2,0) = m(2,0) + m(1,0);
        m(2,1) = m(2,1) + m(1,1);
        m(2,2) = m(2,2) + m(1,2);
        m(2,3) = m(2,3) + m(1,3);
        m(1,0) = m(1,0) + m(0,0);
        m(1,1) = m(1,1) + m(0,1);
        m(1,2) = m(1,2) + m(0,2);
        m(1,3) = m(1,3) + m(0,3);
        v = v3(m(3,0) / m(3,3), m(3,1) / m(3,3), m(3,2) / m(3,3));
        RoundPlaces(v,10);
        result[k+1] =  v;
    }
}

void splineForPlanes(v3 *&result, const PeptidePlane &p1, const PeptidePlane &p2, 
                    const PeptidePlane &p3,const PeptidePlane &p4, int n, float u, float v) {
    v3 g1 = p1.Position + p1.Side*u + p1.Normal * v;
    v3 g2 = p2.Position + p2.Side*u + p2.Normal * v;
    v3 g3 = p3.Position + p3.Side*u + p3.Normal * v;
    v3 g4 = p4.Position + p4.Side*u + p4.Normal * v;
    spline(result, g1, g2, g3, g4, n);
}

float Linear(float t) {
    return t;
}

float InOutQuad(float t) {
    if (t < 0.5f) {
        return 2 * t * t;
    }
    t = 2*t - 1;
    return -0.5f * (t*(t-2) - 1);
}
float OutCirc(float t) {
    t = t-1;
    return sqrtf(1 - (t * t));
}
float InCirc(float t) {
    return -1 * (sqrtf(1-t*t) - 1);
}