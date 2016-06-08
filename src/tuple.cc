#include "libpy/tuple.h"
#include "libpy/utils.h"

namespace t = py::tuple;

const py::type::object<t::object> t::type((PyObject*) &PyTuple_Type);

t::object::object() : py::object() {}

t::object::object(PyObject *pob) : py::object(pob) {
    tuple_check();
}

t::object::object(const py::object &pob) : py::object(pob) {
    tuple_check();
}

t::object::object(const t::object &cpfrom) : py::object((PyObject*) cpfrom) {}

t::object::object(t::object &&mvfrom) noexcept :
    py::object((PyObject*) mvfrom) {
    mvfrom.ob = nullptr;
}

t::object::object(size_t len) : py::object(PyTuple_New(len)) {}

t::object::object(py::ssize_t len) : py::object(PyTuple_New(len)) {}

void t::object::tuple_check() {
    if (ob && !PyTuple_Check(ob)) {
        ob = nullptr;
        if (!PyErr_Occurred()) {
            PyErr_SetString(PyExc_TypeError,
                            "cannot make py::tuple::object from non tuple");
        }
    }
}

t::object::const_iterator t::object::cbegin() const {
    if (!is_nonnull()) {
        return nullptr;
    }
    // it is safe to cast a PyObject* to a py::object because it has standard
    // layout and only a single field
    return (const py::object*) &((PyTupleObject*) ob)->ob_item[0];
}

t::object::const_iterator t::object::cend() const {
    if (!is_nonnull()) {
        return nullptr;
    }

    // it is safe to cast a PyObject* to a py::object because it has standard
    // layout and only a single field
    return (const py::object*) &((PyTupleObject*) ob)->ob_item[Py_SIZE(ob)];
}

t::object::iterator t::object::begin() const {;
    return cbegin();
}

t::object::iterator t::object::end() const {
    return cend();
}

py::ssize_t t::object::len() const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyTuple_GET_SIZE(ob);
}

py::nonnull<t::object> t::object::as_nonnull() const {
    if (!is_nonnull()) {
        throw pyutils::bad_nonnull();
    }
    return nonnull<t::object>(ob);

}

py::tmpref<t::object> t::object::as_tmpref() && {
    tmpref<t::object> ret(ob);
    ob = nullptr;
    return std::move(ret);
}
