#include <unordered_map>
#include <utility>

#include "libpy/object.h"

using namespace py;

object None = Py_None;
object NotImplemented = Py_NotImplemented;
object Ellipsis = Py_Ellipsis;

object::object() : ob(nullptr) {}

object::object(PyObject *pob) : ob(pob) {}

object::object(const object &cpfrom) : ob(cpfrom.ob) {}

object::object(object &&mvfrom) noexcept : ob(mvfrom.ob) {
    mvfrom.ob = nullptr;
}

object py::operator""_p(char c) {
    static std::unordered_map<char, object> cache;
    object ob = cache[c];
    if (!ob.is_nonnull()) {
        ob.ob = PyUnicode_FromStringAndSize(&c, 1);
    }
    return ob;
}

object py::operator""_p(const char *cs, std::size_t len) {
    static std::unordered_map<const char*, object> cache;
    object ob = cache[cs];
    if (!ob.is_nonnull()) {
        ob.ob = PyUnicode_FromStringAndSize(cs, len);
    }
    return ob;
}

object py::operator""_p(wchar_t c) {
    static std::unordered_map<wchar_t, object> cache;
    object ob = cache[c];
    if (!ob.is_nonnull()) {
        ob.ob = PyUnicode_FromWideChar(&c, 1);
    }
    return ob;
}

object py::operator""_p(const wchar_t *cs, std::size_t len) {
    static std::unordered_map<const wchar_t*, object> cache;
    object ob = cache[cs];
    if (!ob.is_nonnull()) {
        ob.ob = PyUnicode_FromWideChar(cs, len);
    }
    return ob;
}

object py::operator""_p(unsigned long long l) {
    static std::unordered_map<unsigned long long, object> cache;
    object ob = cache[l];
    if (!ob.is_nonnull()) {
        ob.ob = PyLong_FromUnsignedLongLong(l);
    }
    return ob;
}

object py::operator""_p(long double d) {
    static std::unordered_map<long double, object> cache;
    object ob = cache[d];
    if (!ob.is_nonnull()) {
        ob.ob = PyFloat_FromDouble(d);
    }
    return ob;
}

object &object::operator=(const object &cpfrom) {
    object tmp(cpfrom);
    return (*this = std::move(tmp));
}

object &object::operator=(object &&mvfrom) noexcept {
    ob = mvfrom.ob;
    mvfrom.ob = nullptr;
    return *this;
}

int object::print(FILE *f, int flags) const {
    return PyObject_Print(ob, f, flags);
}

object object::repr() const {
    return PyObject_Repr(ob);
}

object object::ascii() const {
    return PyObject_ASCII(ob);
}

object object::str() const {
    return PyObject_Str(ob);
}

object object::bytes() const {
    return PyObject_Bytes(ob);
}

bool object::iscallable() const {
    return PyCallable_Check(ob);
}

hash_t object::hash() const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyObject_Hash(ob);
}

int object::istrue() const {
    return int_unary_func<PyObject_IsTrue>();
}

object object::type() const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return nullptr;
    }
    return (PyObject*) Py_TYPE(ob);
}

ssize_t object::len() const {
    // PyObject_Length does its own null checks
    return PyObject_Length(ob);
}

ssize_t object::lenhint(ssize_t fallback) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyObject_LengthHint(ob, fallback);
}

object object::dir() const {
    // PyObject_Dir(NULL) has a very different meaning than expected here
    // so we will raise in that case
    return ob_unary_func<PyObject_Dir>();
}

object object::iter() const {
    return ob_unary_func<PyObject_GetIter>();
}

object object::richcompare(const object &other, compareop opid) const {
    // PyObject_RichCompare does its own null checks
    return PyObject_RichCompare(ob, other.ob, opid);
}

object object::operator<(const object &other) const {
    return t_richcompare<LT>(other);
}

object object::operator<=(const object &other) const {
    return t_richcompare<LE>(other);
}

object object::operator==(const object &other) const {
    return t_richcompare<EQ>(other);
}

object object::operator!=(const object &other) const {
    return t_richcompare<NE>(other);
}

object object::operator>(const object &other) const {
    return t_richcompare<GT>(other);
}

object object::operator>=(const object &other) const {
    return t_richcompare<GE>(other);
}

object object::operator-() const {
    return ob_unary_func<PyNumber_Negative>();
}

object object::operator+() const {
    return ob_unary_func<PyNumber_Positive>();
}

object object::abs() const {
    return ob_unary_func<PyNumber_Absolute>();
}

object object::invert() const {
    return ob_unary_func<PyNumber_Invert>();
}

const object &object::incref() const {
    if (is_nonnull()) {
        Py_INCREF(ob);
    }
    return *this;
}

const object &object::decref() {
    if (is_nonnull()) {
        // reimplement the Py_DECREF macro here so that we can set ob = nullptr
        // when we dealloc without checking the refcount twice
        if (_Py_DEC_REFTOTAL  _Py_REF_DEBUG_COMMA --(ob)->ob_refcnt != 0) {
            _Py_CHECK_REFCNT(_py_decref_tmp)
        }
        else {
            _Py_Dealloc(ob);
            ob = nullptr;
        }
    }
    return *this;
}

Py_ssize_t object::refcnt() const {
    if (!is_nonnull()) {
        return -1;
    }
    return Py_REFCNT(ob);
}

nonnull<object> object::as_nonnull() const {
    if (!is_nonnull()) {
        throw pyutils::bad_nonnull();
    }
    return nonnull<object>(ob);
}


tmpref<object> object::as_tmpref() && {
    tmpref<object> ret(ob);
    ob = nullptr;
    return ret;
}