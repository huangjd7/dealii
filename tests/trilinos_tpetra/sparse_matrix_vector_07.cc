// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2004 - 2024 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// check SparseMatrix::matrix_norm_square

#include <deal.II/base/utilities.h>

#include <deal.II/lac/trilinos_tpetra_sparse_matrix.h>
#include <deal.II/lac/trilinos_tpetra_vector.h>

#include <iostream>
#include <vector>

#include "../tests.h"


void
test(LinearAlgebra::TpetraWrappers::Vector<double> &v,
     LinearAlgebra::TpetraWrappers::Vector<double> &w,
     LinearAlgebra::TpetraWrappers::Vector<double> &x)
{
  LinearAlgebra::TpetraWrappers::SparseMatrix<double> m(v.size(),
                                                        v.size(),
                                                        v.size());
  for (unsigned int i = 0; i < m.m(); ++i)
    for (unsigned int j = 0; j < m.m(); ++j)
      m.set(i, j, i + 2 * j);

  for (unsigned int i = 0; i < v.size(); ++i)
    {
      v(i) = i;
      w(i) = i + 1;
    }

  m.compress(VectorOperation::insert);
  v.compress(VectorOperation::insert);
  w.compress(VectorOperation::insert);

  // x=w-Mv
  const double s = m.residual(x, v, w);

  // make sure we get the expected result
  for (unsigned int i = 0; i < v.size(); ++i)
    {
      AssertThrow(v(i) == i, ExcInternalError());
      AssertThrow(w(i) == i + 1, ExcInternalError());

      double result = i + 1;
      for (unsigned int j = 0; j < m.m(); ++j)
        result -= (i + 2 * j) * j;

      AssertThrow(x(i) == result, ExcInternalError());
    }

  AssertThrow(s == x.l2_norm(), ExcInternalError());

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
        LinearAlgebra::TpetraWrappers::Vector<double> v;
        v.reinit(complete_index_set(100), MPI_COMM_WORLD);
        LinearAlgebra::TpetraWrappers::Vector<double> w;
        w.reinit(complete_index_set(100), MPI_COMM_WORLD);
        LinearAlgebra::TpetraWrappers::Vector<double> x;
        x.reinit(complete_index_set(100), MPI_COMM_WORLD);
        test(v, w, x);
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
