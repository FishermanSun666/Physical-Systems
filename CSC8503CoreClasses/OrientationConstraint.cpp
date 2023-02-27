#include "OrientationConstraint.h"
#include "GameObject.h"
#include "PhysicsObject.h"
using namespace PhysicalProject;
using namespace Maths;
using namespace GameDemo;

OrientationConstraint::OrientationConstraint(GameObject* a, GameObject* b)
{
	objectA = a;
	objectB = b;
}

OrientationConstraint::~OrientationConstraint()
{

}

void OrientationConstraint::UpdateConstraint(float dt) {

}