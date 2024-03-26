#pragma once

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <vector>
#include <functional>
#include "IDisconnectObservers.hh"
#include "Observer.hh"



template <typename RT>
class SignalToOne;

template <typename RT, typename Arg>
class SignalToOne<RT(Arg)> : public IDisconnectObservers
{
private:
    // using Delegate  = Function<RT(Arg)>;
    using Delegate  = std::function<RT(Arg)>;
    std::map<Arg, Delegate> delegates;

protected:
    std::vector<Arg> getKeys() const
    {
        std::vector<Arg> keys;
        keys.reserve(delegates.size());
        std::transform(delegates.begin(), delegates.end(), std::back_inserter(keys),
                       [&](const auto& delegate) { return delegate.first; });
        return keys;
    }

public:
    SignalToOne<RT(Arg)>()
    {
    }

    virtual ~SignalToOne<RT(Arg)>()
    {
        while (!delegates.empty())
        {
            disconnect(*(delegates.rbegin()->second.instance()));
        }
    }

    SignalToOne<RT(Arg)>(const SignalToOne<RT(Arg)>&) = delete;
    SignalToOne<RT(Arg)>(SignalToOne<RT(Arg)>&&)      = delete;
    SignalToOne<RT(Arg)>& operator=(const SignalToOne<RT(Arg)>&) = delete;
    SignalToOne<RT(Arg)>& operator=(SignalToOne<RT(Arg)>&&) = delete;

    // void disconnect(Observer& obs) final override
    // {
    //     auto address           = std::addressof(obs);
    //     auto addressComparison = [address](auto& iter) {
    //         return address == static_cast<Observer*>(iter.second.instance());
    //     };

    //     auto delegate = std::find_if(delegates.begin(), delegates.end(), addressComparison);
    //     if (delegates.end() != delegate)
    //     {
    //         auto dp = delegate->second.instance();
    //         erase_remove_if(delegates, addressComparison);
    //         dp->detach(*this);
    //     }
    // }

    void disconnect(Observer& obs) final override
    {
        auto address = std::addressof(obs);
        auto predicate = [address](const auto& entry) {
            return address == static_cast<Observer*>(entry.second.instance());
        };

        // 查找并移除与obs相同地址的观察者
        auto it = std::find_if(delegates.begin(), delegates.end(), predicate);
        if (it != delegates.end())
        {
            ObserverDelegate* dp = it->second.instance();
            dp->detach(*this);

            // 使用 remove_if 和 erase 移除符合条件的元素
            auto new_end = std::remove_if(delegates.begin(), delegates.end(), predicate);
            delegates.erase(new_end, delegates.end());
        }
    }

    void disconnect(Arg filter)
    {
        auto delegate = delegates.find(filter);
        if (delegates.end() != delegate)
        {
            const auto address           = delegate->second.instance();
            auto       addressComparison = [&address](auto& iter) { return address == iter.second.instance(); };
            const auto count             = std::count_if(delegates.begin(), delegates.end(), addressComparison);
            auto       dp                = delegate->second.instance();
            delegates.erase(delegate);
            if (1 == count)
                dp->detach(*this);
        }
    }

    template <typename T, RT (T::*mem_ptr)(Arg)>
    void connect(Arg filter, Observer& ptr)
    {
        auto filterCheck = delegates.find(filter);
        if (delegates.end() == filterCheck)
        {
            delegates.emplace(std::move(filter), Delegate::template bind<T, mem_ptr>(std::addressof(ptr)));
            ptr.attach(*this);
        }
        else
        {
            std::string msg;
            msg.append(" Arg: ")
                .append(typeid(Arg).name())
                .append(" T: ")
                .append(typeid(T).name())
                .append(" RT: ")
                .append(typeid(RT).name())
                .append(" mem_ptr: ")
                .append(typeid(mem_ptr).name()
            std::cerr << msg << std::endl;
        }
    }

    template <typename ArgRef>
    void emit(ArgRef&& arg)
    {
        auto delegate = delegates.find(arg);
        if (delegates.end() != delegate)
            delegate->second(std::forward<ArgRef>(arg));
    }
};
