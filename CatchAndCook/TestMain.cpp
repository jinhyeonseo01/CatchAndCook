#include "pch.h"
#include "IType.h"

class Derived : public IType
{
public:
    Derived() { TypeRegister(this); } 
};
 

int main()
{
    IType::StaticInit(); 
    IType* obj = IType::CreateObject<Derived>("Derived");

    if (obj)
        std::cout << "Created object of type: " << obj->GetTypeName() << std::endl;

    IType::StaticRelease(); // Ÿ�� �ý��� ����
    return 0;
}