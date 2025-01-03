#include "pch.h"
#include "IType.h"


std::unordered_map<std::string, std::function<void* ()>> IType::_typeTable{};

IType::IType()
{
}

IType::~IType()
{
}

void IType::StaticInit()
{
    _typeTable.reserve(4096);
}

void IType::StaticRelease()
{
    _typeTable.clear();
}

std::string& IType::GetTypeName()
{
    if (_className.empty())
    {
        std::string fullName = GetTypeFullName();

        // "struct " �Ǵ� "class " ���� ���λ� ����
        size_t pos = fullName.find_last_of("::");
        if (pos != std::string::npos)
        {
            _className = fullName.substr(pos + 1);
        }

        // ������ �κ��� �ܼ�ȭ (��: "1Derived" -> "Derived")
        // ���ڰ� ���Ե� ��� ����
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