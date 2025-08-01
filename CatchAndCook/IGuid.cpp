#include "pch.h"
#include "IGuid.h"
#include "guid_utility.h"

std::unordered_map<std::wstring, std::weak_ptr<IGuid>> IGuid::_GuidTable{};

uint64_t IGuid::idAllocator = 0;

void IGuid::StaticInit()
{
    _GuidTable.reserve(10000);
}

void IGuid::StaticRelease()
{
    _GuidTable.clear();
}

uint64_t IGuid::AllocID()
{
    return idAllocator++;
}

IGuid::IGuid()
{
    this->SetGUID(Guid::GetNewGuid());
    this->id = AllocID();
}

IGuid::~IGuid()
{
	RemoveGuid(this->guid);
}

IGuid::IGuid(const std::wstring& guid)
{
    this->SetGUID(guid);
}

IGuid::IGuid(const IGuid& eObject)
{
    this->SetGUID(eObject.guid);
}

IGuid::IGuid(IGuid&& eObject) noexcept
{
    this->SetGUID(std::move(eObject.guid));
}

IGuid& IGuid::operator=(const IGuid& eObject)
{
    if (this == &eObject)
        return *this;

    this->SetGUID(eObject.guid);
    return *this;
}

IGuid& IGuid::operator=(IGuid&& eObject) noexcept
{
    if (this == &eObject)
        return *this;

    this->SetGUID(std::move(eObject.guid));
    return *this;
}


void IGuid::SetGUID(const std::wstring& str)
{
    auto& prevGuid = this->guid;
    bool isFindPrevGuid = this->guid.empty() ? false : ContainsByGuid(prevGuid);
    if (isFindPrevGuid)
    {
        RemoveGuid(prevGuid);
        AddObject(this->shared_from_this());
    }

    this->guid = str;
}

std::wstring& IGuid::GetGUID()
{
    return this->guid;
}

int IGuid::GetInstanceID() const
{
    return this->id;
}

bool IGuid::operator==(const IGuid& other) const
{
    return other.guid == this->guid;
}

bool IGuid::operator<(const IGuid& other) const
{
    return other.guid < this->guid;
}
