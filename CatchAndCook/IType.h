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

        // 복잡한 부분을 단순화 (예: "1Derived" -> "Derived")
        // 숫자가 포함된 경우 제거
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