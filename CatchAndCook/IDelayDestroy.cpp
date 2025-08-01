#include "pch.h"
#include "IDelayDestroy.h"

bool IDelayDestroy::IsDestroy() const
{
    return _destroy;
}

void IDelayDestroy::SetDestroy()
{
    _destroy = true;
}

bool IDelayDestroy::IsExecuteAble()
{
    return (!IsDestroy()) && (!IsFirst());
}

IDelayDestroy::~IDelayDestroy() = default;


IDelayDestroy::IDelayDestroy() : _destroy(false)
{
}