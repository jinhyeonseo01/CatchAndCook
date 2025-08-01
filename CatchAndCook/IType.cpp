#include "pch.h"
#include "IType.h"



IType::IType()
{
}

IType::~IType()
{
}

std::string& IType::GetTypeName()
{
    if (_className.empty())
    {
        std::string fullName = GetTypeFullName();
        _className = fullName;
        // "struct " 또는 "class " 같은 접두사 제거

        size_t pos = fullName.find_last_of(" ");
        if (pos != std::string::npos) {
            _className = fullName.substr(pos + 1);
        }
    	pos = fullName.find_last_of("::");
        if (pos != std::string::npos) {
            _className = fullName.substr(pos + 1);
        }

        // 복잡한 부분을 단순화 (예: "1Derived" -> "Derived")
        // 숫자가 포함된 경우 제거
        for (size_t i = 0; i < _className.length(); ++i)
        {
            if (isdigit(_className[i]))
                continue;
            _className = _className.substr(i);
            break;
        }
    }
    //TypeRegister(this);
    return _className;
}

std::string IType::GetTypeFullName()
{
    return std::string{ typeid(*this).name() };
}