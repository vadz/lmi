// -*- C++ -*-
//
// Copyright (C) 1998, 1999, 2000, 2002  Los Alamos National Laboratory,
// Copyright (C) 1998, 1999, 2000, 2002  CodeSourcery, LLC
//
// This file is part of FreePOOMA.
//
// FreePOOMA is free software; you can redistribute it and/or modify it
// under the terms of the Expat license.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Expat
// license for more details.
//
// You should have received a copy of the Expat license along with
// FreePOOMA; see the file LICENSE.
//

#ifndef PETE_PETE_FOREACH_H
#define PETE_PETE_FOREACH_H

///////////////////////////////////////////////////////////////////////////////
//
// WARNING: THIS FILE IS FOR INTERNAL PETE USE. DON'T INCLUDE IT YOURSELF
//
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//
// CLASS NAME
//   ForEach<Expr, FTag, CTag>
//   forEach(Expr& e, FTag& f, CTag& c)
//
// Expr is the type of the expression tree.
// FTag is the type of the leaf tag.
// CTag is the type of the combiner tag.
//
// ForEach<Expr,FTag,CTag>::apply(Expr &e,FTag& f,CTag& c) is a function
// that traverses the expression tree defined by e, applies the functor f
// to each leaf and combines the results together using the combiner c.
// The type of object returned is given by:
// typename ForEach<Expr,FTag,CTag>::Type_t
// the function forEachTag(Expr& e,FTag& f,CTag& c) duplicates the action
// of ForEach::apply and is provided for convenience.  (You don't have to
// type the template arguments.)
//
// This generic ForEach functor differs from the original
// PETE forEach in 3 ways:
//  1) The user should not specialize ForEach.
//     Specializing TagFunctor and TagCombiner should take care of
//     the behaviour at leaves and nodes and the user knows nothing
//     of the tree.
//  2) It's a functor, so it contains generic type computations.
//     The typedef Type_t gives you the type of the result for any
//     user defined type and method for combining that type.
//  3) It handles generic combination.  Previously the user had the
//     choice of using the operators in the tree to produce the expression
//     return type or defining a combiner with a uniform return type.
//
//-----------------------------------------------------------------------------

// Default behaviour assumes you're at a leaf.

template<class Expr, class FTag, class CTag>
struct ForEach
{
  typedef typename LeafFunctor<Expr, FTag>::Type_t Type_t;
  inline static
  Type_t apply(const Expr &expr, const FTag &f, const CTag &)
  {
    return LeafFunctor<Expr, FTag>::apply(expr, f);
  }
};

template<class Expr, class FTag, class CTag>
inline typename ForEach<Expr,FTag,CTag>::Type_t
forEach(const Expr &e, const FTag &f, const CTag &c)
{
  return ForEach<Expr, FTag, CTag>::apply(e, f, c);
}

template<class Op, class A, class FTag, class CTag>
struct ForEach<UnaryNode<Op, A>, FTag, CTag>
{
  typedef typename ForEach<A, FTag, CTag>::Type_t TypeA_t;
  typedef typename Combine1<TypeA_t, Op, CTag>::Type_t Type_t;
  inline static
  Type_t apply(const UnaryNode<Op, A> &expr, const FTag &f,
    const CTag &c)
  {
    return Combine1<TypeA_t, Op, CTag>::
      combine(ForEach<A, FTag, CTag>::apply(expr.child(), f, c), c);
  }
};

template<class Op, class A, class B, class FTag, class CTag>
struct ForEach<BinaryNode<Op, A, B>, FTag, CTag >
{
  typedef typename ForEach<A, FTag, CTag>::Type_t TypeA_t;
  typedef typename ForEach<B, FTag, CTag>::Type_t TypeB_t;
  typedef typename Combine2<TypeA_t, TypeB_t, Op, CTag>::Type_t Type_t;
  inline static
  Type_t apply(const BinaryNode<Op, A, B> &expr, const FTag &f,
	       const CTag &c)
  {
    return Combine2<TypeA_t, TypeB_t, Op, CTag>::
      combine(ForEach<A, FTag, CTag>::apply(expr.left(), f, c),
              ForEach<B, FTag, CTag>::apply(expr.right(), f, c),
	      c);
  }
};

template<class Op, class A, class B, class C, class FTag, class CTag>
struct ForEach<TrinaryNode<Op, A, B, C>, FTag, CTag >
{
  typedef typename ForEach<A, FTag, CTag>::Type_t TypeA_t;
  typedef typename ForEach<B, FTag, CTag>::Type_t TypeB_t;
  typedef typename ForEach<C, FTag, CTag>::Type_t TypeC_t;
  typedef typename Combine3<TypeA_t, TypeB_t, TypeC_t, Op, CTag>::Type_t
    Type_t;
  inline static
  Type_t apply(const TrinaryNode<Op, A, B, C> &expr, const FTag &f,
	       const CTag &c)
  {
    return Combine3<TypeA_t, TypeB_t, TypeC_t, Op, CTag>::
      combine(ForEach<A, FTag, CTag>::apply(expr.left(), f, c),
	      ForEach<B, FTag, CTag>::apply(expr.middle(), f, c),
	      ForEach<C, FTag, CTag>::apply(expr.right(), f, c),
	      c);
  }
};

#ifndef PETE_USER_DEFINED_EXPRESSION

template<class T> class Expression;

template<class T, class FTag, class CTag>
struct ForEach<Expression<T>, FTag, CTag>
{
  typedef typename ForEach<T, FTag, CTag>::Type_t Type_t;
  inline static
  Type_t apply(const Expression<T> &expr, const FTag &f,
	       const CTag &c)
  {
    return ForEach<T, FTag, CTag>::apply(expr.expression(), f, c);
  }
};

#endif // !PETE_USER_DEFINED_EXPRESSION

template<class T> struct Reference;

template<class T, class FTag, class CTag>
struct ForEach<Reference<T>, FTag, CTag>
{
  typedef typename ForEach<T, FTag, CTag>::Type_t Type_t;
  inline static
  Type_t apply(const Reference<T> &ref, const FTag &f,
	       const CTag &c)
  {
    return ForEach<T, FTag, CTag>::apply(ref.reference(), f, c);
  }
};

#endif // PETE_PETE_FOREACH_H

// ACL:rcsinfo
// ----------------------------------------------------------------------
// $RCSfile: ForEach.h,v $   $Author: chicares $
// $Revision: 1.2 $   $Date: 2008-09-07 17:38:20 $
// ----------------------------------------------------------------------
// ACL:rcsinfo
