#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"
#include "StateGameObject.h"
#include "GamePlayer.h"
#include "GameEnemy.h"
#include "GameBall.h"
#include "NavigationGrid.h"
#include "../NCLCoreClasses/Maths.h"

namespace NCL {
	namespace GameDemo {
		const int GOAL_SCORE = 10;
		const float GOAL_TIP_TIME = 3.0f;
		const int GAME_MODE_DEFAULT = 0;
		const int GAME_MODE_TEST = 1;
		const int GAME_MODE_START = 2;
		const float COUNT_DOWN_TIME = 3.0f;

		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void Update(float dt);
		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();
			void UpdateGameKeys();
			bool UpdateCountdown(float dt);
			void MainMenu();
			void SelectGameMode();
			void InitWorld();
			void InitTest();
			void InitGame();

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitGameExamples();
			void InitGameObjects();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();

			void UpdateTest(float dt);
			void UpdateGame(float dt);
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void RayCast();
			void CameraLockOnPlayer();
			void LockedObjectMovement();

			void BridgeConstraintTest();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GameObject* AddBonusToWorld(const Vector3& position);
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inversMass = 1.0f);

			//test state machine
			StateGameObject* AddStateObjectToWorld(const Vector3& position);
			StateGameObject* testStateObject;

			//game
			void ResetGame();

			void UpdateGameObject(float dt);
			void UpdateScreenInfo(float dt);

			GameObject* AddWallToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			GamePlayer* AddPlayerToWorld(const Vector3& position);
			GameEnemy* AddEnemyToWorld(const Vector3& position);
			GameBall* AddBallToWorld(const Vector3& position);
			GameObject* AddGoalTargetToWorld(const Vector3& position);
			GameObject* AddGoalDeocrationToWorld(const Vector3& position);

			void PlayerObjectMovement(float dt);
			void InitMap();
			void CheckBallState();
			void UpdatePlayerState(float dt);
			bool CheckPositionInEnemyView(Vector3 pos, GameEnemy* enemy);
			void UpdateEnemyState(float dt);
			bool CheckPlayerInEnemyView(GameEnemy* enemy);
			bool KickBall();
			bool Goal();
			void CheckPlayerDead();


#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			int gameMode = GAME_MODE_DEFAULT;

			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			MeshGeometry*	capsuleMesh = nullptr;
			MeshGeometry*	cubeMesh	= nullptr;
			MeshGeometry*	sphereMesh	= nullptr;

			TextureBase*	basicTex	= nullptr;
			ShaderBase*		basicShader = nullptr;

			//Coursework Meshes
			MeshGeometry*	charMesh	= nullptr;
			MeshGeometry*	enemyMesh	= nullptr;
			MeshGeometry*	bonusMesh	= nullptr;
			MeshGeometry* goalMesh = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;
			bool pause = false;
			bool gameover = false;
			float goalTime = 0;
			float countdown = COUNT_DOWN_TIME;
			Vector3 viewOffset = Vector3(5, 1, 5);

			NavigationGrid* mapPathFinding = nullptr;
			GameBall* ballObject = nullptr;
			GameObject* goalObject = nullptr;
			GamePlayer* playerObject = nullptr;
			std::vector<GameEnemy*> enemyObjects ;
		};
	}
}

