#pragma once

class IType
{

public:
    IType();
    virtual ~IType();

    std::string& GetTypeName();
    std::string GetTypeFullName();

	template<class T>
    static std::string GetTypeFullName()
	{
        return std::string{ typeid(T).name() };
	};

    template<class T>
    static std::string GetTypeName()
    {
        std::string fullName = IType::GetTypeFullName<T>();

        size_t pos = fullName.find_last_of("::");
        if (pos != std::string::npos)
            fullName = fullName.substr(pos + 1);
        pos = fullName.find_last_of(" ");
        if (pos != std::string::npos)
            fullName = fullName.substr(pos + 1);

        // ������ �κ��� �ܼ�ȭ (��: "1Derived" -> "Derived")
        // ���ڰ� ���Ե� ��� ����
        for (size_t i = 0; i < fullName.length(); ++i)
        {
            if (isdigit(fullName[i]))
                continue;
            fullName = fullName.substr(i);
            break;
        }
        //TypeRegister(this);
        return fullName;
    };

private:
    std::string _className;
};