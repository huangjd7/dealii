// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2003 - 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------



//   Like parameter_handler_03, but use a MultipleSelection pattern that starts
//   with a space; eat that space

#include <deal.II/base/parameter_handler.h>

#include "../tests.h"


int
main()
{
  try
    {
      initlog();

      ParameterHandler prm;
      prm.enter_subsection("Testing");
      prm.declare_entry("string list1",
                        "a",
                        Patterns::List(Patterns::Selection(" a|b|c|d|e|f|g|h")),
                        "docs 1");
      prm.declare_entry("string list2",
                        "h",
                        Patterns::List(Patterns::Selection("a|b|c|d|e|f|g|h ")),
                        "docs 2");
      prm.declare_entry("int", "1", Patterns::Integer());
      prm.declare_entry("double", "3.1415926", Patterns::Double(), "docs 3");
      prm.leave_subsection();

      // read and then write parameters
      prm.parse_input(SOURCE_DIR "/prm/parameter_handler_3_with_space.prm");
      prm.print_parameters(deallog.get_file_stream(), ParameterHandler::Text);
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

  return 0;
}
