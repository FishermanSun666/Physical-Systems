#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"

#include <stack>
#include <corecrt_math_defines.h>

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= true;
	inSelectionMode = false;

	InitialiseAssets();
}

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;

	//game
	delete mapPathFinding;
	delete ballObject;
	delete goalObject;
	delete playerObject;
	for (auto i : enemyObjects) {
		delete i;
	}
	enemyObjects = std::vector<GameEnemy*>();
}
/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	charMesh	= renderer->LoadMesh("goose.msh");
	enemyMesh	= renderer->LoadMesh("Goat.msh");
	bonusMesh	= renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");
	goalMesh = renderer->LoadMesh("Keeper.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitWorld();
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);
	//stateObject
	//testStateObject = AddStateObjectToWorld(Vector3(0, 15, 0));
	//constraint
	//BridgeConstraintTest();

	//InitGameExamples();

	//game
	try {
		InitMap();
		InitGameObjects();
		InitDefaultFloor();
	}
	catch (int i) {
		switch (i) {
		case 1:
			std::cout << "Load Map error" << std::endl;
			return;
		default:
			std::cout << "Error" << std::endl;
			return;
		}
	}
}

/*

A single function to add a large immovable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), sphereMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position) {
	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), sphereMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, 0, 0));
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 0, 0));
	AddEnemyToWorld(Vector3(0, 0, 5));
	AddBonusToWorld(Vector3(10, 0, 0));
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::UpdateGame(float dt) {
	//reset word
	if (gameover && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		ResetGame();
		return;
	}
	UpdateKeys();
	//game model
	if (playerObject != nullptr && !pause && !gameover) {
		CameraLockOnPlayer();
		UpdateGameObject(dt);
	}
	else {
		playerObject->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
		ballObject->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
		if (!inSelectionMode) {
			world->GetMainCamera()->UpdateCamera(dt);
		}
		DebugObjectMovement();
		RayCast();
	}
	//update screen info
	UpdateScreenInfo(dt);

	world->UpdateWorld(dt);
	renderer->Update(dt);
	if (!pause) {
		physics->Update(dt);
	}

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateKeys() {
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
	//	InitWorld(); //We can reset the simulation at any time with F1
	//	selectionObject = nullptr;
	//}

	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
	//	InitCamera(); //F2 will reset the camera to a specific default place
	//}

	////default use of gravity.
	////if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
	////	useGravity = !useGravity; //Toggle gravity!
	////	physics->UseGravity(useGravity);
	////}

	////Running certain physics updates in a consistent order might cause some
	////bias in the calculations - the same objects might keep 'winning' the constraint
	////allowing the other one to stretch too much etc. Shuffling the order so that it
	////is random every frame can help reduce such bias.
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
	//	world->ShuffleConstraints(true);
	//}
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
	//	world->ShuffleConstraints(false);
	//}

	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
	//	world->ShuffleObjects(true);
	//}
	//if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
	//	world->ShuffleObjects(false);
	//}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
		pause = !pause;
	}
	//TODO test
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		gameover = !gameover;
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		playerObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		playerObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		playerObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
	}
}

void TutorialGame::DebugObjectMovement() {
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}
/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;

				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void TutorialGame::RayCast() {
	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));
}

void TutorialGame::MoveSelectedObject() {
	Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);

	float invCubeMass = 5;	//how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 30;	//constraint distance
	float cubeDistance = 20;	//distance between links

	Vector3 startPos = Vector3(0, 100, 0);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::InitMap() {
	float wallSize = 5.0f;
	//pathFinding
	mapPathFinding = new NavigationGrid(wallSize, "GameGrid.txt");
	if (nullptr == mapPathFinding) {
		std::cout << "Load map error" << std::endl;
		throw 1;
		return;
	}
	return;
}

void TutorialGame::InitGameObjects() {
	if (nullptr == mapPathFinding) {
		throw 1;
		return;
	}
	float wallSize = mapPathFinding->GetNodeSize();
	//build map
	Vector3 wallDimension = Vector3(wallSize, wallSize, wallSize) * 0.5f;
	GridNode* allNodes = mapPathFinding->GetAllNodes();
	if (nullptr == allNodes) {
		throw 1;
		return;
	}
	int mapSize = mapPathFinding->GetMapSize();
	//only 3 path
	int pathNum = 2;
	vector<vector<Vector3>> patrolTargets(pathNum);
	Vector3 docPos = Vector3(0, 0, 0);
	for (int i = 0; i < mapSize; i++) {
		GridNode node = allNodes[i];
		switch (node.type) {
		case ('.'):
		{
			continue;
		}
		case ('x'):
		{
			//add wall;
			AddCubeToWorld(node.position, wallDimension, 0.0f);
			continue;
		}
		case ('e'):
		{
			//enemy;
			GameEnemy* enemyObject = AddEnemyToWorld(node.position);
			enemyObject->SetPathFindingMap(mapPathFinding);
			enemyObjects.push_back(enemyObject);
			continue;
		}
		case ('o'):
		{
			//ball;
			ballObject = AddBallToWorld(node.position);
			continue;
		}
		case ('p'):
		{
			//player;
			playerObject = AddPlayerToWorld(node.position);
			continue;
		}
		case ('g'):
			//goal
			goalObject = AddGoalToWorld(node.position);
		case ('d'):
			docPos = node.position;
			
		default:
			//char to int
			int index = node.type - '0';
			if (index < 0 || index >= pathNum) {
				continue;
			}
			patrolTargets[index].push_back(node.position);
		}
	}
	if (docPos != Vector3(0, 0, 0)) {
		AddGoalDocrationToWorld(docPos);
	}
	int index = 0;
	for (auto i : enemyObjects) {
		if (index >= pathNum) {
			break;
		}
		auto target = patrolTargets[index];
		if (target.empty()) {
			continue;
		}
		//change
		if (index != 0) {
			reverse(target.begin(), target.end());
		}
		i->InitialiseBehaviours(target);
		index++;
	}
	return;
}

GameEnemy* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize = 2.0f;
	float inverseMass = 0.1f;

	GameEnemy* character = new GameEnemy("Enemy");

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.8f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GamePlayer* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	GamePlayer* character = new GamePlayer("Player");
	SphereVolume* volume = new SphereVolume(meshSize);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();
	character->SetOriginPosition(position);

	world->AddGameObject(character);

	return character;
}

GameBall* TutorialGame::AddBallToWorld(const Vector3& position) {
	GameBall* ball = new GameBall("Ball");

	SphereVolume* volume = new SphereVolume(0.5f);
	ball->SetBoundingVolume((CollisionVolume*)volume);
	ball->GetTransform()
		.SetScale(Vector3(0.5, 0.5, 0.5))
		.SetPosition(position);

	ball->SetRenderObject(new RenderObject(&ball->GetTransform(), sphereMesh, nullptr, basicShader));
	ball->SetPhysicsObject(new PhysicsObject(&ball->GetTransform(), ball->GetBoundingVolume()));

	ball->GetPhysicsObject()->SetInverseMass(1.0f);
	ball->GetPhysicsObject()->InitSphereInertia();
	ball->SetColour(Vector4(1, 1, 0, 1));
	ball->SetOriginPosition(position);

	world->AddGameObject(ball);

	return ball;
}

GameObject* TutorialGame::AddGoalToWorld(const Vector3& position) {
	float meshSize = 4.0f;

	GameObject* goal = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	goal->SetBoundingVolume((CollisionVolume*)volume);

	goal->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	goal->SetRenderObject(new RenderObject(&goal->GetTransform(), goalMesh, nullptr, basicShader));
	goal->SetPhysicsObject(new PhysicsObject(&goal->GetTransform(), goal->GetBoundingVolume()));

	//cannot move
	goal->GetPhysicsObject()->SetInverseMass(0.0f);
	goal->GetPhysicsObject()->InitSphereInertia();

	//set colour
	goal->SetColour(Vector4(0, 1, 1, 1));

	world->AddGameObject(goal);

	return goal;
}

GameObject* TutorialGame::AddGoalDocrationToWorld(const Vector3& position) {
	GameObject* doc = new GameObject("Docration");

	SphereVolume* volume = new SphereVolume(0.5f);
	doc->SetBoundingVolume((CollisionVolume*)volume);
	doc->GetTransform()
		.SetScale(Vector3(0.5, 0.5, 0.5))
		.SetPosition(position);

	doc->SetRenderObject(new RenderObject(&doc->GetTransform(), enemyMesh, nullptr, basicShader));
	doc->SetPhysicsObject(new PhysicsObject(&doc->GetTransform(), doc->GetBoundingVolume()));

	doc->GetPhysicsObject()->SetInverseMass(0.2f);
	doc->GetPhysicsObject()->InitSphereInertia();
	doc->SetColour(Vector4(0, 1, 1, 1));
	//add
	if (goalObject != nullptr) {
		PositionConstraint* contraint = new PositionConstraint(goalObject, doc, 5);
		world->AddConstraint(contraint);
	}

	return doc;
}

void TutorialGame::CameraLockOnPlayer() {
	Vector3 objPos = playerObject->GetTransform().GetPosition();
	//find object orientation
	float yrot = playerObject->GetTransform().GetOrientation().ToEuler().y;
	//set camera position
	Vector3 viewOffset = Vector3(playerViewOffset.x * cos((yrot + 270.0f) * M_PI / 180), playerViewOffset.y, playerViewOffset.z * sin((yrot - 270.0f) * M_PI / 180));
	Vector3 camPos = objPos + viewOffset;

	//lock on object
	Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));
	Matrix4 modelMat = temp.Inverse();
	Quaternion q(modelMat);
	Vector3 angles = q.ToEuler(); //nearly there now!

	world->GetMainCamera()->SetPosition(camPos);
	world->GetMainCamera()->SetPitch(angles.x);
	world->GetMainCamera()->SetYaw(angles.y);
}

void TutorialGame::UpdateGameObject(float dt) {
	if (!playerObject->InReviveTime()) {
		PlayerObjectMovement();
	}
	//goal
	Goal();

	//update player info;
	UpdatePlayerInfo(dt);
	//kick ball
	KickBall(dt);
	//catch ball
	CheckBallStatus();
	//update enemy
	UpdateEnemeys(dt);
	//player live check
	KillPlayer();
}

void TutorialGame::PlayerObjectMovement() {
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();
	rightAxis.Normalise();

	Transform& transform = playerObject->GetTransform();

	//orientation
	float dirVal = Window::GetMouse()->GetRelativePosition().x;
	while (180 <= dirVal) {
		dirVal -= 180;
	}
	Quaternion orientation = transform.GetOrientation();
	float turnSpeed = playerObject->GetTurnSpeed();
	orientation = orientation + (Quaternion(Vector3(0, -dirVal * turnSpeed, 0), 0.0f) * orientation);
	orientation.Normalise();
	transform.SetOrientation(orientation);
	//linear
	float speed = 0.0f;
	auto speedUp = [&]() {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
			speed = playerObject->GetQuickSpeed();
		}
		else {
			speed = playerObject->GetSpeed();
		}
	};
	Vector3 position = transform.GetPosition();
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		speedUp();
		playerObject->GetPhysicsObject()->SetLinearVelocity(fwdAxis * speed);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		speedUp();
		playerObject->GetPhysicsObject()->SetLinearVelocity(-fwdAxis * speed);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		speedUp();
		playerObject->GetPhysicsObject()->SetLinearVelocity(-rightAxis * speed);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		speedUp();
		playerObject->GetPhysicsObject()->SetLinearVelocity(rightAxis * speed);
	}
	//clear player velocity
	if (0.0f == speed) {
		playerObject->GetPhysicsObject()->SetLinearVelocity(Vector3(0, playerObject->GetPhysicsObject()->GetLinearVelocity().y, 0));
	}
}

void TutorialGame::UpdatePlayerInfo(float dt) {
	playerObject->Update(dt);
}

void TutorialGame::UpdateEnemeys(float dt) {
	for (auto i = enemyObjects.begin(); i != enemyObjects.end(); i++) {
		GameEnemy* enemy = (*i);
		//check whether player in enemy view.
		Vector3 enemyDir = enemy->GetTransform().GetOrientation() * Vector3(0, 0, -1);
		Ray sightLine = Ray(enemy->GetTransform().GetPosition(), enemyDir);
		//object volume multi 3
		auto playerVolume = (SphereVolume*)playerObject->GetBoundingVolume();
		GameObject temp;
		auto tempVolume = new SphereVolume(playerVolume->GetRadius() * 4);
		temp.SetBoundingVolume((CollisionVolume*)tempVolume);
		temp.SetTrasform(playerObject->GetTransform());
		RayCollision collision;
		if (CollisionDetection::RayIntersection(sightLine, temp, collision)) {
			//in view track
			enemy->SetColour(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
			enemy->StartTrackingPlayer(playerObject->GetTransform().GetPosition());
		}
		else {
			//out of view, give up
			enemy->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			if (enemy->TrackingPlayer()) {
				enemy->LostPlayer();
			}
		}
		//update behaviour
		enemy->UpdateBehaviour(dt);
	}
}

bool TutorialGame::KickBall(float dt) {
	if (!playerObject->KeepBall()) {
		return false;
	}
	if (Window::GetMouse()->ButtonHeld(NCL::MouseButtons::LEFT)) {
		//keep ball;
		Vector3 playerDir = playerObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);
		playerDir.Normalise();
		Vector3 playerPos = playerObject->GetTransform().GetPosition();
		Vector3 tarPos = playerPos + playerDir * 4.0f;
		//higher amend
		tarPos.y += 1.0f;
		ballObject->GetTransform().SetPosition(tarPos);
		//add force;
		ballObject->AddForce(dt*10.0f);
		//std::cout << "kick force: " << ballObject->GetForce() << std::endl;
		//change angle
		float changeAngle = -Window::GetMouse()->GetRelativePosition().y;
		ballObject->AddKickAngle(changeAngle * 0.01);
		std::cout << "kick angle: " << ballObject->GetAngleAmend() << std::endl;
		if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
			//cut constraint
			if (playerObject->KeepBall()) {
				auto cons = playerObject->LostBall();
				world->RemoveConstraint(cons, true);
			}
			//kick ball
			ballObject->Kick(playerDir);
		}
	}
	return true;
}

bool TutorialGame::Goal() {
	//try goal
	CollisionDetection::CollisionInfo info;
	if (!CollisionDetection::ObjectIntersection(goalObject, ballObject, info)) {
		return false;
	}

	//put back ball
	ballObject->Reset();
	//cut constraint
	if (playerObject->KeepBall()) {
		auto cons = playerObject->LostBall();
		world->RemoveConstraint(cons, true);
	}
	
	//goal
	goalTime = g_goal_tip_time;
	playerObject->AddScore(g_goal_score);
	return true;
}

bool TutorialGame::CheckBallStatus() {
	if (playerObject->KeepBall()) {
		return true;
	}
	//check collision detection
	CollisionDetection::CollisionInfo info;
	if (!CollisionDetection::ObjectIntersection(playerObject, ballObject, info)) {
		return false;
	}

	PositionConstraint* contraint = new PositionConstraint(playerObject, ballObject, 2);
	world->AddConstraint(contraint);
	playerObject->CatchBall(contraint);
	std::cout << "catch ball!!" << std::endl;
}

void TutorialGame::KillPlayer() {
	CollisionDetection::CollisionInfo collision;
	for (auto& i : enemyObjects) {
		GameEnemy* enemy = i;
		if (CollisionDetection::ObjectIntersection(enemy, playerObject, collision)) {
			if (playerObject->KeepBall()) {
				//take the ball
				auto cons = playerObject->LostBall();
				world->RemoveConstraint(cons, true);
				//put ball back
				ballObject->Reset();
			}
			//player dead
			playerObject->Revive(g_player_revive_time);
		}
	}
}

void TutorialGame::UpdateScreenInfo(float dt) {
	//pause
	if (pause) {
		Debug::Print("(P) pause on", Vector2(1, 95), Debug::GREEN);
	}
	else {
		Debug::Print("(P) pause off", Vector2(1, 95), Debug::GREEN);
	}
	//game over
	if (gameover) {
		Debug::Print("Game over, reset(F1)", Vector2(30, 50), Debug::RED);
		//win game
		if (40 <= playerObject->GetScore()) {
			Debug::Print("You Win!!", Vector2(30, 55), Debug::RED);
		}
		else {
			Debug::Print("You Lost!!", Vector2(30, 55), Debug::RED);
		}
	}
	//tips
	if (0 < goalTime) {
		Debug::Print("Goal !!!!!", Vector2(40, 40), Debug::RED);
		goalTime -= dt;
	}
	//Operation
	//kick ball
	std::string text = "Click the left!";
	Debug::Print(text, Vector2(5, 15), Debug::GREEN);
	text = "Adjust angle and strength of the kick!";
	Debug::Print(text, Vector2(5, 20), Debug::GREEN);
	text = "kick the ball!";
	Debug::Print(text, Vector2(5, 25), Debug::GREEN);
	text = "Let's all hand the ball to the blue target!!";
	Debug::Print(text, Vector2(5, 30), Debug::GREEN);

	//print player info
	if (nullptr != playerObject) {
		text = "Score: " + std::to_string(playerObject->GetScore());
		Debug::Print(text, Vector2(5, 10), Debug::BLUE);
		text = "GameTime: " + std::to_string(playerObject->GetLiveTime());
		Debug::Print(text, Vector2(35, 10), Debug::BLUE);
		float reviveTime = playerObject->GetReviveTime();
		if (0 < reviveTime) {
			if (3 < reviveTime) {
				text = "YOU DEAD";
				Debug::Print(text, Vector2(42.5, 20), Debug::RED);
			}
			text = std::to_string((int)reviveTime);
			Debug::Print(text, Vector2(49, 30), Debug::BLACK);
		}
		//print ball info
		if (nullptr != ballObject) {
			if (Window::GetMouse()->ButtonHeld(NCL::MouseButtons::LEFT) && playerObject->KeepBall()) {
				Vector3 line = playerObject->GetTransform().GetOrientation() * Vector3(0.0f, 0.0f, -1.0f);
				line.y += ballObject->GetAngleAmend();

				//write orientaion line 
				Vector3 offset = Vector3::Cross(line, Vector3(0, 1, 0));
				offset.Normalise();
				offset *= 1.5f;
				Vector3 pos = ballObject->GetTransform().GetPosition();
				pos.y -= 1.0f;
				Debug::DrawLine(pos+ offset, pos + line + offset, Vector4(1, 0, 0, 1));
				//show force
				text = "Force: " + std::to_string((int)ballObject->GetForce());
				Debug::Print(text, Vector2(65, 70), Debug::YELLOW);
			}
		}
	}
	
}

void TutorialGame::ResetGame() {
	pause = false;
	gameover = false;
	delete mapPathFinding;
	delete ballObject;
	delete goalObject;
	delete playerObject;
	for (auto i : enemyObjects) {
		delete i;
	}
	enemyObjects = std::vector<GameEnemy*>();
	InitWorld();
}