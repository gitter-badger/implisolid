import unittest

import numpy as np

from basic_functions import check_vector3

from vtk_mc import vtk_mc

import math
import example_objects
import sys

import vector3
# import simple_blend

NUMERICAL_GRADIENT_TOLERANCE = 0.0001  # 0.00001   # 0.001
assert NUMERICAL_GRADIENT_TOLERANCE > 0.0000059
# assert NUMERICAL_GRADIENT_TOLERANCE > 0.00000001


def almost_equal3(a, b, TOLERANCE):
    assert not np.any(np.isnan(a.ravel()))
    assert not np.any(np.isinf(b.ravel()))
    assert not issubclass(a.dtype.type, np.integer)
    check_vector3(a)
    check_vector3(b)
    return np.sum(np.abs(a - b)) < TOLERANCE


class ImplicitFunctionTests(unittest.TestCase):

    def test_centroids_projection(self):

        examples_list = example_objects.get_all_examples([2])
        for example_name in examples_list:
            print("example_name = ", example_name)
            iobj = example_objects.make_example_vectorized(example_name)
            from example_objects import make_example_vectorized
            iobj = make_example_vectorized(example_name)
            self.check_centroids_projection(iobj, objname=example_name)

    def check_centroids_projection(self, iobj, objname=None):
        TOLERANCE = 0.0001
        """Do the centroids projection """
        """Define the rang of the objects"""
        if iobj is not None:
            VERTEX_RELAXATION_ITERATIONS_COUNT = 0
            (RANGE_MIN, RANGE_MAX, STEPSIZE) = (-3, +5, 0.2)
            if objname == "cube_with_cylinders" or objname == "twist_object" or objname == "french_fries" or objname == "rdice_vec" or objname == "rods" or objname == "bowl_15_holes":
                VERTEX_RELAXATION_ITERATIONS_COUNT = 1

            if objname == "cyl4":
                (RANGE_MIN, RANGE_MAX, STEPSIZE) = (-32 / 2, +32 / 2, 1.92 / 4.0)

            elif objname == "french_fries" or objname == "rods":
                (RANGE_MIN, RANGE_MAX, STEPSIZE) = (-3, +5, 0.11)  # 0.05

            elif objname == "bowl_15_holes":
                (RANGE_MIN, RANGE_MAX, STEPSIZE) = (-3, +5, 0.15)

            elif objname == "cyl3":
                (RANGE_MIN, RANGE_MAX, STEPSIZE) = (-32 / 2, +32 / 2, 1.92 / 4.0)

            elif objname == "cyl1":
                (RANGE_MIN, RANGE_MAX, STEPSIZE) = (-16, +32, 1.92 * 0.2 * 10 / 2.0)

            from stl_tests import make_mc_values_grid
            gridvals = make_mc_values_grid(iobj, RANGE_MIN, RANGE_MAX, STEPSIZE, old=False)
            verts, facets = vtk_mc(gridvals, (RANGE_MIN, RANGE_MAX, STEPSIZE))
            print("MC calculated.");sys.stdout.flush()

            from ohtake_belyaev_demo_subdivision_projection_qem import process2_vertex_resampling_relaxation, compute_average_edge_length, set_centers_on_surface__ohtake_v3s

            for i in range(VERTEX_RELAXATION_ITERATIONS_COUNT):
                verts, facets_not_used, centroids = process2_vertex_resampling_relaxation(verts, facets, iobj)
            assert not np.any(np.isnan(verts.ravel()))  # fails
            print("Vertex relaxation applied.");sys.stdout.flush()

            # projection
            average_edge = compute_average_edge_length(verts, facets)

            old_centroids = np.mean(verts[facets[:], :], axis=1)

            new_centroids = old_centroids.copy()

            set_centers_on_surface__ohtake_v3s(iobj, new_centroids, average_edge)

            f = iobj.implicitFunction(new_centroids)
            for i in range(new_centroids.shape[0]):
                self.assertTrue(math.fabs(f[i]) < TOLERANCE)



if __name__ == '__main__':
    unittest.main()