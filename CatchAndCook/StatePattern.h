#pragma once

class StatePattern;

enum class StateType
{
	none,
	idle_already,
	idle,
	goto_any,
	goto_shop,
	goto_table,
	eat,
};

class StatePatternGroup : public std::enable_shared_from_this<StatePatternGroup>
{
public:
	std::unordered_map<StateType, std::shared_ptr<StatePattern>> statePatterns;
	std::shared_ptr<StatePattern> currentState;

	virtual void Init();
	virtual void Update();
	virtual void AnyUpdate();
	virtual void AddState(StateType type, const std::shared_ptr<StatePattern>& state);
	virtual void ChangeState(StateType type);
};

class StatePattern
{
private:
	StateType type;
	weak_ptr<StatePatternGroup> group;

public:

	float _time;
	float GetTime(){return _time;}
	StateType GetType() { return type; }
	StateType SetType(StateType type) { this->type = type; return type; }
	shared_ptr<StatePatternGroup> GetGroup() const { return group.lock(); };
    shared_ptr<StatePatternGroup> SetGroup(const shared_ptr<StatePatternGroup>& group) { 
       this->group = group; 
       return group; 
    };


	virtual void Init();
	virtual void Begin(StateType type, const std::shared_ptr<StatePattern>& prevState);
	virtual void Update();
	virtual bool TriggerUpdate();
	virtual void End(const std::shared_ptr<StatePattern>& nextState);
};

