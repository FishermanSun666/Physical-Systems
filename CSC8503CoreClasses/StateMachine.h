#pragma once

namespace NCL {
	namespace GameDemo {
		class State;	// Predeclare the classes we need
		class StateTransition;
		// Typedefs to make life easier !
		typedef std::multimap<State*, StateTransition*> TransitionContainer;
		typedef TransitionContainer::iterator TransitionIterator;

		class StateMachine	{
		public:
			StateMachine();
			virtual ~StateMachine(); //made it virtual!

			void AddState(State* s);
			void AddTransition(StateTransition* t);

			virtual void Update(float dt); //made it virtual!

		protected:
			State * activeState;

			std::vector<State*> allStates;
			TransitionContainer allTransitions;
		};
	}
}