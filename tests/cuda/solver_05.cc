// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2018 - 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------

// Check that dealii::SolverFGMRES works with CUDAWrappers::SparseMatrix

#include <deal.II/base/cuda.h>
#include <deal.II/base/exceptions.h>

#include <deal.II/lac/cuda_sparse_matrix.h>
#include <deal.II/lac/precondition.h>
#include <deal.II/lac/read_write_vector.h>
#include <deal.II/lac/solver_control.h>
#include <deal.II/lac/solver_gmres.h>
#include <deal.II/lac/vector.h>

#include "../tests.h"

#include "../testmatrix.h"

void
test(Utilities::CUDA::Handle &cuda_handle)
{
  // Build the sparse matrix on the host
  const unsigned int   problem_size = 10;
  unsigned int         size         = (problem_size - 1) * (problem_size - 1);
  FDMatrix             testproblem(problem_size, problem_size);
  SparsityPattern      structure(size, size, 5);
  SparseMatrix<double> A;
  testproblem.five_point_structure(structure);
  structure.compress();
  A.reinit(structure);
  testproblem.five_point(A);

  // Solve on the host
  PreconditionIdentity prec_no;
  SolverControl        control(100, 1.e-10);
  SolverFGMRES<>       fgmres_host(control);
  Vector<double>       sol_host(size);
  Vector<double>       rhs_host(size);
  for (unsigned int i = 0; i < size; ++i)
    rhs_host[i] = static_cast<double>(i);
  fgmres_host.solve(A, sol_host, rhs_host, prec_no);

  // Solve on the device
  CUDAWrappers::SparseMatrix<double>          A_dev(cuda_handle, A);
  LinearAlgebra::CUDAWrappers::Vector<double> sol_dev(size);
  LinearAlgebra::CUDAWrappers::Vector<double> rhs_dev(size);
  LinearAlgebra::ReadWriteVector<double>      rw_vector(size);
  for (unsigned int i = 0; i < size; ++i)
    rw_vector[i] = static_cast<double>(i);
  rhs_dev.import_elements(rw_vector, VectorOperation::insert);
  SolverFGMRES<LinearAlgebra::CUDAWrappers::Vector<double>> fgmres_dev(control);
  fgmres_dev.solve(A_dev, sol_dev, rhs_dev, prec_no);

  // Check the result
  rw_vector.import_elements(sol_dev, VectorOperation::insert);
  for (unsigned int i = 0; i < size; ++i)
    AssertThrow(std::fabs(rw_vector[i] - sol_host[i]) < 1e-8,
                ExcInternalError());
}

int
main()
{
  initlog();
  deallog.depth_console(0);

  init_cuda();

  Utilities::CUDA::Handle cuda_handle;
  test(cuda_handle);

  deallog << "OK" << std::endl;

  return 0;
}
