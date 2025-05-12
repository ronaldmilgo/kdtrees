#include <stdio.h>
#include <stdlib.h>
#include "kdtree.h"
#include "location.h"
#include "kdtree_helpers.h"
#include "kdtree_internal.h"


typedef struct _kdtree{
    kdtree_node *root;
    size_t tree_size;
} kdtree;

//copied it over from helpers.c
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



kdtree_node *kdtree_create_helper(location *ptr, int n, int depth){
    if (n <= 0){
        return NULL;
    }

    //  determine the cut dimension of the 
    int cut_dimension = depth % 2;

    if (cut_dimension == 0){//lon
        qsort(ptr, n, sizeof(location), (int (*)(const void *, const void *)) location_compare_longitude);
    } else{
        qsort(ptr, n, sizeof(location), (int (*)(const void *, const void *)) location_compare_latitude);
    }

    int median = n/2;

    //create a new node at the median point(to be root of the subtree)
    kdtree_node *node = malloc(sizeof(kdtree_node));
    if (node == NULL){
        return NULL;
    }
    node->loc = ptr[median];
    node->cut_dim = cut_dimension;
    //call the function recursively
    node->left = kdtree_create_helper(ptr, median, depth + 1);
    node->right = kdtree_create_helper(ptr + median + 1, n - (median + 1), depth + 1);

    return node;
}

kdtree *kdtree_create(const location *pts, int n){
    kdtree *tree = malloc(sizeof(kdtree));
    if (tree == NULL){
        return NULL;
    }

    tree->tree_size = n;
    tree->root = NULL;

    if (n > 0){
        //copy points into a mutable array that we can sort
        location *pts_cpy = malloc(sizeof(location) * n);
        if (pts_cpy == NULL){
            free(tree);
            return NULL;
        }

        for (size_t i = 0; i < n; i++){
            pts_cpy[i] = pts[i];
        }

        //always start from depth 0
        tree->root = kdtree_create_helper(pts_cpy, n, 0);

        free(pts_cpy);
    }
    return tree;
}

bool kdtree_contains(const kdtree *t, const location *p){
    if (t == NULL || p == NULL){
        return false;
    }

    kdtree_node *curr_node = t->root;
    int depth = 0;

    while (curr_node != NULL){
        if(curr_node->loc.lon == p->lon && curr_node->loc.lat == p->lat){
            return true;
        }

        int cut_dim = depth % 2;

        //traverse the tree right or left
        if((cut_dim == 0 && p->lon < curr_node->loc.lon) || (cut_dim == 1 && p->lat < curr_node->loc.lat)){
            curr_node = curr_node->left;
        }else{
            curr_node = curr_node->right;
        }
        depth++;
    }
    return false;
}

kdtree_node *kdtree_add_helper(kdtree_node *node, const location *pt, int depth){
    if (node == NULL){
        //create one and populate
        kdtree_node *new_node = malloc(sizeof(kdtree_node));
        //by dereferencing we are creating a copy
        new_node->loc = *pt;
        new_node->cut_dim = depth % 2;
        new_node->left = NULL;
        new_node->right = NULL;

        // printf("Point ADDED: %lf - %lf   %d\n", pt->lat, pt->lon, new_node->cut_dim);
        return new_node;
    }

    int cut_dime = depth % 2;
    if((cut_dime == 0 && pt->lon < node->loc.lon) || (cut_dime == 1 && pt->lat < node->loc.lat)){
            node->left = kdtree_add_helper(node->left, pt, depth + 1);
    }else{
        node->right = kdtree_add_helper(node->right, pt, depth + 1);
    }
    return node;
}

bool kdtree_add(kdtree *t, const location *p){
    if (t == NULL || p == NULL){
        return false;
    }
    //check if point is already in the tree
    if (kdtree_contains(t, p)){
        return false;
    }

    int cut_dim_of_root = t->root == NULL ? 0 : t->root->cut_dim;
    t->root = kdtree_add_helper(t->root, p, cut_dim_of_root);
    t->tree_size++;
    return true;
}

//Helper function
kdtree_node *kdtree_remove_helper(kdtree_node *node, const location *p){
    if(node == NULL || p == NULL){
        return NULL;
    }
    //base case(if we find the point to delete)
    if(node->loc.lon == p->lon && node->loc.lat == p->lat){
        //case1: if node has no children
        if(node->left == NULL && node->right == NULL){
            // printf("Point REMOVED: %lf - %lf    %d\n", p->lat, p->lon, node->cut_dim);
            free(node);
            return NULL;
        }
        //case 2: if node has 1 child
        if (node->left == NULL){
            // printf("Point REMOVED: %lf - %lf    %d\n", p->lat, p->lon, node->cut_dim);
            int cut_dim = node->cut_dim;
            kdtree_link_info min_node = kdtree_find_extreme(node->right, 1 - cut_dim, &node->right, cut_dim, -1);
            node->loc = min_node.n->loc;
            node->right = kdtree_remove_helper(node->right, &min_node.n->loc);
            return node;
        }
        if (node->right == NULL){
            // printf("Point REMOVED: %lf - %lf    %d\n", p->lat, p->lon, node->cut_dim);
            int cut_dim = node->cut_dim;
            kdtree_link_info max_node = kdtree_find_extreme(node->left, 1 - cut_dim, &node->right, cut_dim, 1);
            node->loc = max_node.n->loc;
            node->right = kdtree_remove_helper(node->right, &max_node.n->loc);
            return node;
        }
        //case 3: two children(we are replacing the deleted node with the minimum node in the right subtree)
        
        int cut_dim = node->cut_dim;
        //find min node in the right subtree
        kdtree_link_info min_node = kdtree_find_extreme(node->right, 1 - cut_dim, &node->right, cut_dim, -1);

        //copy over the min node to replace deleted node
        node->loc = min_node.n->loc;
        //remove the copied over node from the tree
        node->right = kdtree_remove_helper(node->right, &min_node.n->loc);

        return node;
    }

    //traverse tree
    int cut_dim = node->cut_dim;
    //move left
    if((cut_dim == 0 && p->lon < node->loc.lon) || (cut_dim == 1 && p->lat < node->loc.lat)){
        node->left = kdtree_remove_helper(node->left, p);
    }else{
        node->right = kdtree_remove_helper(node->right, p);
    }
    return node;
}

void kdtree_remove(kdtree *t, const location *p){
    if(t == NULL || p == NULL){
        return;
    }

    
    t->root = kdtree_remove_helper(t->root, p);
    if (t->root != NULL){
        t->tree_size--;
    }
}

//Helpr function
void kdtree_range_helper(kdtree_node *node, const location *sw, const location *ne, location **loc_points, size_t *index, size_t *capacity, int depth){
    if (node == NULL){
        return;
    }

    //check if node is within range
    if(sw->lon <= node->loc.lon && ne->lon >= node->loc.lon && sw->lat <= node->loc.lat && ne->lat >= node->loc.lat){
        //resize array if need be
        if((*index + 1) >= *capacity){
            *capacity *= 2;
            *loc_points = realloc(*loc_points, sizeof(location) * *capacity);
        }

        //now add node to array
        (*loc_points)[(*index)++] = node->loc;
    }

    //get cut dimension that determines how to traverse tree
    int cut_dim = depth % 2;
    //for lon
    if(cut_dim == 0){
        //if the node is to the right of sw, traverse its left
        if(sw->lon <= node->loc.lon){
            kdtree_range_helper(node->left, sw, ne, loc_points, index, capacity, depth + 1);
        }
        //if the node is to the left of ne, traverse its right
        if(ne->lon >= node->loc.lon){
            kdtree_range_helper(node->right, sw, ne, loc_points, index, capacity, depth + 1);
        }
    } else{//for lat
        if(sw->lat <= node->loc.lat){
            kdtree_range_helper(node->left, sw, ne, loc_points, index, capacity, depth + 1);
        }
        if(ne->lat >= node->loc.lat){
            kdtree_range_helper(node->right, sw, ne, loc_points, index, capacity, depth + 1);
        }
    }
}

location *kdtree_range(const kdtree *t, const location *sw, const location *ne, int *n){
    if(t == NULL || sw == NULL || ne == NULL || n == NULL){
        return NULL;
    }

    size_t capacity = 15;
    size_t index = 0;
    location *loc_points = malloc(sizeof(location) * capacity);

    kdtree_range_helper(t->root, sw, ne,&loc_points,  &index, &capacity, 0);

    *n = index;
    if (index == 0){//nothing was stored
        free(loc_points);
        return NULL;
    }
    return loc_points;
}

//Helper function
void kdtree_range_for_each_helper(kdtree_node *node, const location *sw, const location *ne, void (*f)(const location *, void *), void *arg, int depth){
    if (node == NULL){
        return;
    }

    //check if node is within range
    if(sw->lon <= node->loc.lon && ne->lon >= node->loc.lon && sw->lat <= node->loc.lat && ne->lat >= node->loc.lat){
        //call f on node
        f(&node->loc, arg);
    }
    //current cut
    int cut_dim = depth % 2;
    //traverse left/right depending on the cut
    //for lon
    if(cut_dim == 0){
        if(sw->lon <= node->loc.lon){
            kdtree_range_for_each_helper(node->left, sw, ne, f, arg, depth + 1);
        }
        if(ne->lon >= node->loc.lon){
            kdtree_range_for_each_helper(node->right, sw, ne, f, arg, depth + 1);
        }
    } else{ //for lat
        if(sw->lat <= node->loc.lat){
            kdtree_range_for_each_helper(node->left, sw, ne, f, arg, depth + 1);
        }
        if(ne->lat >= node->loc.lat){
            kdtree_range_for_each_helper(node->right, sw, ne, f, arg, depth + 1);
        }
    }
}
void kdtree_range_for_each(const kdtree *t, const location *sw, const location *ne, void (*f)(const location *, void *), void *arg){
    if(t == NULL || sw == NULL || ne == NULL || f == NULL){
        return;
    }
    kdtree_range_for_each_helper(t->root, sw, ne, f, arg, 0);
}

//helper function
void kdtree_destroy_helper(kdtree_node *node){
    //base case
    if(node == NULL){
        return;
    }
    //recursively free the nodes
    kdtree_destroy_helper(node->left);
    kdtree_destroy_helper(node->right);

    free(node);
}
void kdtree_destroy(kdtree *t){
    if(t == NULL){
        return;
    }
    kdtree_destroy_helper(t->root);
    //free kdtree itself
    free(t);
}