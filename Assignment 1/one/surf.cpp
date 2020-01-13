#include "surf.h"
#include "extra.h"

#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

int normalizeIndex(int index, int size)
{
	if(index < size - 1)
		return index;
	else
		return index - (size - 1);
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.
	
	float segment = 2 * M_PI / steps;

	for(int i = 0; i < steps; i++)
	{
		for(int j = 0; j < profile.size(); j++)
		{
			Matrix3f M = Matrix3f::rotateY(segment * i);
			// 1. Generate vertices S(u,v) = R(v) * q(u)
			Vector3f rotatedVertex = M * profile[j].V;

			surface.VV.push_back(rotatedVertex);

			// 2. Generate normal
			Vector3f rotatedNormal = M.transposed().inverse() * profile[j].N;
			surface.VN.push_back(rotatedNormal);
		}
	}


	// 3. Generate faces
	for(int i1 = 0; i1 < steps; i1++)
	{
		for(int j1 = 0; j1 < profile.size(); j1++)
		{
			
			Tup3u tuple1;
			tuple1[0] = normalizeIndex(i1 * profile.size() + j1, surface.VV.size());
			tuple1[1] = normalizeIndex(i1 * profile.size() + j1 + 1, surface.VV.size());
			tuple1[2] = normalizeIndex((i1 + 1) * profile.size() + j1, surface.VV.size());

			Tup3u tuple2;
			tuple2[0] = normalizeIndex(i1 * profile.size() + j1 + 1, surface.VV.size());
			tuple2[1] = normalizeIndex((i1 + 1) * profile.size() + j1 + 1, surface.VV.size());
			tuple2[2] = normalizeIndex((i1 + 1) * profile.size() + j1, surface.VV.size());

			surface.VF.push_back(tuple1);
			surface.VF.push_back(tuple2);
		}
	}
    //cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;
 
    return surface;
}



Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

    cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
