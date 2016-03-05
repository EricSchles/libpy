#include <gtest/gtest.h>

#include "libpy/object.h"

using namespace py;

TEST(UserDefinedLiterals, char) {
    object c = 'c'_p;
    EXPECT_EQ((PyObject*) c.type(), (PyObject*) &PyUnicode_Type);
    EXPECT_STREQ(PyUnicode_AsUTF8((PyObject*) c), "c");
    EXPECT_TRUE((c == 'c'_p).istrue());
    EXPECT_TRUE((c == "c"_p).istrue());
}

TEST(UserDefinedLiterals, string) {
    object cs = "test"_p;
    EXPECT_EQ((PyObject*) cs.type(), (PyObject*) &PyUnicode_Type);
    EXPECT_STREQ(PyUnicode_AsUTF8((PyObject*) cs), "test");
    EXPECT_TRUE((cs == "test"_p).istrue());
}

TEST(UserDefinedLiterals, wchar_t) {
    object c = L'c'_p;
    EXPECT_EQ((PyObject*) c.type(), (PyObject*) &PyUnicode_Type);
    EXPECT_STREQ(PyUnicode_AsUTF8((PyObject*) c), "c");
    EXPECT_TRUE((c == 'c'_p).istrue());
    EXPECT_TRUE((c == "c"_p).istrue());
}

TEST(UserDefinedLiterals, wstring) {
    object cs = L"test"_p;
    EXPECT_EQ((PyObject*) cs.type(), (PyObject*) &PyUnicode_Type);
    EXPECT_STREQ(PyUnicode_AsUTF8((PyObject*) cs), "test");
    EXPECT_TRUE((cs == "test"_p).istrue());
}

TEST(UserDefinedLiterals, ull) {
    object n = 10_p;
    EXPECT_EQ((PyObject*) n.type(), (PyObject*) &PyLong_Type);
    EXPECT_EQ(PyLong_AS_LONG((PyObject*) n), 10);
    EXPECT_TRUE((n == 10_p).istrue());
}

TEST(UserDefinedLiterals, longdouble) {
    object n = 2.5_p;
    EXPECT_EQ((PyObject*) n.type(), (PyObject*) &PyFloat_Type);
    EXPECT_EQ(PyFloat_AS_DOUBLE((PyObject*) n), 2.5);
    EXPECT_TRUE((n == 2.5_p).istrue());
}
