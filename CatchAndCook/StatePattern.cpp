#include "pch.h"
#include "StatePattern.h"


void StatePatternGroup::Init()
{
}




void StatePatternGroup::Update()
{
	if (currentState)
		currentState->Update();
}

void StatePatternGroup::AnyUpdate()
{
	for (auto& state : statePatterns)
	{
		if (state.second && state.second->GetType() != currentState->GetType() && state.second->TriggerUpdate())
		{
			ChangeState(state.first);
		}
	}
}

void StatePatternGroup::AddState(StateType type, const std::shared_ptr<StatePattern>& state)
{
	state->Init();
	state->SetType(type);
	state->SetGroup(shared_from_this());

	statePatterns.emplace(type, state);
}

void StatePatternGroup::ChangeState(StateType type)
{
	if (currentState)
	{
		auto nextState = statePatterns[type];
		currentState->End(nextState);
		if (nextState)
		{
			auto prev = currentState;
			currentState = nextState;
			nextState->Begin(type, prev);
		}
	}
	else
	{
		auto nextState = statePatterns[type];
		if (nextState)
		{
			auto prev = currentState;
			currentState = nextState;
			nextState->Begin(type, prev);
		}
	}
}

bool StatePattern::TriggerUpdate()
{
	return false;
}


void StatePattern::Init()
{
}

void StatePattern::Begin(StateType type, const std::shared_ptr<StatePattern>& prevState)
{
	_time = 0;
}

void StatePattern::Update()
{
	_time += Time::main->GetDeltaTime();
	//group.lock()->ChangeState(StateType::idle);
}

void StatePattern::End(const std::shared_ptr<StatePattern>& nextState)
{

}