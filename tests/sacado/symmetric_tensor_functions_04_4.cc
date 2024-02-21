// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2016 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------


// Test to check that tensor functions both compile and produce the right
// result when differentiated using the various auto-differentiable number
// types: Eigenvalues and eignvectors
//
// AD number type: Sacado Rad-DFad

#include "../tests.h"

#include "../ad_common_tests/symmetric_tensor_functions_04.h"

int
main()
{
  initlog();

  deallog.push("Double");
  {
    test_symmetric_tensor<2, double, AD::NumberTypes::sacado_rad_dfad>();
    test_symmetric_tensor<3, double, AD::NumberTypes::sacado_rad_dfad>();
    deallog << "OK" << std::endl;
  }
  deallog.pop();

  deallog << "OK" << std::endl;
}
