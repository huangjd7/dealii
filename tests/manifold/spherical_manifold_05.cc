// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2014 - 2018 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------

// test tangent vectors for linear dependent positions

#include <deal.II/base/utilities.h>

#include <deal.II/grid/manifold_lib.h>

#include "../tests.h"


int
main()
{
  initlog();

  const SphericalManifold<2> manifold_2;
  const SphericalManifold<3> manifold_3;

  // get tangent vectors for positions that are on a line as seen from the
  // center
  deallog << manifold_2.get_tangent_vector(Point<2>(0.5, 0), Point<2>(1.0, 0))
          << std::endl
          << manifold_3.get_tangent_vector(Point<3>(0, 0, -0.5),
                                           Point<3>(0, 0, -1.0))
          << std::endl;
}
