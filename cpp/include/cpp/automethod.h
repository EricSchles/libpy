#pragma once
#include <array>
#include <tuple>

#include <Python.h>

#include "cpp/object.h"
#include "cpp/utils.h"

namespace pyutils {

    template<typename T>
    constexpr char _typeformat;

    template<>
    constexpr char _typeformat<const char*> = 'z';

    template<>
    constexpr char _typeformat<Py_buffer> = 's';

    template<>
    constexpr char _typeformat<char> = 'c';

    template<>
    constexpr char _typeformat<unsigned char> = 'b';

    template<>
    constexpr char _typeformat<unsigned short> = 'h';

    template<>
    constexpr char _typeformat<int> = 'i';

    template<>
    constexpr char _typeformat<long> = 'l';

    template<>
    constexpr char _typeformat<unsigned long> = 'k';

    template<>
    constexpr char _typeformat<float> = 'f';

    template<>
    constexpr char _typeformat<double> = 'd';

    template<>
    constexpr char _typeformat<Py_complex> = 'D';

    template<>
    constexpr char _typeformat<PyObject*> = 'O';

    template<>
    constexpr char _typeformat<bool> = 'p';

    template<typename F>
    struct _function_traits;

    template<typename R, typename Self, typename... Args>
    struct _function_traits<R(Self, Args...)> {
        using return_type = R;
        using parsed_args_type = std::tuple<Args...>;

        static constexpr std::size_t arity = sizeof...(Args);

        static inline auto fmtstr() {
            return std::array<char, arity + 1> {_typeformat<Args>..., '\0'};
        }
    };

    template<typename F, const F &impl>
    PyObject *_automethodwrapper(PyObject *self, PyObject *args) {
        using f = _function_traits<F>;
        typename f::parsed_args_type parsed_args;

        if (!apply(PyArg_ParseTuple,
                   std::tuple_cat(std::make_tuple(args, f::fmtstr().data()),
                                  tuple_refs(parsed_args)))) {
            return NULL;
        }
        return apply(impl, std::tuple_cat(std::make_tuple(self), parsed_args));
    }

#define _cpp_automethod_2(f, doc) (PyMethodDef {                        \
        #f,                                                             \
        (PyCFunction) pyutils::_automethodwrapper<decltype(f), f>,      \
        METH_VARARGS,                                                   \
        doc,                                                            \
    })
#define _cpp_automethod_1(f) _cpp_automethod_2(f, NULL)
#define _cpp_automethod_dispatch(n, f, doc, macro, ...)  macro

    /**
       Wrap a C++ function as a python PyMethodDef structure.

       @param func The function to wrap.
       @param doc  The docstring to use for the function. If this is omitted
                   the docstring will be None.
       @return     A `PyMethodDef` structure for the given function.
    */
#define automethod(...)                                                 \
    _cpp_automethod_dispatch(                                           \
        ,##__VA_ARGS__,                                                 \
        _cpp_automethod_2(__VA_ARGS__),                                 \
        _cpp_automethod_1(__VA_ARGS__),                                 \
    )
}