#ifndef GIW_GIW_H_
#define GIW_GIW_H_

//Graphical Interface Wraper (GIW)

#include <memory>
#include <exception>
#include <utility>
#include <system_error>

template<typename Creator, typename Destructor, typename... Arguments>
    auto make_resource(Creator c, Destructor d, Arguments&&... args)
    {
        auto r = c(std::forward<Arguments>(args)...);
        if (!r) { throw std::system_error(errno, std::generic_category()); }
        return std::unique_ptr<std::decay_t<decltype(*r)>, decltype(d)>(r, d);
    }


#endif