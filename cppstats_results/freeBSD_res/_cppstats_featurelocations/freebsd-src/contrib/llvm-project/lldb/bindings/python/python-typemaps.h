#if !defined(LLDB_BINDINGS_PYTHON_PYTHON_TYPEMAPS_H)
#define LLDB_BINDINGS_PYTHON_PYTHON_TYPEMAPS_H



struct Py_buffer_RAII {
Py_buffer buffer = {};
Py_buffer_RAII(){};
Py_buffer &operator=(const Py_buffer_RAII &) = delete;
Py_buffer_RAII(const Py_buffer_RAII &) = delete;
~Py_buffer_RAII() {
if (buffer.obj)
PyBuffer_Release(&buffer);
}
};

#endif
