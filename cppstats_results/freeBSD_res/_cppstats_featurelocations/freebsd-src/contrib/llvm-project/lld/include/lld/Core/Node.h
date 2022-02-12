













#if !defined(LLD_CORE_NODE_H)
#define LLD_CORE_NODE_H

#include "lld/Core/File.h"
#include <algorithm>
#include <memory>

namespace lld {




class Node {
public:
enum class Kind { File, GroupEnd };

explicit Node(Kind type) : _kind(type) {}
virtual ~Node() = default;

virtual Kind kind() const { return _kind; }

private:
Kind _kind;
};



class GroupEnd : public Node {
public:
explicit GroupEnd(int size) : Node(Kind::GroupEnd), _size(size) {}

int getSize() const { return _size; }

static bool classof(const Node *a) {
return a->kind() == Kind::GroupEnd;
}

private:
int _size;
};


class FileNode : public Node {
public:
explicit FileNode(std::unique_ptr<File> f)
: Node(Node::Kind::File), _file(std::move(f)) {}

static bool classof(const Node *a) {
return a->kind() == Node::Kind::File;
}

File *getFile() { return _file.get(); }

protected:
std::unique_ptr<File> _file;
};

}

#endif
