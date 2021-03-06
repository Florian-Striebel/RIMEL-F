







#if !defined(LLDB_SYMBOL_DECLARATION_H)
#define LLDB_SYMBOL_DECLARATION_H

#include "lldb/Utility/FileSpec.h"
#include "lldb/lldb-private.h"

namespace lldb_private {








class Declaration {
public:

Declaration() : m_file() {}














Declaration(const FileSpec &file_spec, uint32_t line = 0,
uint16_t column = LLDB_INVALID_COLUMN_NUMBER)
: m_file(file_spec), m_line(line), m_column(column) {}


Declaration(const Declaration *decl_ptr)
: m_file(), m_line(0), m_column(LLDB_INVALID_COLUMN_NUMBER) {
if (decl_ptr)
*this = *decl_ptr;
}





void Clear() {
m_file.Clear();
m_line = 0;
m_column = 0;
}

















static int Compare(const Declaration &lhs, const Declaration &rhs);










bool FileAndLineEqual(const Declaration &declaration) const;








void Dump(Stream *s, bool show_fullpaths) const;

bool DumpStopContext(Stream *s, bool show_fullpaths) const;





FileSpec &GetFile() { return m_file; }





const FileSpec &GetFile() const { return m_file; }






uint32_t GetLine() const { return m_line; }






uint16_t GetColumn() const { return m_column; }















explicit operator bool() const { return IsValid(); }

bool IsValid() const {
return m_file && m_line != 0 && m_line != LLDB_INVALID_LINE_NUMBER;
}









size_t MemorySize() const;





void SetFile(const FileSpec &file_spec) { m_file = file_spec; }






void SetLine(uint32_t line) { m_line = line; }






void SetColumn(uint16_t column) { m_column = column; }

protected:


FileSpec m_file;


uint32_t m_line = 0;


uint16_t m_column = LLDB_INVALID_COLUMN_NUMBER;
};

bool operator==(const Declaration &lhs, const Declaration &rhs);

}

#endif
