#!/usr/bin/python3

## ------------------------------------------------------------------------
##
## SPDX-License-Identifier: LGPL-2.1-or-later
## Copyright (C) 2014 - 2022 by the deal.II authors
##
## This file is part of the deal.II library.
##
## Part of the source code is dual licensed under Apache-2.0 WITH
## LLVM-exception OR LGPL-2.1-or-later. Detailed license information
## governing the source code and code contributions can be found in
## LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
##
## ------------------------------------------------------------------------

# Read through a list of the header files and see if we have any
# cycles in the #include statements. With include guards properly in
# place, it is not a bug to have cycles in header files, but it is
# inefficient and likely unnecessary. Furthermore, it is not allowed
# to have cycles in files that declare module partitions and import
# other module partitions.
#
# Call this script via
#   python3 detect_include_cycles.py include/deal.II/*/*.h
# from the top-level directory.

import sys
import networkx as nx


# For a given header file, read through all the lines and extract the
# ones that correspond to #include statements. For those, add a link
# from header file to the one it includes to the graph.
def add_includes_for_file(header_file_name, G) :
    f = open(header_file_name)
    lines = f.readlines()
    f.close()

    for line in lines :
        if "#include" in line :
            line = line.strip()
            line = line.replace("#include <", "")
            included_file = line.replace(">", "")
            G.add_edge(header_file_name.replace("include/", ""),
                       included_file)



# Take the list of call arguments, excluding the first that contains
# the name of the executable (i.e., this program). For each, add the
# includes as the edges of a directed graph.
G = nx.DiGraph()
for header_file_name in sys.argv[1:] :
    add_includes_for_file(header_file_name, G)

# Then figure out whether there are cycles and if so, print them:
cycles = nx.simple_cycles(G)
cycles_as_list = list(cycles)
if (len(cycles_as_list) > 0) :
    print (f"Cycles in the include graph detected!")
    for cycle in cycles_as_list :
        print(cycle)
    exit(1)
