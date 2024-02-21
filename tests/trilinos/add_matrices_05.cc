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



// check SparseMatrix::add(SparseMatrix) where the other matrix has either
// more or less entries (but with reasonable entries in any case)

#include <deal.II/base/utilities.h>

#include <deal.II/lac/trilinos_sparse_matrix.h>

#include <iostream>

#include "../tests.h"


void
test(TrilinosWrappers::SparseMatrix &m)
{
  TrilinosWrappers::SparseMatrix m2(m.m(), m.n(), 0);

  // first set a few entries one-by-one
  for (unsigned int i = 0; i < m.m(); ++i)
    for (unsigned int j = 0; j < m.n(); ++j)
      if ((i + 2 * j + 1) % 3 == 0)
        {
          m.set(i, j, i * j * .5 + .5);
          m2.set(i, j, 1.);
        }
      else if (j % 2 == 0)
        m2.set(i, j, 0);

  m.compress(VectorOperation::insert);
  m2.compress(VectorOperation::insert);

  deallog << "Matrix nonzeros: " << m.n_nonzero_elements() << ' '
          << m2.n_nonzero_elements() << std::endl;

  m.print(deallog.get_file_stream());
  deallog << std::endl;

  m.add(1, m2);
  m.print(deallog.get_file_stream());
  deallog << std::endl;

  m.add(-1, m2);
  m2.add(-1, m);
  m2.print(deallog.get_file_stream());
  deallog << std::endl;

  m.add(-1, m2);
  m.print(deallog.get_file_stream());

  deallog << "OK" << std::endl;
}



int
main(int argc, char **argv)
{
  initlog();

  Utilities::MPI::MPI_InitFinalize mpi_initialization(
    argc, argv, testing_max_num_threads());

  try
    {
      {
        TrilinosWrappers::SparseMatrix m(5U, 6U, 3U);

        test(m);
      }
    }
  catch (const std::exception &exc)
    {
      std::cerr << std::endl
                << std::endl
                << "----------------------------------------------------"
                << std::endl;
      std::cerr << "Exception on processing: " << std::endl
                << exc.what() << std::endl
                << "Aborting!" << std::endl
                << "----------------------------------------------------"
                << std::endl;

      return 1;
    }
  catch (...)
    {
      std::cerr << std::endl
                << std::endl
                << "----------------------------------------------------"
                << std::endl;
      std::cerr << "Unknown exception!" << std::endl
                << "Aborting!" << std::endl
                << "----------------------------------------------------"
                << std::endl;
      return 1;
    };
}
