import pymeshlab
ms = pymeshlab.MeshSet()

ms.load_new_mesh('airplane.obj')
ms.apply_filter('convex_hull')
ms.save_current_mesh('convex_hull.ply')

ms.print_filter_list()
ms.print_filter_parameter_list('surface_reconstruction_screened_poisson')