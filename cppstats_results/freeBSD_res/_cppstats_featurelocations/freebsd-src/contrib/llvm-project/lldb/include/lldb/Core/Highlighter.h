







#if !defined(LLDB_CORE_HIGHLIGHTER_H)
#define LLDB_CORE_HIGHLIGHTER_H

#include <utility>
#include <vector>

#include "lldb/Utility/Stream.h"
#include "lldb/lldb-enumerations.h"
#include "llvm/ADT/StringRef.h"

namespace lldb_private {



struct HighlightStyle {




class ColorStyle {
std::string m_prefix;
std::string m_suffix;

public:
ColorStyle() = default;
ColorStyle(llvm::StringRef prefix, llvm::StringRef suffix) {
Set(prefix, suffix);
}






void Apply(Stream &s, llvm::StringRef value) const;


void Set(llvm::StringRef prefix, llvm::StringRef suffix);
};




ColorStyle selected;


ColorStyle identifier;

ColorStyle string_literal;

ColorStyle scalar_literal;

ColorStyle keyword;

ColorStyle comment;

ColorStyle comma;

ColorStyle colon;

ColorStyle semicolons;

ColorStyle operators;


ColorStyle braces;

ColorStyle square_brackets;

ColorStyle parentheses;




ColorStyle pp_directive;


static HighlightStyle MakeVimStyle();
};


class Highlighter {
public:
Highlighter() = default;
virtual ~Highlighter() = default;
Highlighter(const Highlighter &) = delete;
const Highlighter &operator=(const Highlighter &) = delete;


virtual llvm::StringRef GetName() const = 0;















virtual void Highlight(const HighlightStyle &options, llvm::StringRef line,
llvm::Optional<size_t> cursor_pos,
llvm::StringRef previous_lines, Stream &s) const = 0;


std::string Highlight(const HighlightStyle &options, llvm::StringRef line,
llvm::Optional<size_t> cursor_pos,
llvm::StringRef previous_lines = "") const;
};



class DefaultHighlighter : public Highlighter {
public:
llvm::StringRef GetName() const override { return "none"; }

void Highlight(const HighlightStyle &options, llvm::StringRef line,
llvm::Optional<size_t> cursor_pos,
llvm::StringRef previous_lines, Stream &s) const override;
};


class HighlighterManager {
DefaultHighlighter m_default;

public:









const Highlighter &getHighlighterFor(lldb::LanguageType language_type,
llvm::StringRef path) const;
const Highlighter &getDefaultHighlighter() const { return m_default; }
};

}

#endif
