// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2007 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



// check the cells generated by the CylindricalManifold for the default
// axiparallel cylinder

#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/mapping_c1.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/manifold_lib.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include "../tests.h"


template <int dim>
void
check()
{
  AssertThrow(false, ExcInternalError());
}


template <>
void
check<2>()
{
  constexpr int      dim = 2;
  Triangulation<dim> triangulation;
  GridGenerator::cylinder(triangulation);
  triangulation.refine_global(2);

  for (typename Triangulation<dim>::active_cell_iterator cell =
         triangulation.begin_active();
       cell != triangulation.end();
       ++cell)
    for (const unsigned int i : GeometryInfo<dim>::vertex_indices())
      deallog << cell->vertex(i) << std::endl;
}


template <>
void
check<3>()
{
  constexpr int                  dim = 3;
  const CylindricalManifold<dim> boundary;
  Triangulation<dim>             triangulation;
  GridGenerator::cylinder(triangulation);
  triangulation.set_manifold(0, boundary);
  triangulation.refine_global(2);

  for (typename Triangulation<dim>::active_cell_iterator cell =
         triangulation.begin_active();
       cell != triangulation.end();
       ++cell)
    for (const unsigned int i : GeometryInfo<dim>::vertex_indices())
      deallog << cell->vertex(i) << std::endl;
}


int
main()
{
  initlog();

  check<2>();
  check<3>();
}
