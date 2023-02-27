#pragma once

namespace PhysicalProject {
	namespace GameDemo {
		class PushdownState;

		class PushdownMachine
		{
		public:
			PushdownMachine(PushdownState* initialState);
			~PushdownMachine();

			bool Update(float dt);

			void AddReserveState(PushdownState* state);

		protected:
			PushdownState* activeState;
			PushdownState* initialState;

			std::stack<PushdownState*> stateStack;
			std::stack<PushdownState*> reserveStack;
		};
	}
}

