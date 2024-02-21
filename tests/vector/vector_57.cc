// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2004 - 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// check Vector::is_non_zero

#include <deal.II/lac/vector.h>

#include <vector>

#include "../tests.h"


void
test(Vector<double> &v)
{
  // set only certain elements of the
  // vector. they are all positive
  std::vector<bool> pattern(v.size(), false);
  for (unsigned int i = 0; i < v.size(); i += 1 + i)
    {
      v(i) += i;
      pattern[i] = true;
    }

  v.compress();

  // check that the vector is really
  // non-negative
  AssertThrow(v.is_non_negative() == true, ExcInternalError());

  // then set a single element to a negative
  // value and check again
  v(v.size() / 2) = -1;
  AssertThrow(v.is_non_negative() == false, ExcInternalError());

  deallog << "OK" << std::endl;
}



int
main()
{
  initlog();

  try
    {
      Vector<double> v(100);
      test(v);
    }
  catch (const std::exception &exc)
    {
      deallog << std::endl
              << std::endl
              << "----------------------------------------------------"
              << std::endl;
      deallog << "Exception on processing: " << std::endl
              << exc.what() << std::endl
              << "Aborting!" << std::endl
              << "----------------------------------------------------"
              << std::endl;

      return 1;
    }
  catch (...)
    {
      deallog << std::endl
              << std::endl
              << "----------------------------------------------------"
              << std::endl;
      deallog << "Unknown exception!" << std::endl
              << "Aborting!" << std::endl
              << "----------------------------------------------------"
              << std::endl;
      return 1;
    };
}
