#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"
#include "NetworkedGame.h"

#include "PushdownMachine.h"
#include "PushdownState.h"
#include "TestPushdown.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "ParallelBehaviour.h"

using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>

class TestPacketReceiver : public PacketReceiver {
public:
	TestPacketReceiver(string name) {
		this->name = name;
	}

	void ReceivePacket(int type, GamePacket* payload, int source) {
		if (type == String_Message) {
			StringPacket* realPacket = (StringPacket*)payload;

			string msg = realPacket->GetStringFromData();
			std::cout << name << "Received message: " << msg << std::endl;
		}
	}
protected:
	string name;
};

void TestNetworking() {
	NetworkBase::Initialise();

	TestPacketReceiver serverReceiver("Server");
	TestPacketReceiver clientReceiver("Client");
	
	int port = NetworkBase::GetDefaultPort();

	GameServer* server = new GameServer(port, 1);
	GameClient* client = new GameClient();

	server->RegisterPacketHandler(String_Message, &serverReceiver);
	client->RegisterPacketHandler(String_Message, &clientReceiver);

	bool canConnect = client->Connect(127, 0, 0, 1, port);

	for (int i = 0; i < 100; ++i) {
		StringPacket serverMsg = StringPacket("Server says hello!" + std::to_string(i));
		server->SendGlobalPacket(serverMsg);
		StringPacket clientMsg = StringPacket("Client says hello!" + std::to_string(i));
		client->SendPacket(clientMsg);

		server->UpdateServer();
		client->UpdateClient();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	NetworkBase::Destroy();
}

int main() {
	//test networking
	//TestNetworking();
	//return 0;

	Window*w = Window::CreateGameWindow("Game technology!", 1280, 720);

	if (!w->HasInitialised()) {
		return -1;
	}	

	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	TutorialGame* g = new TutorialGame();
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a large first dt!
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		g->UpdateGame(dt);
	}
	Window::DestroyGameWindow();
}
