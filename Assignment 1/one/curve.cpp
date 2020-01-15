#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}
    

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;

	Vector4f Gx, Gy, Gz;
	float t = 0;
	Matrix4f Bezier = Matrix4f(1, -3, 3, -1,
						  0, 3, -6, 3,
						  0, 0, 3, -3,
	                      0, 0, 0, 1);
	
	Matrix4f B_derivative = Matrix4f(-3, 6, -3, 0,
									 3, -12, 9, 0,
									 0, 6, -9, 0,
								     0, 0, 3, 0);

	// Only 4 control points
	Gx = Vector4f(P[0][0], P[1][0], P[2][0], P[3][0]);
	Gy = Vector4f(P[0][1], P[1][1], P[2][1], P[3][1]);
	Gz = Vector4f(P[0][2], P[1][2], P[2][2], P[3][2]);

	Curve curve;
	Vector3f binormal_0(P[0][0], P[0][1], P[0][2]);
	for (float i = 0; i < steps; i++)
	{
		t = i/steps;
		Vector4f T = Vector4f(1, t, t*t, t*t*t);
		CurvePoint curvePoint;

		// Calculate Point on the curve
		// G * B
		Vector4f intermediateX(Vector4f::dot(Gx, Bezier.getCol(0)), Vector4f::dot(Gx, Bezier.getCol(1)),
			Vector4f::dot(Gx, Bezier.getCol(2)), Vector4f::dot(Gx, Bezier.getCol(3)));
		Vector4f intermediateY(Vector4f::dot(Gy, Bezier.getCol(0)), Vector4f::dot(Gy, Bezier.getCol(1)),
			Vector4f::dot(Gy, Bezier.getCol(2)), Vector4f::dot(Gy, Bezier.getCol(3)));
		Vector4f intermediateZ(Vector4f::dot(Gz, Bezier.getCol(0)), Vector4f::dot(Gz, Bezier.getCol(1)),
			Vector4f::dot(Gz, Bezier.getCol(2)), Vector4f::dot(Gz, Bezier.getCol(3)));
		// q(t) = G * B * T
		float pointX = Vector4f::dot(intermediateX, T);
		float pointY = Vector4f::dot(intermediateY, T);
		float pointZ = Vector4f::dot(intermediateZ, T);
		Vector3f pointOnCurve(pointX, pointY, pointZ);
		curvePoint.V = pointOnCurve;

		// Calculate Tangent
		// G * B'
		Vector4f intermediateX_derivative(Vector4f::dot(Gx, B_derivative.getCol(0)), Vector4f::dot(Gx, B_derivative.getCol(1)), 
			Vector4f::dot(Gx, B_derivative.getCol(2)), Vector4f::dot(Gx, B_derivative.getCol(3)));
		Vector4f intermediateY_derivative(Vector4f::dot(Gy, B_derivative.getCol(0)), Vector4f::dot(Gy, B_derivative.getCol(1)), 
			Vector4f::dot(Gy, B_derivative.getCol(2)), Vector4f::dot(Gy, B_derivative.getCol(3)));
		Vector4f intermediateZ_derivative(Vector4f::dot(Gz, B_derivative.getCol(0)), Vector4f::dot(Gz, B_derivative.getCol(1)),
			Vector4f::dot(Gz, B_derivative.getCol(2)), Vector4f::dot(Gz, B_derivative.getCol(3)));
		// q'(t) = G * B' * T
		float pointX_derivative = Vector4f::dot(intermediateX_derivative, T);
		float pointY_derivative = Vector4f::dot(intermediateY_derivative, T);
		float pointZ_derivative = Vector4f::dot(intermediateZ_derivative, T);
		Vector3f pointOnDerivative(pointX_derivative, pointY_derivative, pointZ_derivative);
		//Vector3f tangent = (pointOnDerivative / pointOnDerivative.abs()).normalized();
		Vector3f tangent = pointOnDerivative.normalized();
		curvePoint.T = tangent;

		// 2-D
		if(Gz == Vector4f())
		{
			// Binormal
			Vector3f binormal(0, 0, 1);
			curvePoint.B = binormal.normalized();

			// Normal
			Vector3f normal = Vector3f::cross(binormal, tangent);
			curvePoint.N = normal.normalized();
			curve.push_back(curvePoint);
		}
		else
		{
			// Normal
			Vector3f normal = Vector3f::cross(binormal_0, tangent);
			curvePoint.N = normal.normalized();

			//Update Binormal
			binormal_0 = Vector3f::cross(tangent, normal).normalized();
			curvePoint.B = binormal_0;
			curve.push_back(curvePoint);
		}
	}
	
    // Right now this will just return this empty curve.
    //return Curve();
	return curve;
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;

	// constant
	float fraction = float(1) / float(6);
	Matrix4f Bspline = Matrix4f(fraction*1, fraction*-3, fraction * 3, fraction*-1,
		fraction * 4, fraction * 0, fraction*-6, fraction * 3,
		fraction * 1, fraction * 3, fraction * 3, fraction*-3,
		fraction * 0, fraction * 0, fraction * 0, fraction * 1);
	Matrix4f Bezier = Matrix4f(1, -3, 3, -1,
							   0, 3, -6, 3,
							   0, 0, 3, -3,
		                       0, 0, 0, 1);

	Curve completeCurve;
	for(unsigned j = 0; j < P.size()-3; j ++)
	{
		Curve localCurve;
		// Bspline can be more than 4 points
		Vector4f Gx, Gy, Gz;
		// Only 4 control points
		Gx = Vector4f(P[j][0], P[j+1][0], P[j+2][0], P[j+3][0]);
		Gy = Vector4f(P[j][1], P[j+1][1], P[j+2][1], P[j+3][1]);
		Gz = Vector4f(P[j][2], P[j+1][2], P[j+2][2], P[j+3][2]);

		// Calculate G * Bspline * Bbezier_inverse
		Vector4f intermediateX(Vector4f::dot(Gx, Bspline.getCol(0)), Vector4f::dot(Gx, Bspline.getCol(1)),
			Vector4f::dot(Gx, Bspline.getCol(2)), Vector4f::dot(Gx, Bspline.getCol(3)));
		Vector4f intermediateY(Vector4f::dot(Gy, Bspline.getCol(0)), Vector4f::dot(Gy, Bspline.getCol(1)),
			Vector4f::dot(Gy, Bspline.getCol(2)), Vector4f::dot(Gy, Bspline.getCol(3)));
		Vector4f intermediateZ(Vector4f::dot(Gz, Bspline.getCol(0)), Vector4f::dot(Gz, Bspline.getCol(1)),
			Vector4f::dot(Gz, Bspline.getCol(2)), Vector4f::dot(Gz, Bspline.getCol(3)));
	    
		Vector4f finalX(Vector4f::dot(intermediateX, Bezier.inverse().getCol(0)), Vector4f::dot(intermediateX, Bezier.inverse().getCol(1)), 
			Vector4f::dot(intermediateX, Bezier.inverse().getCol(2)), Vector4f::dot(intermediateX, Bezier.inverse().getCol(3)));
		Vector4f finalY(Vector4f::dot(intermediateY, Bezier.inverse().getCol(0)), Vector4f::dot(intermediateY, Bezier.inverse().getCol(1)),
			Vector4f::dot(intermediateY, Bezier.inverse().getCol(2)), Vector4f::dot(intermediateY, Bezier.inverse().getCol(3)));
		Vector4f finalZ(Vector4f::dot(intermediateZ, Bezier.inverse().getCol(0)), Vector4f::dot(intermediateZ, Bezier.inverse().getCol(1)),
			Vector4f::dot(intermediateZ, Bezier.inverse().getCol(2)), Vector4f::dot(intermediateZ, Bezier.inverse().getCol(3)));
		
		Vector3f newP1(finalX[0], finalY[0], finalZ[0]);
		Vector3f newP2(finalX[1], finalY[1], finalZ[1]);
		Vector3f newP3(finalX[2], finalY[2], finalZ[2]);
		Vector3f newP4(finalX[3], finalY[3], finalZ[3]);

		vector<Vector3f> controlPoints = { newP1, newP2, newP3, newP4 };

		localCurve = evalBezier(controlPoints, steps);

		completeCurve.insert(completeCurve.end(), localCurve.begin(), localCurve.end());
	}

	return completeCurve;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

