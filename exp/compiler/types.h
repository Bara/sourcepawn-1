// vim: set sts=2 ts=8 sw=2 tw=99 et:
// 
// Copyright (C) 2012 David Anderson
// 
// This file is part of SourcePawn.
// 
// SourcePawn is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
// 
// SourcePawn is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with
// SourcePawn. If not, see http://www.gnu.org/licenses/.

#ifndef _include_jc_types_h_
#define _include_jc_types_h_

#include "assert.h"
#include "tokens.h"
#include "string-pool.h"
#include "pool-allocator.h"

namespace sp {

struct ReportingContext;
class TypeSpecifier;
class TypeExpr;

enum class PrimitiveType : uint32_t
{
  // A boolean type semantically represents 0 or 1. As an optimization, its
  // internal storage type is the same as int32, and any non-zero value
  // compares positively to "true".
  Bool,

  // A char is an 8-bit signed integer that has some extra coercion rules for
  // legacy code.
  Char,

  // The "implicit int" pseudo-type that we use when an int type was not
  // explicitly specified.
  ImplicitInt,

  Int8,
  Uint8,
  Int16,
  Uint16,
  Int32,
  Uint32,
  Int64,
  Uint64,
  NativeInt,
  NativeUint,

  // Floating point values are IEEE-754 floats.
  Float,
  Double,

  TOTAL
};

static const size_t kTotalPrimitiveTypes = size_t(PrimitiveType::TOTAL);

enum class Qualifiers : uint32_t
{
  // None (the default).
  None      = 0x0,

  // Storage and mutability is constant.
  Const     = 0x1,
};
KE_DEFINE_ENUM_OPERATORS(Qualifiers);

class Type;
class TypedefType;
typedef FixedPoolList<Type *> TypeList;

#define TYPE_ENUM_MAP(_)        \
  _(Enum)                       \
  _(Reference)                  \
  _(Array)                      \
  _(Function)                   \
  _(Typeset)                    \
  _(Struct)

#define FORWARD_DECLARE(name) class name##Type;
TYPE_ENUM_MAP(FORWARD_DECLARE)
#undef FORWARD_DECLARE
class RecordType;

// A Type object represents a builtin or user-created type. Core types have
// singleton instances (primitives, void, etc), and extremely common types
// are cached. All Type instantiation occurs through TypeManager.
//
// Types that have more complex state (such as arrays or structs) need to be
// casted to derived types to get more detail. These casts are initiated
// through methods like toArray or toStruct. IT IS IMPORTANT TO NOTE, that
// these functions will discard qualifiers. Qualifiers are stored on an
// intervening wrapping type, and methods like isStruct/toString will peek
// past the qualifying wrapper.
class Type : public PoolObject
{
 public:
  enum class Kind {
    // A type that could not be resolved during type resolution.
    Unresolvable,

    // A primitive is a plain-old-data type.
    Primitive,

    // Void is an internal type used to specify that a function returns no
    // value.
    Void,

    // The "implicit void" pseudo-type that we use when an int type was not
    // explicitly specified, but no values are ever returned.
    ImplicitVoid,

    // Enums are plain-old-data with int32 storage. They allow for extra
    // type checking that would not be possible with integers.
    Enum,

    // A named typedef.
    Typedef,

    // Qualifying wrapper.
    Qualifier,

    // Unchecked is a magic type that has implicit, bitwise coercion to
    // int32, float, bool, or an enum.
    Unchecked,

    // An array is a fixed-length vector of any other type.
    Array,

    // A discriminated union of types.
    Typeset,

    // A value-typed composite type.
    Struct,

    // A reference type may only be specified on parameters, and
    // references may only be computed to primitives or enums.
    Reference,

    // A function type encapsulates a function signature.
    Function,

    // The type used for the "Function" tag.
    MetaFunction,

    // The type used for "null".
    NullType,

    // A temporary type used to indicate that an overloaded function was not
    // resolved to a specific address. This happens, for example, when a
    // NameProxy resolves to an overloaded function symbol. If the function
    // cannot or is not coerced or casted to a specific overload, the special
    // OverloadedFunction type will leak out.
    OverloadedFunction
  };

  void init(Kind kind, Type *root = NULL);

 public:
  Type(Kind kind)
   : kind_(kind)
  {
    canonical_ = this;
  }
  Type(Kind kind, Type *canonical)
   : kind_(kind),
     canonical_(canonical)
  {
  }

 public:
  static Type *NewVoid();
  static Type *NewImplicitVoid();
  static Type *NewMetaFunction();
  static Type *NewUnchecked();
  static Type *NewNullType();
  static Type *NewPrimitive(PrimitiveType type);
  static Type *NewQualified(Type *type, Qualifiers quals);
  static Type *NewImportable();
  static Type *NewOverloadedFunction();
  static bool Compare(Type *left, Type *right);

  bool isPrimitive() {
    return canonical()->kind_ == Kind::Primitive;
  }
  bool isVoid() {
    return canonical()->kind_ == Kind::Void;
  }
  bool isImplicitVoid() {
    return canonical()->kind_ == Kind::ImplicitVoid;
  }
  bool isUnchecked() {
    return canonical()->kind_ == Kind::Unchecked;
  }
  bool isMetaFunction() {
    return canonical()->kind_ == Kind::MetaFunction;
  }
  bool isUnresolvable() {
    return canonical()->kind_ == Kind::Unresolvable;
  }
  bool isNullType() {
    return canonical()->kind_ == Kind::NullType;
  }

  Kind canonicalKind() {
    return canonical()->kind_;
  }

  // Note that isTypedef() is special in that it does not bypass wrappers.
  bool isResolvedTypedef() const {
    return kind_ == Kind::Typedef && canonical_ != this;
  }
  bool isUnresolvedTypedef() const {
    return kind_ == Kind::Typedef && canonical_ == this;
  }
  TypedefType *toTypedef() {
    assert(kind_ == Kind::Typedef);
    return (TypedefType *)this;
  }
  TypedefType *asTypedef() {
    if (kind_ != Kind::Typedef)
      return nullptr;
    return (TypedefType *)this;
  }

  bool canUseInReferenceType() {
    return !isArray() && !isReference();
  }
  bool canBeUsedInConstExpr() {
    return isPrimitive() || isEnum();
  }

  // For certain types, 'const' is meaningless in some scopes. For example,
  // int, enum, and function types in unions or as arguments basically don't
  // mean anything as 'const'.
  bool hasMeaninglessConstCoercion() {
    switch (canonicalKind()) {
      case Kind::Primitive:
      case Kind::Enum:
      case Kind::Function:
      case Kind::MetaFunction:
      case Kind::Unchecked:
        return true;
      default:
        return false;
    }
  }

  // void and & cannot be const. We compute lvalue constness separately.
  bool canUseInConstType() {
    return !isVoid() && !isReference();
  }

  PrimitiveType primitive() {
    assert(isPrimitive());
    return canonical()->primitive_;
  }

  // Same as primitive(), but return int32 for implicit-int and int8 for char.
  PrimitiveType semanticPrimitive() {
    switch (primitive()) {
      case PrimitiveType::Char:
        return PrimitiveType::Int8;
      case PrimitiveType::ImplicitInt:
        return PrimitiveType::Int32;
      default:
        return primitive();
    }
  }

  bool isPod() {
    return isPrimitive() || isEnum() || isUnchecked();
  }
  PrimitiveType pod() {
    assert(isPod());
    return isPrimitive()
           ? (primitive() == PrimitiveType::Bool)
             ? PrimitiveType::Int32
             : primitive()
           : PrimitiveType::Int32;
  }
  bool isChar() {
    return isPrimitive() && primitive() == PrimitiveType::Char;
  }
  bool isFloat() {
    return isPrimitive() && primitive() == PrimitiveType::Float;
  }
  bool isInt32() {
    return isPrimitive() && primitive() == PrimitiveType::Int32;
  }
  bool isInt32OrEnum() {
    return isInt32() || isEnum();
  }
  bool isBool() {
    return isPrimitive() && primitive() == PrimitiveType::Bool;
  }

  // This is a very specific check intended for phases which need to know
  // if a type was not specified, and thus it ignores qualifiers.
  bool isImplicitInt() const {
    return kind_ == Kind::Primitive && primitive_ == PrimitiveType::ImplicitInt;
  }

  // Check whether this type pointer is actually resolved to anything. This
  // will return true for Unresolvable types, since technically it has been
  // resolved to an error. It will return false if the canonical type is a
  // typedef that has not been resolved.
  bool isResolved() {
    // Canonical type should only be a typedef if it is unresolved.
    assert(canonical()->kind_ != Kind::Typedef ||
           canonical()->isUnresolvedTypedef());
    return canonical()->isUnresolvedTypedef();
  }

  Qualifiers qualifiers() {
    Type *norm = normalized();
    if (norm->kind_ != Kind::Qualifier)
      return Qualifiers::None;
    return norm->qualifiers_;
  }
  bool isQualified() {
    return normalized()->kind_ == Kind::Qualifier;
  }
  bool isConst() {
    return (qualifiers() & Qualifiers::Const) == Qualifiers::Const;
  }
  Type *unqualified() {
    return canonical();
  }

#define CAST(name)                                \
  bool is##name() {                               \
    return canonical()->kind_ == Kind::name;      \
  }                                               \
  name##Type *to##name() {                        \
    assert(is##name());                           \
    return (name##Type *)canonical();             \
  }                                               \
  name##Type *as##name() {                        \
    if (!is##name())                              \
      return nullptr;                             \
    return to##name();                            \
  }
  TYPE_ENUM_MAP(CAST)
#undef CAST

  // Record type covers multiple types, so we have separate accessors here.
  bool isRecord() {
    return isStruct();
  }
  RecordType *toRecord() {
    assert(isRecord());
    return (RecordType *)this;
  }
  RecordType *asRecord() {
    if (!isRecord())
      return nullptr;
    return toRecord();
  }

 protected:
  bool isWrapped() const {
    return canonical_ != this;
  }

 public:
  // Return the underlying canonical type, meaning, the current type
  // normalized and without its wrappings.
  Type *canonical() {
    // Keep canonical bits up to date.
    if (isWrapped() && canonical_->isResolvedTypedef())
      normalize();
    return canonical_;
  }

 protected:
  // Desugar typedefs until we reach a valid type or a typedef that has not
  // been resolved.
  Type *normalized() {
    if (isResolvedTypedef()) {
      if (canonical_->isResolvedTypedef())
        normalize();

      // We might still return a typedef here, for example if we are unresovled
      // or we're wrapping something unresolved.
      if (isWrapped())
        return canonical_;
    }

    // We're either an actual type or a qualifier.
    return this;
  }

  // Collapse redundant pairs of wrappers.
  //   (qual -> qual) -> (qual)
  //   (typedef -> qual) -> (qual)
  //   (qual -> typedef) -> (qual)
  //
  // If a typedef chain ends in an unresolved typedef, then
  // normalization stops.
  void normalize() {
    assert(isWrapped() && canonical_->isResolvedTypedef());
    do {
      canonical_ = canonical_->normalized();

      if (canonical_->kind_ != Kind::Qualifier)
        break;

      assert(kind_ == Kind::Qualifier || kind_ == Kind::Typedef);

      // Assume the qualifiers of our inner type, then reach behind the quals.
      // This could cause us to hit another typedef, so we have to be prepared
      // to collapse again. This could happen with something really nasty like:
      //     typedef E const D
      //     typedef D const C
      //     typedef C const B
      // etc...
      kind_ = Kind::Qualifier;
      qualifiers_ |= canonical_->qualifiers_;
      canonical_ = canonical_->canonical_;
    } while (isWrapped() && canonical_->isResolvedTypedef());
  }

 protected:
  Kind kind_;
  union {
    PrimitiveType primitive_;
    Qualifiers qualifiers_;
    uint32_t flags_;
  };

  // For unqualified types, this points to |this|. For qualified types, it
  // points to the unqualified original type.
  //
  // For typedefs, it points to |this| until the actual canonical is known.
  Type *canonical_;
};

class MethodmapDecl;

class EnumType : public Type
{
  EnumType()
   : Type(Kind::Enum),
     methodmap_(nullptr)
  {
  }

 public:
  static EnumType *New(Atom *name);

  Atom *name() {
    return name_;
  }

  void setMethodmap(MethodmapDecl *methodmap) {
    assert(!methodmap_);
    methodmap_ = methodmap;
  }
  MethodmapDecl *methodmap() const {
    return methodmap_;
  }

 private:
  Atom *name_;
  MethodmapDecl *methodmap_;
};

class ReferenceType : public Type
{
  ReferenceType()
   : Type(Kind::Reference)
  {
  }

 public:
  static ReferenceType *New(Type *contained);

  Type *contained() {
    return contained_;
  }

 private:
  Type *contained_;
};

class ArrayType : public Type
{
  ArrayType(Kind kind)
   : Type(kind)
  {
  }

 public:
  // This array does not have a size specified. It may or may not internally
  // have a fixed size.
  static const int kUnsized = -1;

  // This array's size is indeterminate. This is different from Unsized; an
  // indeterminate rank implies that the number of elements is fixed, but
  // not known. For example:
  //   int a[][] = {
  //      {1, 2},
  //      {3, 4, 5},
  //   };
  //
  // This is an ArrayType(ArrayType(Primitive::Int32, Indeterminate), 2). The
  // initial rank is known to have a size of 2, but the next rank's size is not
  // known. The distinction is for SourcePawn 1 sizeof() and assignment
  // compatibility.
  static const int kIndeterminate = -2;

  // Maximum array length.
  static const int kMaxSize = INT_MAX / 4;

  // Maximum size of an array. We choose this value because we can compute
  // addresses as multiples of an index without overflowing.
  static ArrayType *New(Type *contained, int elements);

  Type *innermost() const {
    Type *temp = contained();
    while (temp->isArray())
      temp = temp->toArray()->contained();
    return temp;
  }

  Type *contained() const {
    return contained_;
  }
  bool hasFixedLength() const {
    return elements_ >= 0;
  }
  int32_t fixedLength() const {
    assert(hasFixedLength());
    return elements_;
  }

  bool equalTo(ArrayType *other) {
    return elements_ == other->elements_ &&
           contained_ == other->contained_;
  }

 private:
  int32_t elements_;
  Type *contained_;
};

class TypedefType : public Type
{
  TypedefType(Atom *name)
   : Type(Kind::Typedef),
     name_(name)
  {}

 public:
  static TypedefType *New(Atom *name);

  Atom *name() const {
    return name_;
  }

  // The actual type may be null if it is unresolved. It is only guaranteed to
  // be set if the TypeResolver phase passes.
  Type *actual() const {
    return actual_;
  }
  void resolve(Type *actual);

 private:
  Atom *name_;
  Type *actual_;
};

class FunctionSignature;

class FunctionType : public Type
{
  FunctionType(FunctionSignature *signature)
   : Type(Kind::Function),
     signature_(signature)
  {
  }

 public:
  static FunctionType *New(FunctionSignature *sig);

  FunctionSignature *signature() const {
    return signature_;
  }

 private:
  FunctionSignature *signature_;
};

class RecordDecl;

class RecordType : public Type
{
 public:
  RecordType(Kind kind, RecordDecl *decl)
   : Type(kind),
     decl_(decl)
  {}

  Atom *name() const;

 private:
  RecordDecl *decl_;
};

class TypesetDecl;

class TypesetType : public Type
{
  TypesetType(TypesetDecl *decl)
   : Type(Kind::Typeset),
     decl_(decl)
  {}

 public:
  static TypesetType *New(TypesetDecl *decl);

  Atom *name() const;
  size_t numTypes() const;
  Type *typeAt(size_t i) const;

 private:
  TypesetDecl *decl_;
};

class StructType : public RecordType
{
  StructType(RecordDecl *decl)
   : RecordType(Kind::Struct, decl)
  {}

 public:
  static StructType *New(RecordDecl *decl);
};

static inline size_t
SizeOfPrimitiveType(PrimitiveType type)
{
  switch (type) {
    case PrimitiveType::Bool:
    case PrimitiveType::Int8:
    case PrimitiveType::Uint8:
      return 1;
    case PrimitiveType::Int16:
    case PrimitiveType::Uint16:
      return 2;
    case PrimitiveType::Int32:
    case PrimitiveType::Uint32:
    case PrimitiveType::Float:
      return 4;
    case PrimitiveType::Int64:
    case PrimitiveType::Uint64:
    case PrimitiveType::Double:
      return 8;
    case PrimitiveType::NativeInt:
    case PrimitiveType::NativeUint:
      return sizeof(void *);
    default:
      assert(false);
  }
}

static inline bool
IsPrimitiveTypeSigned(PrimitiveType type)
{
  switch (type) {
    case PrimitiveType::Int8:
    case PrimitiveType::Int16:
    case PrimitiveType::Int32:
    case PrimitiveType::Int64:
    case PrimitiveType::NativeInt:
      return true;
    case PrimitiveType::Uint16:
    case PrimitiveType::Uint8:
    case PrimitiveType::Uint32:
    case PrimitiveType::Uint64:
    case PrimitiveType::NativeUint:
      return false;
    default:
      assert(false);
  }
}

static inline PrimitiveType
SignedTypeForIntegerSize(size_t size)
{
  switch (size) {
    case 1:
      return PrimitiveType::Int8;
    case 2:
      return PrimitiveType::Int16;
    case 4:
      return PrimitiveType::Int32;
    default:
      assert(size == 8);
      return PrimitiveType::Int64;
  }
}

static inline PrimitiveType
UnsignedTypeForIntegerSize(size_t size)
{
  switch (size) {
    case 1:
      return PrimitiveType::Uint8;
    case 2:
      return PrimitiveType::Uint16;
    case 4:
      return PrimitiveType::Uint32;
    default:
      assert(size == 8);
      return PrimitiveType::Uint64;
  }
}

// This should probably be in the type manager... but it should never leak past
// type resolution.
extern Type UnresolvableType;

const char *GetPrimitiveName(PrimitiveType type);

enum class TypeDiagFlags
{
  None = 0x0,
  Names = 0x1
};
KE_DEFINE_ENUM_OPERATORS(TypeDiagFlags);

// Build a type name for diagnostics, with an optional name for building a
// declaration.
AString BuildTypeName(Type *type, Atom *name = nullptr, TypeDiagFlags flags = TypeDiagFlags::None);
AString BuildTypeName(const TypeSpecifier *spec, Atom *name, TypeDiagFlags flags = TypeDiagFlags::None);
AString BuildTypeName(const TypeExpr &te, Atom *name, TypeDiagFlags flags = TypeDiagFlags::None);

// Compute the size of a type. It must be an array type, and it must have
// at least as many levels as specified, and the specified level must be
// determinate (fixed).
//
// On failure, returns 0 and an error will have been reported.
int32_t ComputeSizeOfType(ReportingContext &cc, Type *type, size_t level);

// Test whether two function types are equivalent.
bool AreFunctionTypesEqual(FunctionType *a, FunctionType *b);

// Test whether two types are equivalent for the purpose of signature matching
// and casting. If |context| is const, both types are considered const.
bool AreTypesEquivalent(Type *a, Type *b, Qualifiers context);

}

#endif // _include_jc_types_h_