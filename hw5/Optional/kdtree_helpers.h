#ifndef __KDTREE_HELPERS_H__
#define __KDTREE_HELPERS_H__
#include "kdtree_internal.h"

/**
 * Note that this assumes there is a kdtree_node struct defined with
 * members left, right, and loc.  You can include this file from your
 * implementation file *after* you declare the kdtree_node type.  You
 * may still need to rename and reorganize in order to work with your
 * implementation.  Note that the functions declared here expect
 * dimension 0 to be longitude and dimension 1 to be latitude (the
 * opposite of how they're normally listed, but matching longitude as
 * analagous to x-coordinate and latitude as y-coordinate.a
 */


/**
 * Information about a link to a node in a tree.
 */
typedef struct
{
  kdtree_node *n; // the node
  kdtree_node **ptr_to_n; // a pointer to the pointer to the node
  int n_dim; // the cutting dimention of n (0=lon, 1=lat)
} kdtree_link_info;


/**
 * Finds the node with the extremest (max or min) value in the
 * given dimension in the subtree rooted at the given node.
 *
 * @param r a pointer to the root of the subtree to search, non-NULL
 * @param r_dim the cutting dimension of that root, 0 for x/lon and 1 for y/lat
 * @param ptr_to_r a pointer to the pointer to r
 * @param dim the dimension to find the extreme in: 0 for x/lon, 1 for y/lat
 * @param factor 1 to find max, -1 to find min
 * @return a kdtree_link_info struct for the node in the subtree rooted at r
 * that contains the most extreme coordinate
 */
kdtree_link_info kdtree_find_extreme(kdtree_node *r, int r_dim, kdtree_node **ptr_to_r, int dim, int factor);


#endif
