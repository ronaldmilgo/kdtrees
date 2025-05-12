#include <stdlib.h>
#include "kdtree.h"
#include "kdtree_helpers.h"


kdtree_link_info kdtree_find_extreme(kdtree_node *r, int r_dim, kdtree_node **ptr_to_r, int dim, int factor)
{
  kdtree_link_info info;
  
  if (r->left == NULL && r->right == NULL)
    {
      // node is a leaf, so it has the most extreme value in its subtree
      info.n = r;
      info.ptr_to_n = ptr_to_r;
      info.n_dim = r_dim;
    }
  else
    {
      // initialize extreme to value in root of subtree
      info.n = r;
      info.ptr_to_n = ptr_to_r;
      info.n_dim = r_dim;
      location extreme = r->loc;

      // compare extreme to extreme from left subtree
      if (r->left != NULL)
	{
	  kdtree_link_info left_info = kdtree_find_extreme(r->left, 1 - r_dim, &r->left, dim, factor);

	  // if cutting dimension is 0, compare by longitude, otherwise
	  // by latitude; note that determining which of two values a and b
	  // is the minimum is equivalent to determining the max of -a and -b
	  // (this is how factor is used)
	  if ((dim == 0 && factor * location_compare_longitude(&left_info.n->loc, &extreme) > 0)
	      || (dim == 1 && factor * location_compare_latitude(&left_info.n->loc, &extreme) > 0))
	    {
	      info = left_info;
	      extreme = info.n->loc;
	    }
	}

      // compare extreme to extreme from right subtree
      if (r->right != NULL)
	{
	  kdtree_link_info right_info = kdtree_find_extreme(r->right, 1 - r_dim, &r->right, dim, factor);

	  if ((dim == 0 && factor * location_compare_longitude(&right_info.n->loc, &extreme) > 0)
	      || (dim == 1 && factor * location_compare_latitude(&right_info.n->loc, &extreme) > 0))
	    {
	      info = right_info;
	      extreme = info.n->loc;
	    }
	}
    }

  return info;
}
