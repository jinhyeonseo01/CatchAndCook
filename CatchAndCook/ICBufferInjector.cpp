#include "pch.h"
#include "ICBufferInjector.h"

std::unique_ptr<InjectorManager> InjectorManager::main = nullptr;

std::unordered_map<BufferType,std::function<std::shared_ptr<ICBufferInjector>()>> InjectorManager::_injectTable = {};