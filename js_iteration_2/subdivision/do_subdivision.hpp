
#include <vector>
#include <map>
#include "../implicit_function/implicit_function.hpp"
using mp5_implicit::implicit_function;

namespace mp5_implicit {
// namespace subdivision {


// based on mp5-private/solidmodeler/implicit/ohtake_belyaev_5.py

void do_subdivision (
    const vectorized_faces & old_faces,
    const vectorized_vect & old_verts,
    const implicit_function& iobj,
    REAL curvature_epsilon,
    REAL randomized_probability = 1.0
) {
    REAL  EXPECTED_SUBDIVISION_PERCENTAGE = 10.0;

    //check NaN in old_verts. Can fail.
    vectrorized_real  curvatures = compute_facets_subdivision_curvatures(old_facets, old_verts, iobj);
    //check NaN in curvatures.
    // Convert NaN into 0.0

    /*
    vectrorized_faceindex  which_facets = find_greater_than(curvatures);

    std::transform(
        curvatures.begin(), curvatures.end(),
        which_facets.begin(), [](REAL curvature) { return curvature > curvature_epsilon;});
    */
    int expected_subdivisions = old_faces.shape()[0] * (EXPECTED_SUBDIVISION_PERCENTAGE) / 100.0;
    std::vector<faceindex_type> which_facets(0);  // zero elements, non-zero capacity
    which_facets.reserve(expected_subdivisions);

    /*
    std::transform(
        curvatures.begin(), curvatures.end(),
        std::back_inserter(name_sizes),
        [](REAL& curvature) { if (curvature > curvature_epsilon) return &curvature-begin });
    */
    /*
    for (faceindex_type fi = 0; fi < old_faces.shape()[0]; ++fi) {
        if (curvature_epsilon[fi] > curvature_epsilon) {
            which_facets.push_back(fi);
        }
    }
    */
    {
        faceindex_type  fi = 0;
        auto it = curvatures.begin(), e = curvatures.end();
        for (; it != e; ++it, ++fi) {
            if (*it > curvature_epsilon) {
                which_facets.push_back(fi);
            }
        }
        assert (fi == old_faces.shape()[0]);
        assert (curvature_epsilon.shape()[0] == old_faces.shape()[0]);
    }

    if (randomized_probability < 1.0) {
        assert(randomized_probability >= 0.0);
        assert(randomized_probability <= 1.0);
        int n0 = which_facets.shape()[0];
        int m0 = static_cast<int>(std::ceil(randomized_probability * (REAL)n0 ));

        // choose_randomly()
        // which_facets = choose_random_subset(which_facets, randomized_probability);
        which_facets = choose_random_subset(which_facets, m0);
    }

    subdivision::midpointmap_type midpoint_map;

    // return std::make_tuple
    // (new_vertices, new_faces, presubdivision_edges);

    auto tuple_ = subdivide_multiple_facets(verts, facets, which_facets_set, midpoint_map)
    // verts2, facets2, presubdivision_edges = tuple;

    vectorized_vect new_vertices = std::get<0>(tuple_);
    vectorized_faces new_faces = std::get<1>(tuple_);
    boost::multi_array<edge_pair_type, 2> =
        presubdivision_edges = std::get<2>(tuple_);
    // todo: presubdivision_edges is not assigned to.

    while (true) {

        // propag_dict, edges_which_in1 = propagated_subdiv(facets2, presubdivision_edges)

        break;
    }
}

}  // namespace mp5_implicit

int main() {

}