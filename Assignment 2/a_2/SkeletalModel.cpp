#include "SkeletalModel.h"

#include <FL/Fl.H>
#include <iostream>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.
	cout << "Loading skeleton " << filename << endl;
	std::ifstream ifs(filename);
	if (!ifs)
    {
        cerr<< filename << " not found\a" << endl;
        exit(0);
    }
	float x, y, z;
	int index;
	int count = 0;
	while(ifs >> x >> y >> z >> index)
	{
		count ++;
		cout << x << " " << y << " " << z  << " " << index << endl;
		
		Joint *joint = new Joint();
		
		joint->transform = Matrix4f::translation(Vector3f(x, y, z));		
		m_joints.push_back(joint);
		if(index >= 0)
		{
			m_joints[index]->children.push_back(joint);
		}
	}
	ifs.close();
	m_rootJoint = m_joints.front();
}

void dfsForJoint(Joint* joint, MatrixStack stack)
{
	stack.push(joint->transform);

	for (int i = 0; i < joint->children.size(); i++) {
		dfsForJoint(joint->children[i], stack);
	}

	glLoadMatrixf(stack.top());
	glutSolidSphere(0.025f, 12, 12);
	stack.pop();
}

float getDistance(Vector3f v1, Vector3f v2){

	return sqrt(pow(v1.x() - v2.x(), 2) + pow(v1.y() - v2.y(), 2) + pow(v1.z() - v2.z(), 2));
}


void dfsForBone(Joint* joint, MatrixStack stack, Joint* rootNode){
	
	stack.push(joint->transform);

	for(int i = 0; i < joint->children.size(); i++)
	{
		// draw bone to parent
		dfsForBone(joint->children[i], stack, rootNode);
	}
	stack.pop();

	if(joint != rootNode)
	{
		Matrix4f translation = Matrix4f::translation(0,0,0.5f);
		Vector3f tansform = joint->transform.getCol(3).xyz();

		float length = tansform.abs();
		Matrix4f scale = Matrix4f::scaling(0.03f, 0.03f, length);
		
		Vector3f rnd(0,0,1);
		Vector3f z = tansform.normalized();
		Vector3f y = Vector3f::cross(z, rnd).normalized();
		Vector3f x = Vector3f::cross(y, z).normalized();

		Matrix4f rotation;
		Vector4f col1(x, 0);
		Vector4f col2(y, 0);
		Vector4f col3(z, 0);
		Vector4f col4(0, 0, 0, 1);

		rotation.setCol(0, col1);
		rotation.setCol(1, col2);
		rotation.setCol(2, col3);
		rotation.setCol(3, col4);


		stack.push(rotation);
		stack.push(scale);
		stack.push(translation);

		glLoadMatrixf(stack.top());

		glutSolidCube(1.0f);
		stack.pop();
		stack.pop();
		stack.pop();
	}
}



void SkeletalModel::drawJoints()
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.
	
	dfsForJoint(m_rootJoint, m_matrixStack);

}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
	
	dfsForBone(m_rootJoint, m_matrixStack, m_rootJoint);
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
}


void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.
}

