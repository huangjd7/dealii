// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2005 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



/* Author: Ralf Hartmann, 2005; the DoFHandler class had troubles
   when the triangulation had unused vertices before, make sure that
   this is now fixed. */

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_dgq.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>

#include "../tests.h"



int
main()
{
  initlog();
  deallog << std::setprecision(2);

  const unsigned int dim = 2;
  Triangulation<dim> tria;
  GridGenerator::hyper_cube(tria);

  FE_DGQ<dim>     fe(1);
  DoFHandler<dim> dof_handler(tria);

  tria.begin_active()->set_refine_flag();
  tria.execute_coarsening_and_refinement();

  Triangulation<dim>::active_cell_iterator cell = tria.begin_active(),
                                           endc = tria.end();
  for (; cell != endc; ++cell)
    cell->set_coarsen_flag();
  tria.execute_coarsening_and_refinement();

  dof_handler.distribute_dofs(fe);

  deallog << "OK" << std::endl;
}
