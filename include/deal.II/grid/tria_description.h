// ---------------------------------------------------------------------
//
// Copyright (C) 2019 - 2023 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------

#ifndef dealii_grid_construction_utilities_h
#define dealii_grid_construction_utilities_h

#include <deal.II/base/config.h>

#include <deal.II/base/memory_space.h>
#include <deal.II/base/mpi_stub.h>

#include <deal.II/grid/cell_id.h>
#include <deal.II/grid/reference_cell.h>
#include <deal.II/grid/tria.h>


DEAL_II_NAMESPACE_OPEN
#ifndef DOXYGEN
namespace LinearAlgebra
{
  namespace distributed
  {
    template <typename Number, typename MemorySpace>
    class Vector;
  }
} // namespace LinearAlgebra
#endif

/*------------------------------------------------------------------------*/

/**
 * The CellData class (and the related SubCellData class) is used to
 * provide a comprehensive, but minimal, description of the cells when
 * creating a triangulation via Triangulation::create_triangulation().
 * Specifically, each CellData object -- describing one cell in a
 * triangulation -- has member variables for indices of the $2^d$ vertices
 * (the actual coordinates of the vertices are described in a separate
 * vector passed to Triangulation::create_triangulation(), so the CellData
 * object only needs to store indices into that vector), the material
 * id of the cell that can be used in applications to describe which
 * part of the domain a cell belongs to (see
 * @ref GlossMaterialId "the glossary entry on material ids"),
 * and a manifold id that is used to describe the geometry object
 * that is responsible for this cell (see
 * @ref GlossManifoldIndicator "the glossary entry on manifold ids")
 * to describe the manifold this object belongs to.
 *
 * This structure is also used to represent data for faces and edges when used
 * as a member of the SubCellData class. In this case, the template argument
 * @p structdim of an object will be less than the dimension @p dim of the
 * triangulation. If this is so, then #vertices array represents the indices of
 * the vertices of one face or edge of one of the cells passed to
 * Triangulation::create_triangulation(). Furthermore, for faces the
 * material id has no meaning, and the @p material_id field is reused
 * to store a @p boundary_id instead to designate which part of the boundary
 * the face or edge belongs to (see
 * @ref GlossBoundaryIndicator "the glossary entry on boundary ids").
 *
 * An example showing how this class can be used is in the
 * <code>create_coarse_grid()</code> function of step-14. There are also
 * many more use cases in the implementation of the functions of the
 * GridGenerator namespace.
 *
 * @ingroup grid
 */
template <int structdim>
struct CellData
{
  /**
   * Indices of the vertices of this cell. These indices correspond
   * to entries in the vector of vertex locations passed to
   * Triangulation::create_triangulation().
   *
   * By default, the constructor of this class initializes this variable to
   * have as many entries as it takes to describe a hypercube cell (i.e., a
   * ReferenceCells::Line, ReferenceCells::Quadrilateral, or
   * ReferenceCells::Hexahedron). This is historical and dates back to the time
   * where deal.II could only deal with these kinds of cells. If you want an
   * object of the current type to describe, for example, a triangle or
   * tetrahedron, then you either have to call this constructor with an explicit
   * argument different from the default value, or manually resize the
   * `vertices` member variable after construction.
   *
   * The kind of cell described by the current object is then determined by
   * calling ReferenceCell::n_vertices_to_type() on the number of vertices
   * described by this array.
   */
  std::vector<unsigned int> vertices;

  /**
   * Material or boundary indicator of this cell.
   * This field is a union that stores <i>either</i> a boundary or
   * a material id, depending on whether the current object is used
   * to describe a cell (in a vector of CellData objects) or a
   * face or edge (as part of a SubCellData object).
   */
  union
  {
    /**
     * The material id of the cell being described. See the documentation
     * of the CellData class for examples of how to use this field.
     *
     * This variable can only be used if the current object is used to
     * describe a cell, i.e., if @p structdim equals the dimension
     * @p dim of a triangulation.
     */
    types::material_id material_id;

    /**
     * The boundary id of a face or edge being described. See the documentation
     * of the CellData class for examples of how to use this field.
     *
     * This variable can only be used if the current object is used to
     * describe a face or edge, i.e., if @p structdim is less than the dimension
     * @p dim of a triangulation. In this case, the CellData object this
     * variable belongs to will be part of a SubCellData object.
     */
    types::boundary_id boundary_id;
  };

  /**
   * Manifold identifier of this object. This identifier should be used to
   * identify the manifold to which this object belongs, and from which this
   * object will collect information on how to add points upon refinement.
   *
   * See the documentation of the CellData class for examples of how to use
   * this field.
   */
  types::manifold_id manifold_id;

  /**
   * Default constructor. Sets the member variables to the following values:
   *
   * - vertex indices to invalid values
   * - boundary or material id zero (the default for boundary or material ids)
   * - manifold id to numbers::flat_manifold_id.
   *
   * By default, the constructor initializes the `vertices` member variable to
   * have as many entries as it takes to describe a hypercube cell (i.e., a
   * ReferenceCells::Line, ReferenceCells::Quadrilateral, or
   * ReferenceCells::Hexahedron). This is historical and dates back to the time
   * where deal.II could only deal with these kinds of cells. If you want an
   * object of the current type to describe, for example, a triangle or
   * tetrahedron, then you either have to call this constructor with an explicit
   * argument different from the default value, or manually resize the
   * `vertices` member variable after construction.
   */
  CellData(const unsigned int n_vertices =
             ReferenceCells::get_hypercube<structdim>().n_vertices());

  /**
   * Comparison operator.
   */
  bool
  operator==(const CellData<structdim> &other) const;

  /**
   * Read or write the data of this object to or from a stream for the
   * purpose of serialization using the [BOOST serialization
   * library](https://www.boost.org/doc/libs/1_74_0/libs/serialization/doc/index.html).
   */
  template <class Archive>
  void
  serialize(Archive &ar, const unsigned int version);

  static_assert(structdim > 0,
                "The class CellData can only be used for structdim>0.");
};



/**
 * The SubCellData class is used to describe information about faces and
 * edges at the boundary of a mesh when creating a triangulation via
 * Triangulation::create_triangulation(). It contains member variables
 * that describe boundary edges and boundary quads.
 *
 * The class has no template argument and is used both in the description
 * of boundary edges in 2d (in which case the contents of the
 * @p boundary_quads member variable are ignored), as well as in the
 * description of boundary edges and faces in 3d (in which case both the
 * @p boundary_lines and @p boundary_quads members may be used). It is also
 * used as the argument to Triangulation::create_triangulation() in 1d,
 * where the contents of objects of the current type are simply ignored.
 *
 * By default, Triangulation::create_triangulation() simply assigns
 * default boundary indicators and manifold indicators to edges and
 * quads at the boundary of the mesh. (See the glossary entries on
 * @ref GlossBoundaryIndicator "boundary ids"
 * and
 * @ref GlossManifoldIndicator "manifold ids"
 * for more information on what they represent.) As a consequence,
 * it is not <i>necessary</i> to explicitly describe the properties
 * of boundary objects. In all cases, these properties can also be
 * set at a later time, once the triangulation has already been
 * created. On the other hand, it is sometimes convenient to describe
 * boundary indicators or manifold ids at the time of creation. In
 * these cases, the current class can be used by filling the
 * @p boundary_lines and @p boundary_quads vectors with
 * CellData<1> and CellData<2> objects that correspond to boundary
 * edges and quads for which properties other than the default
 * values should be used.
 *
 * Each entry in the @p boundary_lines and @p boundary_quads vectors
 * then needs to correspond to an edge or quad of the cells that
 * are described by the vector of CellData objects passed to
 * Triangulation::create_triangulation(). I.e., the vertex indices
 * stored in each entry need to correspond to an edge or face
 * of the triangulation that has the same set of vertex indices,
 * and in the same order. For these boundary edges or quads, one can
 * then set either or both the CellData::boundary_id and
 * CellData::manifold_id.
 *
 * There are also use cases where one may want to set the manifold id
 * of an <i>interior</i> edge or face. Such faces, identified by
 * their vertex indices, may also appear in the
 * @p boundary_lines and @p boundary_quads vectors (despite the names of
 * these member variables). However, it is then obviously not allowed
 * to set a boundary id (because the object is not actually part of
 * the boundary). As a consequence, to be valid, the CellData::boundary_id
 * of interior edges or faces needs to equal
 * numbers::internal_face_boundary_id.
 *
 * @ingroup grid
 */
struct SubCellData
{
  /**
   * A vector of CellData<1> objects that describe boundary and manifold
   * information for edges of 2d or 3d triangulations. For 2d triangulations,
   * edges (lines) are of course the faces of the cells.
   *
   * This vector must not be used in the creation of 1d triangulations.
   */
  std::vector<CellData<1>> boundary_lines;

  /**
   * A vector of CellData<2> objects that describe boundary and manifold
   * information for triangles and quads of 3d triangulations. The name of
   * the variable is historical and dates back to a time when deal.II only
   * supported hexahedral meshes in 3d, where then all boundary faces were
   * necessarily quadrilaterals. However, the variable is now also used to
   * describe boundary triangles for tetrahedral and mixed meshes.
   *
   * Whether an element in this array describes a boundary triangle or a
   * boundary quad is determined by how many elements the `vertices`
   * member variable of `CellData<2>` stores.
   *
   * This vector must not be used in the creation of 1d or 2d triangulations.
   */
  std::vector<CellData<2>> boundary_quads;

  /**
   * Determine whether the member variables above which may not be used in a
   * given dimension are really empty. In other words, this function returns
   * whether
   * both @p boundary_lines and @p boundary_quads are empty vectors
   * when @p dim equals one, and whether the @p boundary_quads
   * vector is empty when @p dim equals two.
   */
  bool
  check_consistency(const unsigned int dim) const;
};


template <int structdim>
template <class Archive>
void
CellData<structdim>::serialize(Archive &ar, const unsigned int /*version*/)
{
  ar &vertices;
  ar &material_id;
  ar &boundary_id;
  ar &manifold_id;
}

/**
 * A namespace dedicated to the struct Description, which can be used in
 * one of the overloads of Triangulation::create_triangulation(). All
 * triangulations in deal.II are, in one way or the other, created by
 * calling Triangulation::create_triangulation(); for example, all of the
 * functions in class GridIn and namespace GridGenerator call these
 * functions. Most of these call the overload that takes a vector of
 * vertices plus a vector of CellData objects that describe which vertices
 * form each cell (along with some other information). But there are other
 * overloads of Triangulation::create_triangulation() that require more
 * elaborate descriptions of triangulations, and these typically use an
 * object of type TriangulationDescription::Description declared in this
 * namespace.
 */
namespace TriangulationDescription
{
  /**
   * Configuration flags for Triangulation objects.
   * Settings can be combined using bitwise OR.
   */
  enum Settings
  {
    /**
     * Default settings, other options are disabled.
     */
    default_setting = 0x0,
    /**
     * This flag needs to be set to use the geometric multigrid
     * functionality. This option requires additional computation and
     * communication.
     */
    construct_multigrid_hierarchy = 0x1
  };

  /**
   * This class stores information needed for creating a locally relevant
   * cell when calling the overload of Triangulation::create_triangulation()
   * that takes an object of type TriangulationDescription::Description.
   * Objects of the current type are stored in Description. In contrast to
   * the dealii::CellData class, the current class also stores information
   * relevant for the parallel partitioning of a triangulation, such as
   * a global cell id, the subdomain_id, and the level_subdomain_id as well as
   * information related to manifold_id and boundary_id.
   *
   * In contrast to dealii::CellData, it does not store geometric information
   * such as vertex locations. This information is only needed on the coarsest
   * level of a triangulation (whereas the current structure is also used for
   * refined levels of a triangulation) and is stored separately by
   * the Description class.
   */
  template <int dim>
  struct CellData
  {
    /**
     * Constructor
     */
    CellData();

    /**
     * Read or write the data of this object to or from a stream for the
     * purpose of serialization using the [BOOST serialization
     * library](https://www.boost.org/doc/libs/1_74_0/libs/serialization/doc/index.html).
     */
    template <class Archive>
    void
    serialize(Archive &ar, const unsigned int /*version*/);

    /**
     * Comparison operator.
     */
    bool
    operator==(const CellData<dim> &other) const;

    /**
     * Unique CellID of the cell.
     */
    CellId::binary_type id;

    /**
     * subdomain_id of the cell.
     */
    types::subdomain_id subdomain_id;

    /**
     * level_subdomain_id of the cell.
     */
    types::subdomain_id level_subdomain_id;

    /**
     * Manifold id of the cell.
     */
    types::manifold_id manifold_id;

    /**
     * Manifold id of all lines of the cell.
     *
     * @note Only used for 2d and 3d.
     */
    std::array<types::manifold_id, GeometryInfo<dim>::lines_per_cell>
      manifold_line_ids;

    /**
     * Manifold id of all face quads of the cell.
     *
     * @note Only used for 3d.
     */
    std::array<types::manifold_id,
               dim == 1 ? 1 : GeometryInfo<3>::quads_per_cell>
      manifold_quad_ids;

    /**
     * List of face number and boundary id of all non-internal faces of the
     * cell.
     */
    std::vector<std::pair<unsigned int, types::boundary_id>> boundary_ids;
  };


  /**
   * Data used in the Triangulation::create_triangulation() overloads that
   * builds a triangulation out of objects of the current type. The contents
   * of this kind of object are typically creates by functions such as
   * TriangulationDescription::Utilities::create_description_from_triangulation().
   */
  template <int dim, int spacedim = dim>
  struct Description
  {
    /**
     * Constructor.
     */
    Description();

    /**
     * Read or write the data of this object to or from a stream for the
     * purpose of serialization using the [BOOST serialization
     * library](https://www.boost.org/doc/libs/1_74_0/libs/serialization/doc/index.html).
     */
    template <class Archive>
    void
    serialize(Archive &ar, const unsigned int /*version*/);

    /**
     * Comparison operator.
     */
    bool
    operator==(const Description<dim, spacedim> &other) const;

    /**
     * Cells of the locally-relevant coarse-grid triangulation.
     */
    std::vector<dealii::CellData<dim>> coarse_cells;

    /**
     * Vertices of the locally-relevant coarse-grid triangulation.
     */
    std::vector<Point<spacedim>> coarse_cell_vertices;

    /**
     * List that for each locally-relevant coarse cell provides the
     * corresponding global
     * @ref GlossCoarseCellId.
     */
    std::vector<types::coarse_cell_id> coarse_cell_index_to_coarse_cell_id;

    /**
     * CellData for each locally relevant cell on each level. cell_infos[i]
     * contains the CellData for each locally relevant cell on the ith
     * level.
     */
    std::vector<std::vector<CellData<dim>>> cell_infos;

    /**
     * The MPI communicator used to create this struct. It will be compared
     * to the communicator inside of the Triangulation
     * and an assert is thrown if they do not match.
     *
     * @note Please note this is necessary since the communicator inside of
     * parallel::TriangulationBase is const and cannot be changed after the
     * constructor has been called.
     */
    MPI_Comm comm;

    /**
     * Properties to be use in the construction of the triangulation.
     */
    Settings settings;

    /**
     * Mesh smoothing type.
     */
    typename Triangulation<dim, spacedim>::MeshSmoothing smoothing;
  };


  /**
   * A namespace for TriangulationDescription::Description utility functions.
   *
   * @ingroup TriangulationDescription
   */
  namespace Utilities
  {
    /**
     * Construct a TriangulationDescription::Description object that for the
     * current process, using a given triangulation `tria` describes how
     * one can re-create a partitioned triangulation (such as a
     * parallel::fullydistributed::Triangulation).
     * The input triangulation can be either
     * a serial triangulation of type dealii::Triangulation which has been
     * colored (i.e., for which the subdomain_id and/or level_subdomain_id have
     * been set to create partitions), or a distributed triangulation of type
     * dealii::parallel::distributed::Triangulation, where the output of
     * this function takes over the existing partitioning unaltered.
     *
     * An example for a serial Triangulation looks like this:
     * @code
     * Triangulation<dim, spacedim> tria_base;
     *
     * // Create a serial triangulation (here by reading an external mesh):
     * GridIn<dim, spacedim> grid_in;
     * grid_in.attach_triangulation(tria_base);
     * grid_in.read(file_name);
     *
     * // Partition serial triangulation:
     * GridTools::partition_triangulation(
     *   Utilities::MPI::n_mpi_processes(comm), tria_base);
     *
     * // Create building blocks:
     * const TriangulationDescription::Description<dim, spacedim> description =
     *   TriangulationDescription::Utilities::
     *     create_description_from_triangulation(tria_base, comm);
     *
     * // Create a fully distributed triangulation:
     * parallel::fullydistributed::Triangulation<dim, spacedim> tria_pft(comm);
     * tria_pft.create_triangulation(description);
     * @endcode
     * In this example, *all* processes in a parallel universe read the same
     * mesh, and partition it in the same way based on the subdomain ids set
     * by GridTools::partition_triangulation(). Then, on each process,
     * the current function creates building blocks that extract information
     * about those cells whose subdomain ids match the current process's
     * rank (plus perhaps other necessary information), and in a last step,
     * we create a fully distributed triangulation out of these building blocks.
     *
     * In contrast, when starting with a parallel::distributed::Triangulation
     * object, partitioning can be skipped, since the triangulation has already
     * been partitioned and the corresponding code looks as follows:
     * @code
     * parallel::distributed::Triangulation<dim, spacedim> tria_base(comm);
     *
     * // Create a triangulation (not shown, see for example step-40)
     * ...
     *
     * // Create building blocks:
     * const TriangulationDescription::Description<dim, spacedim> description =
     *   TriangulationDescription::Utilities::
     *     create_description_from_triangulation(tria_base, comm);
     *
     * // Create a fully distributed triangulation:
     * parallel::fullydistributed::Triangulation<dim, spacedim> tria_pft(comm);
     * tria_pft.create_triangulation(description);
     * @endcode
     * This code therefore takes a dealii::parallel::distributed::Triangulation
     * object (such as the ones used, for example, in step-40 and many other
     * tutorial programs) and converts it into a
     * dealii::parallel::fullydistributed::Triangulation.
     *
     * @param[in] tria The input triangulation. This function uses the
     * partitioning provided by either explicitly setting subdomain_ids (if the
     * triangulation is sequential) or based on how the existing triangulation
     * is already distributed to different processes.
     * @param[in] comm The MPI communicator to be used. If the input
     * triangulation is parallel, then the communicators have to match.
     * @param[in] settings See the description of the Settings enumerator.
     * @param[in] my_rank_in Construct Description for the specified rank. This
     *   parameter can only be set for serial input triangulations that have
     * been partitioned by functions like GridTools::partition_triangulation().
     * For parallel input triangulations, `my_rank_in` needs to equal to the
     * default value, or to the rank of the current process within the given
     *   communicator.
     * @return The Description object that can then be used to set up a
     *   dealii::parallel::fullydistributed::Triangulation.
     *
     * @note If `construct_multigrid_hierarchy` is set in the @p settings, then
     *   the input triangulation has to be set up with
     *   `limit_level_difference_at_vertices` among the triangulation's
     * smoothing flags provided to the triangulation constructor.
     */
    template <int dim, int spacedim = dim>
    Description<dim, spacedim>
    create_description_from_triangulation(
      const dealii::Triangulation<dim, spacedim> &tria,
      const MPI_Comm                              comm,
      const TriangulationDescription::Settings    settings =
        TriangulationDescription::Settings::default_setting,
      const unsigned int my_rank_in = numbers::invalid_unsigned_int);

    /**
     * Similar to the above function but the desired owners of active cells are
     * not provided via the subdomain_ids of cells, but rather by the
     * corresponding values of a cell vector (see also
     * parallel::TriangulationBase::global_active_cell_index_partitioner() and
     * CellAccessor::global_active_cell_index()). This function allows
     * repartitioning Triangulation objects that are already distributed and
     * that may be partitioned differently than the desired partitioning.
     *
     * If the setup of multigrid levels is requested by the @p settings argument,
     * they are partitioned according to a first-child policy. In other words,
     * a process owns a (non-active) cell if it owns its first child. If that
     * first child is not active itself, the policy is applied recursively.
     *
     * The partitioning of cells is determined based on the elements of the
     * `partition` vector. While that vector stores elements of type `double`,
     * actual values must be integers and be within the range of process
     * ranks of the relevant communicator.
     *
     * @note The communicator is extracted from the vector @p partition.
     *
     * @note The triangulation @p tria can be set up on a subcommunicator of the
     *   communicator of @p partitioner. All processes that are not part of that
     *   subcommunicator need to set up the local triangulation with the
     *   special-purpose communicator MPI_COMM_NULL.
     *
     * @note The multigrid levels are currently not constructed, since
     *   @p partition only describes the partitioning of the active level.
     */
    template <int dim, int spacedim>
    Description<dim, spacedim>
    create_description_from_triangulation(
      const Triangulation<dim, spacedim> &tria,
      const LinearAlgebra::distributed::Vector<double, MemorySpace::Host>
                                              &partition,
      const TriangulationDescription::Settings settings =
        TriangulationDescription::Settings::default_setting);

    /**
     * Similar to the above function but allowing the user to prescribe the
     * partitioning of the multigrid levels. As with the other function,
     * while both the global and level-wise partition vectors store elements
     * with data type `double`, actual elements must be integers and equal
     * possible process ranks within the relevant communicator.
     */
    template <int dim, int spacedim>
    Description<dim, spacedim>
    create_description_from_triangulation(
      const Triangulation<dim, spacedim> &tria,
      const LinearAlgebra::distributed::Vector<double, MemorySpace::Host>
        &partition,
      const std::vector<
        LinearAlgebra::distributed::Vector<double, MemorySpace::Host>>
                                              &mg_partitions,
      const TriangulationDescription::Settings settings =
        TriangulationDescription::Settings::default_setting);

    /**
     * Construct a TriangulationDescription::Description. In contrast
     * to the function above, this function is also responsible for creating
     * a serial triangulation and for its partitioning (by calling the
     * provided `std::function` objects). Internally only selected processes (
     * every n-th/each root of a group of size group_size) create a serial
     * triangulation and the TriangulationDescription::Description for all
     * processes in its group, which is communicated.
     *
     * This function can also be used to read an external mesh only once (by
     * the root process and a group consisting all processes). The following
     * code snippet shows a modified version of the example provided in the
     * documentation of create_description_from_triangulation().
     * Function calls that only need to be performed by the root process
     * (read and partition mesh) have been moved into `std::function` objects.
     *
     * @code
     * // create and partition serial triangulation and create description
     * const TriangulationDescription::Description<dim, spacedim> description =
     *   TriangulationDescription::Utilities::
     *     create_description_from_triangulation_in_groups<dim, spacedim>(
     *       [file_name](auto &tria_base) {
     *         GridIn<dim, spacedim> grid_in;
     *         grid_in.attach_triangulation(tria_base);
     *         grid_in.read(file_name);
     *       },
     *       [](auto &tria_base, const auto comm, const auto group_size) {
     *         GridTools::partition_triangulation(
     *           Utilities::MPI::n_mpi_processes(comm), tria_base);
     *       },
     *       comm,
     *       Utilities::MPI::n_mpi_processes(comm));
     *
     * // create triangulation
     * parallel::fullydistributed::Triangulation<dim, spacedim> tria_pft(comm);
     * tria_pft.create_triangulation(description);
     * @endcode
     *
     * @note A reasonable group size is the size of a NUMA domain or the
     * size of a compute node.
     *
     * @param serial_grid_generator A function which creates a serial triangulation.
     * @param serial_grid_partitioner A function which can partition a serial
     *   triangulation, i.e., sets the sudomain_ids of the active cells.
     *   The function takes as the first argument a serial triangulation,
     *   as the second argument the MPI communicator, and as the third
     *   argument the group size.
     * @param comm MPI communicator.
     * @param group_size The size of each group.
     * @param smoothing Mesh smoothing type.
     * @param setting See the description of the Settings enumerator.
     * @return Description to be used to set up a Triangulation.
     *
     * @note If construct_multigrid_hierarchy is set in the settings, the
     *   @p smoothing parameter is extended with the
     *   limit_level_difference_at_vertices flag.
     */
    template <int dim, int spacedim = dim>
    Description<dim, spacedim>
    create_description_from_triangulation_in_groups(
      const std::function<void(dealii::Triangulation<dim, spacedim> &)>
                                                    &serial_grid_generator,
      const std::function<void(dealii::Triangulation<dim, spacedim> &,
                               const MPI_Comm,
                               const unsigned int)> &serial_grid_partitioner,
      const MPI_Comm                                 comm,
      const int                                      group_size = 1,
      const typename Triangulation<dim, spacedim>::MeshSmoothing smoothing =
        dealii::Triangulation<dim, spacedim>::none,
      const TriangulationDescription::Settings setting =
        TriangulationDescription::Settings::default_setting);

  } // namespace Utilities



  template <int dim>
  CellData<dim>::CellData()
    : subdomain_id(numbers::invalid_subdomain_id)
    , level_subdomain_id(numbers::invalid_subdomain_id)
    , manifold_id(numbers::flat_manifold_id)
  {
    std::fill(id.begin(), id.end(), numbers::invalid_unsigned_int);
    std::fill(manifold_line_ids.begin(),
              manifold_line_ids.end(),
              numbers::flat_manifold_id);
    std::fill(manifold_quad_ids.begin(),
              manifold_quad_ids.end(),
              numbers::flat_manifold_id);
  }



  template <int dim>
  template <class Archive>
  void
  CellData<dim>::serialize(Archive &ar, const unsigned int /*version*/)
  {
    ar &id;
    ar &subdomain_id;
    ar &level_subdomain_id;
    ar &manifold_id;
    if (dim >= 2)
      ar &manifold_line_ids;
    if (dim >= 3)
      ar &manifold_quad_ids;
    ar &boundary_ids;
  }



  template <int dim, int spacedim>
  Description<dim, spacedim>::Description()
    : comm(MPI_COMM_NULL)
    , settings(Settings::default_setting)
    , smoothing(Triangulation<dim, spacedim>::MeshSmoothing::none)
  {}



  template <int dim, int spacedim>
  template <class Archive>
  void
  Description<dim, spacedim>::serialize(Archive &ar,
                                        const unsigned int /*version*/)
  {
    ar &coarse_cells;
    ar &coarse_cell_vertices;
    ar &coarse_cell_index_to_coarse_cell_id;
    ar &cell_infos;
    ar &settings;
    ar &smoothing;
  }



  template <int dim>
  bool
  CellData<dim>::operator==(const CellData<dim> &other) const
  {
    if (this->id != other.id)
      return false;
    if (this->subdomain_id != other.subdomain_id)
      return false;
    if (this->level_subdomain_id != other.level_subdomain_id)
      return false;
    if (this->manifold_id != other.manifold_id)
      return false;
    if (dim >= 2 && this->manifold_line_ids != other.manifold_line_ids)
      return false;
    if (dim >= 3 && this->manifold_quad_ids != other.manifold_quad_ids)
      return false;
    if (this->boundary_ids != other.boundary_ids)
      return false;

    return true;
  }



  template <int dim, int spacedim>
  bool
  Description<dim, spacedim>::operator==(
    const Description<dim, spacedim> &other) const
  {
    if (this->coarse_cells != other.coarse_cells)
      return false;
    if (this->coarse_cell_vertices != other.coarse_cell_vertices)
      return false;
    if (this->coarse_cell_index_to_coarse_cell_id !=
        other.coarse_cell_index_to_coarse_cell_id)
      return false;
    if (this->cell_infos != other.cell_infos)
      return false;
    if (this->settings != other.settings)
      return false;
    if (this->smoothing != other.smoothing)
      return false;

    return true;
  }
} // namespace TriangulationDescription


DEAL_II_NAMESPACE_CLOSE

#endif
