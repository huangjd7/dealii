// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2019 - 2022 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------

// Test MappingQCache::get_vertices()

#include <deal.II/fe/fe_nothing.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/mapping_q.h>
#include <deal.II/fe/mapping_q_cache.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria.h>

#include "../tests.h"

template <int dim>
void
do_test(const unsigned int degree)
{
  Triangulation<dim> tria;
  if (dim > 1)
    GridGenerator::hyper_ball(tria);
  else
    GridGenerator::hyper_cube(tria, -1, 1);

  MappingQ<dim>      mapping(degree);
  MappingQCache<dim> mapping_cache(degree);
  Point<dim>         shift;
  for (unsigned int d = 0; d < dim; ++d)
    shift[d] = -0.5 + 0.1 * d;

  FE_Q<dim> fe_q(degree);

  const auto position_lambda =
    [&](const typename Triangulation<dim>::cell_iterator &cell)
    -> std::vector<Point<dim>> {
    FE_Nothing<dim> fe;
    FEValues<dim>   fe_values(mapping,
                            fe,
                            Quadrature<dim>(fe_q.get_unit_support_points()),
                            update_quadrature_points);

    std::vector<Point<dim>> support_points_moved(fe_q.dofs_per_cell);
    fe_values.reinit(cell);
    for (unsigned int i = 0; i < fe_q.dofs_per_cell; ++i)
      {
        const Point<dim> point  = fe_values.quadrature_point(i);
        Point<dim>       out    = point + shift;
        support_points_moved[i] = out;
      }
    return support_points_moved;
  };

  mapping_cache.initialize(tria, position_lambda);

  deallog << "Testing degree " << degree << " in " << dim << 'D' << std::endl;
  for (const auto &cell : tria.cell_iterators())
    {
      deallog << "vertices on cell " << cell->id() << std::endl;
      for (const auto vertex : cell->vertex_indices())
        deallog << cell->vertex(vertex) << std::endl;
      const auto &mapping_vertices = mapping_cache.get_vertices(cell);
      deallog << "vertices given mapping" << std::endl;
      for (const auto &vertex : mapping_vertices)
        deallog << vertex << std::endl;
      deallog << std::endl;
    }
  deallog << std::endl;

  tria.refine_global(1);
  mapping_cache.initialize(tria, position_lambda);

  deallog << "Testing degree " << degree << " in " << dim << 'D' << std::endl;
  for (const auto &cell : tria.active_cell_iterators())
    {
      deallog << "vertices on cell " << cell->id() << std::endl;
      for (const auto vertex : cell->vertex_indices())
        deallog << cell->vertex(vertex) << std::endl;
      const auto &mapping_vertices = mapping_cache.get_vertices(cell);
      deallog << "vertices given mapping" << std::endl;
      for (const auto &vertex : mapping_vertices)
        deallog << vertex << std::endl;
      deallog << std::endl;
    }
  deallog << std::endl;
}


int
main()
{
  initlog();
  do_test<1>(1);
  do_test<1>(3);
  do_test<2>(1);
  do_test<2>(3);
  do_test<2>(4);
  do_test<3>(1);
  do_test<3>(2);
  do_test<3>(3);
}
