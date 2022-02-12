















































#if !defined(LLVM_CLANG_ASTMATCHERS_ASTMATCHERSMACROS_H)
#define LLVM_CLANG_ASTMATCHERS_ASTMATCHERSMACROS_H




#define AST_MATCHER_FUNCTION(ReturnType, DefineMatcher) inline ReturnType DefineMatcher##_getInstance(); inline ReturnType DefineMatcher() { return ::clang::ast_matchers::internal::MemoizedMatcher< ReturnType, DefineMatcher##_getInstance>::getInstance(); } inline ReturnType DefineMatcher##_getInstance()


















#define AST_MATCHER_FUNCTION_P(ReturnType, DefineMatcher, ParamType, Param) AST_MATCHER_FUNCTION_P_OVERLOAD(ReturnType, DefineMatcher, ParamType, Param, 0)


#define AST_MATCHER_FUNCTION_P_OVERLOAD(ReturnType, DefineMatcher, ParamType, Param, OverloadId) inline ReturnType DefineMatcher(ParamType const &Param); typedef ReturnType (&DefineMatcher##_Type##OverloadId)(ParamType const &); inline ReturnType DefineMatcher(ParamType const &Param)
















#define AST_MATCHER(Type, DefineMatcher) namespace internal { class matcher_##DefineMatcher##Matcher : public ::clang::ast_matchers::internal::MatcherInterface<Type> { public: explicit matcher_##DefineMatcher##Matcher() = default; bool matches(const Type &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const override; }; } inline ::clang::ast_matchers::internal::Matcher<Type> DefineMatcher() { return ::clang::ast_matchers::internal::makeMatcher( new internal::matcher_##DefineMatcher##Matcher()); } inline bool internal::matcher_##DefineMatcher##Matcher::matches( const Type &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const

































#define AST_MATCHER_P(Type, DefineMatcher, ParamType, Param) AST_MATCHER_P_OVERLOAD(Type, DefineMatcher, ParamType, Param, 0)


#define AST_MATCHER_P_OVERLOAD(Type, DefineMatcher, ParamType, Param, OverloadId) namespace internal { class matcher_##DefineMatcher##OverloadId##Matcher : public ::clang::ast_matchers::internal::MatcherInterface<Type> { public: explicit matcher_##DefineMatcher##OverloadId##Matcher( ParamType const &A##Param) : Param(A##Param) {} bool matches(const Type &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const override; private: ParamType Param; }; } inline ::clang::ast_matchers::internal::Matcher<Type> DefineMatcher( ParamType const &Param) { return ::clang::ast_matchers::internal::makeMatcher( new internal::matcher_##DefineMatcher##OverloadId##Matcher(Param)); } typedef ::clang::ast_matchers::internal::Matcher<Type> ( &DefineMatcher##_Type##OverloadId)(ParamType const &Param); inline bool internal::matcher_##DefineMatcher##OverloadId##Matcher::matches( const Type &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const











































#define AST_MATCHER_P2(Type, DefineMatcher, ParamType1, Param1, ParamType2, Param2) AST_MATCHER_P2_OVERLOAD(Type, DefineMatcher, ParamType1, Param1, ParamType2, Param2, 0)




#define AST_MATCHER_P2_OVERLOAD(Type, DefineMatcher, ParamType1, Param1, ParamType2, Param2, OverloadId) namespace internal { class matcher_##DefineMatcher##OverloadId##Matcher : public ::clang::ast_matchers::internal::MatcherInterface<Type> { public: matcher_##DefineMatcher##OverloadId##Matcher(ParamType1 const &A##Param1, ParamType2 const &A##Param2) : Param1(A##Param1), Param2(A##Param2) {} bool matches(const Type &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const override; private: ParamType1 Param1; ParamType2 Param2; }; } inline ::clang::ast_matchers::internal::Matcher<Type> DefineMatcher( ParamType1 const &Param1, ParamType2 const &Param2) { return ::clang::ast_matchers::internal::makeMatcher( new internal::matcher_##DefineMatcher##OverloadId##Matcher(Param1, Param2)); } typedef ::clang::ast_matchers::internal::Matcher<Type> ( &DefineMatcher##_Type##OverloadId)(ParamType1 const &Param1, ParamType2 const &Param2); inline bool internal::matcher_##DefineMatcher##OverloadId##Matcher::matches( const Type &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const








































#define AST_POLYMORPHIC_SUPPORTED_TYPES(...) void(::clang::ast_matchers::internal::TypeList<__VA_ARGS__>)








#define AST_POLYMORPHIC_MATCHER(DefineMatcher, ReturnTypesF) namespace internal { template <typename NodeType> class matcher_##DefineMatcher##Matcher : public ::clang::ast_matchers::internal::MatcherInterface<NodeType> { public: bool matches(const NodeType &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const override; }; } inline ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##Matcher, ReturnTypesF> DefineMatcher() { return ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##Matcher, ReturnTypesF>(); } template <typename NodeType> bool internal::matcher_##DefineMatcher##Matcher<NodeType>::matches( const NodeType &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const
































#define AST_POLYMORPHIC_MATCHER_P(DefineMatcher, ReturnTypesF, ParamType, Param) AST_POLYMORPHIC_MATCHER_P_OVERLOAD(DefineMatcher, ReturnTypesF, ParamType, Param, 0)




#define AST_POLYMORPHIC_MATCHER_P_OVERLOAD(DefineMatcher, ReturnTypesF, ParamType, Param, OverloadId) namespace internal { template <typename NodeType, typename ParamT> class matcher_##DefineMatcher##OverloadId##Matcher : public ::clang::ast_matchers::internal::MatcherInterface<NodeType> { public: explicit matcher_##DefineMatcher##OverloadId##Matcher( ParamType const &A##Param) : Param(A##Param) {} bool matches(const NodeType &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const override; private: ParamType Param; }; } inline ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, ParamType> DefineMatcher(ParamType const &Param) { return ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, ParamType>(Param); } typedef ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, ParamType> (&DefineMatcher##_Type##OverloadId)(ParamType const &Param); template <typename NodeType, typename ParamT> bool internal:: matcher_##DefineMatcher##OverloadId##Matcher<NodeType, ParamT>::matches( const NodeType &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const













































#define AST_POLYMORPHIC_MATCHER_P2(DefineMatcher, ReturnTypesF, ParamType1, Param1, ParamType2, Param2) AST_POLYMORPHIC_MATCHER_P2_OVERLOAD(DefineMatcher, ReturnTypesF, ParamType1, Param1, ParamType2, Param2, 0)




#define AST_POLYMORPHIC_MATCHER_P2_OVERLOAD(DefineMatcher, ReturnTypesF, ParamType1, Param1, ParamType2, Param2, OverloadId) namespace internal { template <typename NodeType, typename ParamT1, typename ParamT2> class matcher_##DefineMatcher##OverloadId##Matcher : public ::clang::ast_matchers::internal::MatcherInterface<NodeType> { public: matcher_##DefineMatcher##OverloadId##Matcher(ParamType1 const &A##Param1, ParamType2 const &A##Param2) : Param1(A##Param1), Param2(A##Param2) {} bool matches(const NodeType &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const override; private: ParamType1 Param1; ParamType2 Param2; }; } inline ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, ParamType1, ParamType2> DefineMatcher(ParamType1 const &Param1, ParamType2 const &Param2) { return ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, ParamType1, ParamType2>(Param1, Param2); } typedef ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, ParamType1, ParamType2> (&DefineMatcher##_Type##OverloadId)( ParamType1 const &Param1, ParamType2 const &Param2); template <typename NodeType, typename ParamT1, typename ParamT2> bool internal::matcher_##DefineMatcher##OverloadId##Matcher< NodeType, ParamT1, ParamT2>:: matches(const NodeType &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const











































#define AST_TYPE_TRAVERSE_MATCHER_DECL(MatcherName, FunctionName, ReturnTypesF) namespace internal { template <typename T> struct TypeMatcher##MatcherName##Getter { static QualType (T::*value())() const { return &T::FunctionName; } }; } extern const ::clang::ast_matchers::internal:: TypeTraversePolymorphicMatcher< QualType, ::clang::ast_matchers::internal::TypeMatcher##MatcherName##Getter, ::clang::ast_matchers::internal::TypeTraverseMatcher, ReturnTypesF>::Func MatcherName













#define AST_TYPE_TRAVERSE_MATCHER_DEF(MatcherName, ReturnTypesF) const ::clang::ast_matchers::internal::TypeTraversePolymorphicMatcher< QualType, ::clang::ast_matchers::internal::TypeMatcher##MatcherName##Getter, ::clang::ast_matchers::internal::TypeTraverseMatcher, ReturnTypesF>::Func MatcherName













#define AST_TYPE_TRAVERSE_MATCHER(MatcherName, FunctionName, ReturnTypesF) namespace internal { template <typename T> struct TypeMatcher##MatcherName##Getter { static QualType (T::*value())() const { return &T::FunctionName; } }; } const ::clang::ast_matchers::internal::TypeTraversePolymorphicMatcher< QualType, ::clang::ast_matchers::internal::TypeMatcher##MatcherName##Getter, ::clang::ast_matchers::internal::TypeTraverseMatcher, ReturnTypesF>::Func MatcherName











#define AST_TYPELOC_TRAVERSE_MATCHER_DECL(MatcherName, FunctionName, ReturnTypesF) namespace internal { template <typename T> struct TypeLocMatcher##MatcherName##Getter { static TypeLoc (T::*value())() const { return &T::FunctionName##Loc; } }; } extern const ::clang::ast_matchers::internal:: TypeTraversePolymorphicMatcher< TypeLoc, ::clang::ast_matchers::internal:: TypeLocMatcher##MatcherName##Getter, ::clang::ast_matchers::internal::TypeLocTraverseMatcher, ReturnTypesF>::Func MatcherName##Loc; AST_TYPE_TRAVERSE_MATCHER_DECL(MatcherName, FunctionName##Type, ReturnTypesF)















#define AST_TYPELOC_TRAVERSE_MATCHER_DEF(MatcherName, ReturnTypesF) const ::clang::ast_matchers::internal::TypeTraversePolymorphicMatcher< TypeLoc, ::clang::ast_matchers::internal::TypeLocMatcher##MatcherName##Getter, ::clang::ast_matchers::internal::TypeLocTraverseMatcher, ReturnTypesF>::Func MatcherName##Loc; AST_TYPE_TRAVERSE_MATCHER_DEF(MatcherName, ReturnTypesF)









#define AST_TYPELOC_TRAVERSE_MATCHER(MatcherName, FunctionName, ReturnTypesF) namespace internal { template <typename T> struct TypeLocMatcher##MatcherName##Getter { static TypeLoc (T::*value())() const { return &T::FunctionName##Loc; } }; } const ::clang::ast_matchers::internal::TypeTraversePolymorphicMatcher< TypeLoc, ::clang::ast_matchers::internal::TypeLocMatcher##MatcherName##Getter, ::clang::ast_matchers::internal::TypeLocTraverseMatcher, ReturnTypesF>::Func MatcherName##Loc; AST_TYPE_TRAVERSE_MATCHER(MatcherName, FunctionName##Type, ReturnTypesF)

























#define AST_MATCHER_REGEX(Type, DefineMatcher, Param) AST_MATCHER_REGEX_OVERLOAD(Type, DefineMatcher, Param, 0)


#define AST_MATCHER_REGEX_OVERLOAD(Type, DefineMatcher, Param, OverloadId) namespace internal { class matcher_##DefineMatcher##OverloadId##Matcher : public ::clang::ast_matchers::internal::MatcherInterface<Type> { public: explicit matcher_##DefineMatcher##OverloadId##Matcher( std::shared_ptr<llvm::Regex> RE) : Param(std::move(RE)) {} bool matches(const Type &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const override; private: std::shared_ptr<llvm::Regex> Param; }; } inline ::clang::ast_matchers::internal::Matcher<Type> DefineMatcher( llvm::StringRef Param, llvm::Regex::RegexFlags RegexFlags) { return ::clang::ast_matchers::internal::makeMatcher( new internal::matcher_##DefineMatcher##OverloadId##Matcher( ::clang::ast_matchers::internal::createAndVerifyRegex( Param, RegexFlags, #DefineMatcher))); } inline ::clang::ast_matchers::internal::Matcher<Type> DefineMatcher( llvm::StringRef Param) { return DefineMatcher(Param, llvm::Regex::NoFlags); } typedef ::clang::ast_matchers::internal::Matcher<Type> ( &DefineMatcher##_Type##OverloadId##Flags)(llvm::StringRef, llvm::Regex::RegexFlags); typedef ::clang::ast_matchers::internal::Matcher<Type> ( &DefineMatcher##_Type##OverloadId)(llvm::StringRef); inline bool internal::matcher_##DefineMatcher##OverloadId##Matcher::matches( const Type &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const














































#define AST_POLYMORPHIC_MATCHER_REGEX(DefineMatcher, ReturnTypesF, Param) AST_POLYMORPHIC_MATCHER_REGEX_OVERLOAD(DefineMatcher, ReturnTypesF, Param, 0)


#define AST_POLYMORPHIC_MATCHER_REGEX_OVERLOAD(DefineMatcher, ReturnTypesF, Param, OverloadId) namespace internal { template <typename NodeType, typename ParamT> class matcher_##DefineMatcher##OverloadId##Matcher : public ::clang::ast_matchers::internal::MatcherInterface<NodeType> { public: explicit matcher_##DefineMatcher##OverloadId##Matcher( std::shared_ptr<llvm::Regex> RE) : Param(std::move(RE)) {} bool matches(const NodeType &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const override; private: std::shared_ptr<llvm::Regex> Param; }; } inline ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, std::shared_ptr<llvm::Regex>> DefineMatcher(llvm::StringRef Param, llvm::Regex::RegexFlags RegexFlags) { return ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, std::shared_ptr<llvm::Regex>>( ::clang::ast_matchers::internal::createAndVerifyRegex( Param, RegexFlags, #DefineMatcher)); } inline ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, std::shared_ptr<llvm::Regex>> DefineMatcher(llvm::StringRef Param) { return DefineMatcher(Param, llvm::Regex::NoFlags); } typedef ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, std::shared_ptr<llvm::Regex>> ( &DefineMatcher##_Type##OverloadId##Flags)( llvm::StringRef Param, llvm::Regex::RegexFlags RegexFlags); typedef ::clang::ast_matchers::internal::PolymorphicMatcher< internal::matcher_##DefineMatcher##OverloadId##Matcher, ReturnTypesF, std::shared_ptr<llvm::Regex>> (&DefineMatcher##_Type##OverloadId)( llvm::StringRef Param); template <typename NodeType, typename ParamT> bool internal:: matcher_##DefineMatcher##OverloadId##Matcher<NodeType, ParamT>::matches( const NodeType &Node, ::clang::ast_matchers::internal::ASTMatchFinder *Finder, ::clang::ast_matchers::internal::BoundNodesTreeBuilder *Builder) const



















































#endif
