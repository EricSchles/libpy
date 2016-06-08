#include <sstream>
#include <type_traits>
#include <unordered_map>

#include <gtest/gtest.h>
#include <Python.h>

#include "libpy/libpy.h"

using py::operator""_p;

TEST(Layout, py_object) {
    EXPECT_TRUE(std::is_standard_layout<py::object>::value);
    EXPECT_EQ(sizeof(py::object), sizeof(PyObject*));
}

class Object : public testing::Test {
protected:
    py::object C;
    py::object container;

    Object() : C(nullptr) {}

    virtual void SetUp() {
        PyObject *ns = PyEval_GetBuiltins();
        // create a type so that it has a mutable dict
        C = PyRun_String("type('C', (), {})",
                         Py_eval_input,
                         ns,
                         ns);
        container = PyRun_String("[1, 2, 3]",
                                 Py_eval_input,
                                 ns,
                                 ns);
    }

    virtual void TearDown() {
        C.decref();
        container.decref();
    }
};

TEST_F(Object, ostream_nonnull) {
    std::stringstream stream;
    std::unordered_map<std::string, py::object> subtests = {{"c", 'c'_p},
                                                            {"1", 1_p},
                                                            {"1.5", 1.5_p},
                                                            {"test", "test"_p}};

    for (const auto &kv : subtests) {
        for (const auto &as_nonnull : {false, true}) {
            py::object ob = std::get<1>(kv);
            if (!as_nonnull) {
                stream << ob;
            } else {
                stream << ob.as_nonnull();
            }
            EXPECT_EQ(stream.str(), std::get<0>(kv)) <<
                "as_nonnull = " << as_nonnull << '\n';
            stream.str("");  // clear the stream's contents
        }
    }
}

TEST_F(Object, ostream_nullptr) {
    std::stringstream stream;
    stream << py::object(nullptr);
    EXPECT_EQ(stream.str(), "<NULL>");
}

TEST_F(Object, hasattr) {
    py::object ob = "test"_p;
    py::object attrs = ob.dir().iter();
    py::tmpref<py::object> attr;

    while ((attr = attrs.next())) {
        EXPECT_TRUE(ob.hasattr(attr));
    }
    ASSERT_FALSE(PyErr_Occurred());

    for (const auto &attr : {"invalid1"_p, "invalid2"_p}) {
        EXPECT_FALSE(ob.hasattr(attr));
    }
}

TEST_F(Object, getattr) {
    py::object ob = "test"_p;
    py::object attrs = ob.dir().iter();
    py::tmpref<py::object> attr;

    while ((attr = attrs.next())) {
        EXPECT_NE((PyObject*) ob.getattr(attr), nullptr);
    }
    ASSERT_FALSE(PyErr_Occurred());

    for (const auto &attr : {"invalid1"_p, "invalid2"_p}) {
        EXPECT_EQ((PyObject*) ob.getattr(attr), nullptr);
        PyErr_Clear();
    }
}

TEST_F(Object, setattr_delattr) {
    ASSERT_FALSE(C.hasattr("test"_p));
    ASSERT_EQ(C.setattr("test"_p, 1_p), 0);
    EXPECT_TRUE(C.hasattr("test"_p));
    EXPECT_EQ((PyObject*) C.getattr("test"_p), (PyObject*) 1_p);
    ASSERT_EQ(C.delattr("test"_p), 0);
    EXPECT_FALSE(C.hasattr("test"_p));
}

TEST_F(Object, setitem_syntax) {
    std::array<py::object, 3> indices = {0_p, 1_p, 2_p};
    for (const auto &idx : indices) {
        container[idx] = -idx;
        EXPECT_TRUE((container[idx] == -idx).istrue());
    }
}
