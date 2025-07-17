#pragma once
#include "IType.h"

class IGuid : public std::enable_shared_from_this<IGuid>, public IType
{
public:
    static std::unordered_map<std::wstring, std::weak_ptr<IGuid>> _GuidTable;
	static void StaticInit();
    static void StaticRelease();
    static uint64_t AllocID();
    static uint64_t idAllocator;

protected:
    std::wstring guid;
    uint64_t id = 0;

public:
    IGuid();
    ~IGuid() override;
    IGuid(const std::wstring& guid);
    IGuid(const IGuid& eObject);
    IGuid(IGuid&& eObject) noexcept;
    IGuid& operator=(const IGuid& eObject);
    IGuid& operator=(IGuid&& eObject) noexcept;
    bool operator==(const IGuid& other) const;
    bool operator<(const IGuid& other) const;

    void SetGUID(const std::wstring& str);
    std::wstring& GetGUID();
    int GetInstanceID() const;

    //template <class T, class = std::enable_if_t<std::is_base_of_v<IGuid, T>>>
    void InitGuid()
    {
        AddObject(this->shared_from_this());
    }

    template <class T, class = std::enable_if_t<std::is_base_of_v<IGuid, T>>>
    std::shared_ptr<T> GetCast()
    {
        return std::static_pointer_cast<T>(this->shared_from_this());
    }


public:
    template <class T, class = std::enable_if_t<std::is_convertible_v<T*, IGuid*>>>
    static bool AddObject(std::shared_ptr<T> object)
    {
        if (object == nullptr)
            return false;

        if (!_GuidTable.contains(object->guid))
        {
            _GuidTable.insert(std::make_pair(object->guid,
                std::weak_ptr<IGuid>
                {
                    std::dynamic_pointer_cast<IGuid>(object)
                }));
            return true;
        }
        return false;
    }

    template <class T, class = std::enable_if_t<std::is_convertible_v<T*, IGuid*>>>
    static bool RemoveObject(std::shared_ptr<T> object)
    {
        if (object == nullptr)
            return false;
        if (_GuidTable.contains(object->guid))
        {
            _GuidTable.erase(object->guid);
            return true;
        }
        return false;
    }

    static bool RemoveGuid(const std::wstring& guid)
    {
        auto iter = _GuidTable.find(guid);
        if (iter != _GuidTable.end()) {
            _GuidTable.erase(iter);
            return true;
        }

        return false;
    }

    static bool ContainsByGuid(const std::wstring& guid)
    {
        if (_GuidTable.contains(guid))
        {
            if (_GuidTable[guid].expired())
            {
                _GuidTable.erase(guid);
                return false;
            }

            return true;
        }

        return false;
    }

    template <class T, class = std::enable_if_t<std::is_convertible_v<T*, IGuid*>>>
    static std::shared_ptr<T> FindObjectByType()
    {
        for (auto it = _GuidTable.begin(); it != _GuidTable.end();)
        {
            auto& current = *it;
            if (!current.second.expired())
            {
                auto ptr = std::dynamic_pointer_cast<T>(current.second.lock());
                if (ptr != nullptr)
                    return ptr;
                ++it;
            }
            else
                it = _GuidTable.erase(it);
        }
        return nullptr;
    }

    template <class T, class = std::enable_if_t<std::is_convertible_v<T*, IGuid*>>>
    static bool FindObjectsByType(std::vector<std::shared_ptr<T>>& vec)
    {
        bool tf = false;
        for (auto it = _GuidTable.begin(); it != _GuidTable.end();)
        {
            auto& current = *it;
            if (!current.second.expired())
            {
                auto ptr = std::dynamic_pointer_cast<T>(current.second.lock());
                if (ptr != nullptr)
                {
                    vec.push_back(ptr);
                    tf = true;
                }
                ++it;
            }
            else
                it = _GuidTable.erase(it);
        }
        return tf;
    }

    template <class T, class = std::enable_if_t<std::is_convertible_v<T*, IGuid*>>>
    static std::shared_ptr<T> FindObjectByGuid(const std::wstring& guid)
    {
        if (_GuidTable.contains(guid))
        {
            auto& current = _GuidTable[guid];
            if (!current.expired()) {
                auto ptr = std::dynamic_pointer_cast<T>(current.lock());
                return ptr;
            }
            _GuidTable.erase(guid);
        }
        return nullptr;
    }

};
