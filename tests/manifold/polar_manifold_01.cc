// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2016 - 2018 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------


// Test spherical manifold on hyper shells.

#include "../tests.h"


// all include files you need here
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/manifold_lib.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

// Helper function
template <int dim, int spacedim>
void
test(unsigned int ref = 1)
{
  deallog << "Testing dim " << dim << ", spacedim " << spacedim << std::endl;

  PolarManifold<dim, spacedim> manifold;

  Triangulation<dim, spacedim> tria;
  GridGenerator::hyper_shell(tria, Point<spacedim>(), .3, .6, 12);

  for (typename Triangulation<dim, spacedim>::active_cell_iterator cell =
         tria.begin_active();
       cell != tria.end();
       ++cell)
    {
      cell->set_all_manifold_ids(1);
    }

  tria.set_manifold(1, manifold);
  tria.refine_global(1);

  GridOut gridout;
  gridout.write_msh(tria, deallog.get_file_stream());

  // char fname[50];
  // sprintf(fname, "mesh_%d_%d.msh", dim, spacedim);
  // std::ofstream of(fname);
  // gridout.write_msh(tria, of);
}

int
main()
{
  initlog();

  test<2, 2>();
  test<3, 3>();
}
