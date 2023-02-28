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
#include <cmath>

using namespace NCL;
using namespace GameDemo;

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
	//InitWorld();
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
}

void TutorialGame::InitTest() {
	InitGameExamples();
	InitDefaultFloor();
}

void TutorialGame::InitGame() {
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
	floor->SetBoundingVolume((CollisionVolume*)volume, floorSize);
	floor->GetTransform().SetScale(floorSize*2).SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetColour(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);

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
	sphere->SetBoundingVolume((CollisionVolume*)volume, sphereSize);

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
	GameObject* cube = new GameObject("Cube");
	AABBVolume* volume = new AABBVolume(dimensions * 0.45f);
	cube->SetBoundingVolume((CollisionVolume*)volume, dimensions * 0.45f);

	cube->GetTransform().SetPosition(position).SetScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
	cube->GetPhysicsObject()->SetElasticity(0.3f);
	cube->GetPhysicsObject()->SetFriction(0.5f);
	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	float scale = 1.0f;
	GameObject* apple = new GameObject();
	SphereVolume* volume = new SphereVolume(scale);
	apple->SetBoundingVolume((CollisionVolume*)volume, Vector3(scale, scale, scale));
	apple->GetTransform().SetScale(Vector3(scale, scale, scale)).SetPosition(position);

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
	apple->SetBoundingVolume((CollisionVolume*)volume, Vector3(0.5f, 0.5f, 0.5f));
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
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(0, 5, 5));
	AddBonusToWorld(Vector3(10, 5, 0));
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

void TutorialGame::SelectMode() {
	string text = "1. Test Mode.";
	Debug::Print(text, Vector2(35, 30), Debug::GREEN);
	text = "2. Game Start.";
	Debug::Print(text, Vector2(35, 50), Debug::GREEN);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM1)) {
		gameMode = GAME_MODE_TEST;
		InitTest();
	}
	else if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM2)) {
		gameMode = GAME_MODE_START;
		InitGame();
	}
}

void TutorialGame::Update(float dt) {
	//game mode
	switch (gameMode) {
	case GAME_MODE_DEFAULT:
	{
		SelectMode();
		break;
	}
	case GAME_MODE_TEST:
	{
		UpdateTest(dt);
		break;
	}
	case GAME_MODE_START:
	{
		UpdateGame(dt);
		break;
	}
	default:
		std::cout << "mode error" << std::endl;
	}
	renderer->Update(dt);
	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateTest(float dt) {
	world->GetMainCamera()->UpdateCamera(dt);
	world->UpdateWorld(dt);
	physics->Update(dt);
}

void TutorialGame::UpdateGame(float dt) {
	//reset word
	if (gameover && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		ResetGame();
		return;
	}
	UpdateKeys();
	//game status
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

	if (!pause) {
		physics->Update(dt);
	}
}

void TutorialGame::UpdateKeys() {

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P)) {
		pause = !pause;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		gameover = !gameover;
	}
}

bool TutorialGame::UpdateCountdown(float dt) {
	if (0.0f == countdown) { return false; }
	countdown = countdown - dt >= 0.0f ? countdown - dt : 0.0f;
	return true;
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

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

void TutorialGame::InitMap() {
	float wallSize = 6.0f;
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
	Vector3 wallDimension = Vector3(wallSize, wallSize * 3.0f, wallSize);
	GridNode* allNodes = mapPathFinding->GetAllNodes();
	if (nullptr == allNodes) {
		throw 1;
		return;
	}
	int mapSize = mapPathFinding->GetMapSize();
	//only 5 path
	int pathNum = 5;
	vector<vector<Vector3>> patrolTargets(pathNum);
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
			continue;
		default:
			//char to int
			int index = node.type - '0';
			if (index < 0 || index >= pathNum) { continue; }
			patrolTargets[index].push_back(node.position);
		}
	}
	int index = 0;
	for (auto i : enemyObjects) {
		if (index >= pathNum) { break;}
		auto target = patrolTargets[index];
		if (target.empty()) { continue; }
		//change dir
		if (index != 0) { reverse(target.begin(), target.end()); }
		i->InitialiseBehaviours(target);
		index++;
	}
	return;
}

GameEnemy* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float scale = 2.0f;
	float inverseMass = 0.1f;

	GameEnemy* character = new GameEnemy("Enemy");
	Vector3 size = Vector3(0.9f, 0.9f, 0.2f) * scale;
	AABBVolume* volume = new AABBVolume(size);
	character->SetBoundingVolume((CollisionVolume*)volume, size);

	character->GetTransform().SetScale(Vector3(scale, scale, scale)).SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->SetFriction(ENEMY_FRICTION);
	character->SetColour(ENEMY_DEFAULT_COLOUR);

	world->AddGameObject(character);

	return character;
}

GamePlayer* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float scale = 1.2f;
	float inverseMass = 0.1f;

	GamePlayer* character = new GamePlayer("Player");
	SphereVolume* volume = new SphereVolume(scale);

	character->SetBoundingVolume((CollisionVolume*)volume, Vector3(scale, scale, scale));

	character->GetTransform().SetScale(Vector3(scale, scale, scale)).SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetColour(PLAYER_DEFAULT_COLOUR);
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->SetElasticity(0.0f);
	character->SetOriginPosition(position);

	world->AddGameObject(character);

	return character;
}

GameBall* TutorialGame::AddBallToWorld(const Vector3& position) {
	GameBall* ball = new GameBall("Ball");

	SphereVolume* volume = new SphereVolume(0.5f);
	ball->SetBoundingVolume((CollisionVolume*)volume, Vector3(0.5f, 0.5f, 0.5f));
	ball->GetTransform().SetScale(Vector3(0.5, 0.5, 0.5)).SetPosition(position);

	ball->SetRenderObject(new RenderObject(&ball->GetTransform(), sphereMesh, basicTex, basicShader));
	ball->SetPhysicsObject(new PhysicsObject(&ball->GetTransform(), ball->GetBoundingVolume()));

	ball->GetPhysicsObject()->SetInverseMass(1.0f);
	ball->GetPhysicsObject()->SetFriction(0.5f);
	ball->GetPhysicsObject()->InitCubeInertia();
	ball->SetColour(Vector4(1, 1, 0, 1));
	ball->SetOriginPosition(position);

	world->AddGameObject(ball);

	return ball;
}

GameObject* TutorialGame::AddGoalToWorld(const Vector3& position) {
	float scale = 4.0f;

	GameObject* goal = new GameObject();
	Vector3 size = Vector3(0.3f, 0.9f, 0.3f) * scale;
	AABBVolume* volume = new AABBVolume(size);
	goal->SetBoundingVolume((CollisionVolume*)volume, size);

	goal->GetTransform().SetScale(Vector3(scale, scale, scale)).SetPosition(position);

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

GameObject* TutorialGame::AddGoalDeocrationToWorld(const Vector3& position) {
	GameObject* doc = new GameObject("Decoration");
	float scale = 0.5f;
	Vector3 size = Vector3(scale, scale, scale);
	SphereVolume* volume = new SphereVolume(scale);
	doc->SetBoundingVolume((CollisionVolume*)volume, size);
	doc->GetTransform().SetScale(size).SetPosition(position);

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
	Vector3 offSet = Vector3(viewOffset.x * cos((yrot + 270.0f) * M_PI / 180), viewOffset.y, viewOffset.z * sin((yrot - 270.0f) * M_PI / 180));
	Vector3 camPos = objPos + offSet;
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
	if (gameover) { return; }
	if (UpdateCountdown(dt)) { return; }
	if (!playerObject->CheckInReviveTime()) {
		PlayerObjectMovement(dt);
	}
	//update player info;
	UpdatePlayerState(dt);
	//catch ball
	CheckBallState();
	//update enemy
	UpdateEnemyState(dt);
	//player live check
	CheckPlayerDead();
}

void TutorialGame::PlayerObjectMovement(float dt) {
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!
	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();
	rightAxis.Normalise();

	Transform& transform = playerObject->GetTransform();
	//orientation
	float dirVal = Window::GetMouse()->GetRelativePosition().x;
	while (180.0f <= dirVal) { dirVal -= 180.0f; }
	Quaternion orientation = transform.GetOrientation();
	orientation = orientation + (Quaternion(Vector3(0, -dirVal * playerObject->GetTurnSpeed(), 0), 0.0f) * orientation);
	transform.SetOrientation(orientation.Normalised());
	//linear
	float speed = 0.0f;
	auto speedUp = [&]() {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) { //speed up
			if (playerObject->SpeedUp(dt)) {
				speed = playerObject->GetQuickSpeed();
			}
			else {
				speed = playerObject->GetSpeed();
			}
		} else {
			speed = playerObject->GetSpeed();
			playerObject->RecoverStrength(dt);
		}
	};
	Vector3 position = transform.GetPosition();
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		speedUp();
		playerObject->GetPhysicsObject()->ApplyLinearImpulse(fwdAxis * speed);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		speedUp();
		playerObject->GetPhysicsObject()->ApplyLinearImpulse(-fwdAxis * speed);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		speedUp();
		playerObject->GetPhysicsObject()->ApplyLinearImpulse(-rightAxis * speed);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		speedUp();
		playerObject->GetPhysicsObject()->ApplyLinearImpulse(rightAxis * speed);
	}
}

void TutorialGame::UpdatePlayerState(float dt) {
	playerObject->Update(dt);
}

//check for position in enemy's view
//@param pos - player's position
//@param enemy - enemy object
bool TutorialGame::CheckPositionInEnemyView(Vector3 pos, GameEnemy* enemy) {
	if (!enemy) { return false; }
	if (!enemy->CheckPositionInView(pos)) { return false; }
	//check for objects blocking the vie
	Vector3 enemyPos = enemy->GetTransform().GetPosition();
	Vector3 vecEtoP = pos - enemyPos;
	float dist = NCL::Maths::Distance(enemyPos, pos);
	vector<GameObject*> objects;
	world->GetObjectsInTargetView(objects, enemyPos, vecEtoP, dist);
	auto ray = Ray(enemyPos, vecEtoP.Normalised());
	RayCollision collision;
	for (auto object : objects) {
		if (CollisionDetection::RayIntersection(ray, *object, collision)) {
			return false;
		}
	}
	return true;
}

void TutorialGame::UpdateEnemyState(float dt) {
	Vector3 playerPos = playerObject->GetTransform().GetPosition();
	for (auto enemy : enemyObjects) {
		if (CheckPositionInEnemyView(playerPos, enemy)) {
			if (!enemy->TrackingPlayer()) {
				enemy->SetColour(Vector4(1.0f, 0.0f, 0.0f, 1.0f)); //become red
			}
			enemy->StartTrackingPlayer(playerObject->GetTransform().GetPosition()); //update player' position
		}
		else if (enemy->TrackingPlayer()) {
			enemy->SetColour(Vector4(0.0f, 0.5f, 1.0f, 1.0f));
			enemy->LostPlayer();
		}
		//update action
		enemy->UpdateAction(dt);
	}
}


bool TutorialGame::KickBall() {
	if (!playerObject->CheckCatchBall()) {
		return false;
	}
	if (Window::GetMouse()->ButtonHeld(NCL::MouseButtons::LEFT)) {
		//keep ball;
		Vector3 playerDir = -playerObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);
		playerDir.Normalise();
		Vector3 playerPos = playerObject->GetTransform().GetPosition();
		Vector3 tarPos = playerPos + playerDir * 4.0f;
		//higher amend
		tarPos.y += 1.0f;
		ballObject->GetTransform().SetPosition(tarPos);
		/*
		ballObject->GetPhysicsObject()->ClearForces();
		ballObject->GetPhysicsObject()->SetAngularVelocity(Vector3());
		*/
		//change force;
		if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::E)) { ballObject->IncreaseForce(100.0f); }
		else if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::Q)) { ballObject->DecreaseForce(100.0f); }
		//change angle
		float changeAngle = -Window::GetMouse()->GetRelativePosition().y;
		ballObject->AddKickAngle(changeAngle * 0.01);
		if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
			ballObject->GetPhysicsObject()->SetLinearVelocity(playerObject->GetPhysicsObject()->GetLinearVelocity());
			//cut constraint
			if (playerObject->CheckCatchBall()) {
				auto cons = playerObject->LostBall();
				world->RemoveConstraint(cons, true);
			}
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
	//cut constraint
	if (playerObject->CheckCatchBall()) {
		auto cons = playerObject->LostBall();
		world->RemoveConstraint(cons, true);
	}
	goalTime = GOAL_TIP_TIME;
	float extraScore = (int)Maths::Distance(ballObject->GetKickedPosition(), goalObject->GetTransform().GetPosition());
	playerObject->AddScore(GOAL_SCORE + extraScore);
	//put back ball
	ballObject->Reset();
	return true;
}

void TutorialGame::CheckBallState() {
	//kick ball
	KickBall();
	//goal
	if (Goal()) { return; }
	//catch ball
	if (playerObject->CheckCatchBall()) { return; }
	//check collision detection
	CollisionDetection::CollisionInfo info;
	if (!CollisionDetection::ObjectIntersection(playerObject, ballObject, info)) { return; }
	PositionConstraint* contraint = new PositionConstraint(playerObject, ballObject, 4);
	world->AddConstraint(contraint);
	playerObject->CatchBall(contraint);
	std::cout << "player catch ball!!" << std::endl;
}

void TutorialGame::CheckPlayerDead() {
	if (playerObject->CheckGameOver() && !gameover) { 
		gameover = true; 
		return;
	}
	CollisionDetection::CollisionInfo collision;
	for (auto& i : enemyObjects) {
		GameEnemy* enemy = i;
		if (!enemy->TrackingPlayer()) { continue; }
		if (CollisionDetection::ObjectIntersection(enemy, playerObject, collision)) {
			if (playerObject->CheckCatchBall()) {
				//take the ball
				auto cons = playerObject->LostBall();
				world->RemoveConstraint(cons, true);
				//put ball back
				ballObject->Reset();
			}
			//player dead
			playerObject->Revive();
		}
	}
}

void TutorialGame::UpdateScreenInfo(float dt) {
	//countdown
	if (countdown != 0.0f) {
		Debug::Print(std::to_string(int(countdown)), Vector2(50, 30), Debug::WHITE);
	}
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
	//print player info
	string text;
	if (nullptr != playerObject) {
		text = "Score: " + std::to_string(playerObject->GetScore());
		Debug::Print(text, Vector2(5, 10), Debug::BLUE);
		text = "GameTime: " + std::to_string(playerObject->GetLiveTime());
		Debug::Print(text, Vector2(35, 10), Debug::BLUE);
		float reviveTime = playerObject->GetReviveTime();
		if (0 < reviveTime) {
			if (PLAYER_RIVIVE_TIME * 3.0f / 5.0f < reviveTime) {
				text = "YOU DEAD";
				Debug::Print(text, Vector2(42.5, 20), Debug::RED);
			}
			text = std::to_string((int)reviveTime);
			Debug::Print(text, Vector2(49, 30), Debug::BLACK);
		}
		//print ball info
		if (nullptr != ballObject) {
			if (Window::GetMouse()->ButtonHeld(NCL::MouseButtons::LEFT) && playerObject->CheckCatchBall()) {
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
	ballObject = nullptr;
	goalObject = nullptr;
	playerObject = nullptr;
	enemyObjects = std::vector<GameEnemy*>();

	InitWorld();
	InitGame();
}