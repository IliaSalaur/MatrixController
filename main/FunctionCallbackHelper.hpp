#ifndef MY_FUNCTION_CALLBACK_HANDLER_HPP
#define MY_FUNCTION_CALLBACK_HANDLER_HPP

#include <cstdio>
#include <functional>

// Base Callback struct
template <typename T>
struct Callback;

// Callback struct wrapper for std::function
template <typename Ret, typename... Params>
struct Callback<Ret(Params...)>
{
    // Our std::function, that keeps the callback
    static std::function<Ret(Params...)> func;

    // Wrapper function for the func, same signature. Basically, just calls the func and returns it's result
    template<typename... Args>
    static Ret callback(Args... args)
    {
        return func(args...);
    }    
};

// Init the func
template <typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<Ret(Params...)>::func;

#endif