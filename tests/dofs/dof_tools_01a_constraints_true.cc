// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2003 - 2022 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------


#include <deal.II/lac/sparsity_pattern.h>

#include "../tests.h"

#include "dof_tools_common.h"

// check
//   DoFTools::
//   make_sparsity_pattern (const DoFHandler<dim> &,
//                      SparsityPattern       &,
//                          AffineConstraints<double>,
//                          true);



template <int dim>
void
check_this(const DoFHandler<dim> &dof_handler)
{
  AffineConstraints<double> cm;
  DoFTools::make_hanging_node_constraints(dof_handler, cm);
  cm.close();

  // create sparsity pattern
  SparsityPattern sp(dof_handler.n_dofs(),
                     dof_handler.max_couplings_between_dofs());
  DoFTools::make_sparsity_pattern(dof_handler, sp, cm, true);
  sp.compress();

  // write out 10 lines of this
  // pattern (if we write out the
  // whole pattern, the output file
  // would be in the range of 40 MB)
  for (unsigned int l = 0; l < 10; ++l)
    {
      const unsigned int line = l * (sp.n_rows() / 10);
      for (unsigned int c = 0; c < sp.row_length(line); ++c)
        deallog << sp.column_number(line, c) << ' ';
      deallog << std::endl;
    }

  // write out some other indicators
  deallog << sp.bandwidth() << std::endl
          << sp.max_entries_per_row() << std::endl
          << sp.n_nonzero_elements() << std::endl;

  unsigned int hash = 0;
  for (unsigned int l = 0; l < sp.n_rows(); ++l)
    hash +=
      l *
      (sp.row_length(l) + (sp.begin(l) - sp.begin()) +
       (sp.row_length(l) > 1 ? std::next(sp.begin(l)) : sp.begin(l))->column());
  deallog << hash << std::endl;
}
