#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
using namespace std;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;

// You will need more global variables to implement color and position changes
float deltaX = 0;
float deltaY = 0;

int colorIndex = 0;

// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }

// forward declaration
void timerFunc(int);

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
		//cout << "Unhandled key press " << key << "." << endl; 
		if (colorIndex < 3)
			colorIndex++;
		else
			colorIndex = 0;
        break;
	case 'r':
    	glutTimerFunc(30, timerFunc, 0); // Callback once every 30ms and pass value 0;
    	break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}


// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
		//cout << "Unhandled key press: up arrow." << endl;
		deltaY += 0.5f;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
		//cout << "Unhandled key press: down arrow." << endl;
		deltaY -= 0.5f;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
		//cout << "Unhandled key press: left arrow." << endl;
		deltaX -= 0.5f;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
		//cout << "Unhandled key press: right arrow." << endl;
		deltaX += 0.5f;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}
int angle = 10;
void drawObjMesh()
{
	cout << "drawLoadedObj" << endl;
	unsigned a,b,c,d,e,f,g,h,i;

	glRotatef(angle, 0,1,0);
	for(unsigned int index = 0; index < vecf.size(); index++)
	{
		glBegin(GL_TRIANGLES);
		a = vecf[index][0];
		//b = vecf[index][1];
		c = vecf[index][2];

		d = vecf[index][3];
		//e = vecf[index][4];
		f = vecf[index][5];
		
		g = vecf[index][6];
		//h = vecf[index][7];
		i = vecf[index][8];

		// vertex a => normal c
		glNormal3f(vecn[c-1][0], vecn[c-1][1], vecn[c-1][2]);
		glVertex3f(vecv[a-1][0], vecv[a-1][1], vecv[a-1][2]);

		// d=>f
		glNormal3f(vecn[f-1][0], vecn[f-1][1], vecn[f-1][2]);
		glVertex3f(vecv[d-1][0], vecv[d-1][1], vecv[d-1][2]);

		// vertex g => normal i
		glNormal3f(vecn[i-1][0], vecn[i-1][1], vecn[i-1][2]);
		glVertex3f(vecv[g-1][0], vecv[g-1][1], vecv[g-1][2]);

		glEnd();
	}
}

void drawObjMeshByDisplayList()
{
	cout << "drawLoadedObj" << endl;
	unsigned a,b,c,d,e,f,g,h,i;

	// Create one display list
	GLuint index = glGenLists(1);
	glNewList(index, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for(unsigned int index = 0; index < vecf.size(); index++)
	{

		a = vecf[index][0];
		//b = vecf[index][1];
		c = vecf[index][2];

		d = vecf[index][3];
		//e = vecf[index][4];
		f = vecf[index][5];
		
		g = vecf[index][6];
		//h = vecf[index][7];
		i = vecf[index][8];

		// vertex a => normal c
		glNormal3f(vecn[c-1][0], vecn[c-1][1], vecn[c-1][2]);
		glVertex3f(vecv[a-1][0], vecv[a-1][1], vecv[a-1][2]);

		// d=>f
		glNormal3f(vecn[f-1][0], vecn[f-1][1], vecn[f-1][2]);
		glVertex3f(vecv[d-1][0], vecv[d-1][1], vecv[d-1][2]);

		// vertex g => normal i
		glNormal3f(vecn[i-1][0], vecn[i-1][1], vecn[i-1][2]);
		glVertex3f(vecv[g-1][0], vecv[g-1][1], vecv[g-1][2]);
		
	}
	glEnd();
	glEndList();

	// draw
	glCallList(index);
	glDeleteLists(index, 1);
}


void timerFunc(int timer)
{
	cout << "timerfunc" <<endl;
	//cout << angle << endl;
	angle += 1;

	drawObjMesh();
	glutPostRedisplay();

	glutTimerFunc(100, timerFunc, 0);
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };
    
	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[colorIndex]);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
	GLfloat Lt0pos[] = {1.0f + deltaX, 1.0f + deltaY, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	//glutSolidTeapot(1.0);

	//drawObjMesh();
	drawObjMeshByDisplayList();
	
    // Dump the image to the screen.
    glutSwapBuffers();

}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void split(const std::string &s, char delim, vector<unsigned>& result)
{
	std::istringstream iss(s);
	std::string token;
	while(std::getline(iss, token, delim))
	{
		result.push_back(std::stoi(token));
	}
}

void loadInput()
{
	// load the OBJ file here
	cout << "LoadInput" <<endl;
	const int MAX_BUFFER_SIZE = 256;
	char buffer[MAX_BUFFER_SIZE];
	while(cin.getline(buffer, MAX_BUFFER_SIZE))
	{
		stringstream ss(buffer);
		Vector3f v;
		string s;
		ss >> s;
		
		if(s == "v")
		{
			ss >> v[0] >> v[1] >> v[2];
			vecv.push_back(v);
		}
		if(s == "vn")
		{
			ss >> v[0] >> v[1] >> v[2];
			vecn.push_back(v);
		}
		if(s == "f")
		{
			vector<unsigned> face;
			string token1, token2, token3;
			ss >> token1 >> token2 >> token3;
			
			split(token1, '/', face);
			split(token2, '/', face);
			split(token3, '/', face);
			vecf.push_back(face);
		}
	}
}


// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
