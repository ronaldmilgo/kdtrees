# KD-Tree: Invasive Species Tracker

This project implements a **2D kd-tree** data structure in C, supporting efficient spatial queries over geographic points. It simulates a system for tracking reports of invasive species and helps identify the most affected regions based on spatial density.

## 🌱 Project Context

Imagine a mobile app where users submit GPS coordinates of invasive species sightings. Environmental teams can use this data to plan responses, focusing on areas with the most reports. This project enables those range queries efficiently through a kd-tree.

## 🎯 Objectives

- Implement a spatial set-of-points ADT  
- Use a 2D kd-tree to support add, find, remove, and range queries  
- Ensure balanced or approximately balanced tree construction  
- Achieve expected logarithmic time bounds for operations  

## 📌 Supported Operations

| Function                  | Description                                              |
|---------------------------|----------------------------------------------------------|
| `kdtree_create`           | Build a balanced tree from an array of points            |
| `kdtree_add`              | Insert a new point into the kd-tree                      |
| `kdtree_contains`         | Check if a point exists in the tree                      |
| `kdtree_remove`           | Delete a point from the tree                             |
| `kdtree_range`            | Return list of points in a rectangular region            |
| `kdtree_range_for_each`   | Apply a function to all points in a rectangular region   |
| `kdtree_destroy`          | Free all memory used by the tree                         |

## 🧭 Coordinate Notes

- All points are treated as unique, even if they lie at poles or opposite longitudes (`-180` vs `180`)  
- No coordinate normalization is applied — input must meet preconditions  
- Range queries do **not** cross the ±180° longitude boundary  

## ⏱️ Performance

Assuming a balanced or approximately balanced kd-tree:

| Function                | Time Complexity         |
|-------------------------|--------------------------|
| `kdtree_create`         | O(n log n)               |
| `kdtree_add`            | O(log n) (expected)      |
| `kdtree_contains`       | O(log n) (expected)      |
| `kdtree_remove`         | O(log n) (expected)      |
| `kdtree_range`          | O(k + log n)             |
| `kdtree_range_for_each` | O(k + log n)             |
| `kdtree_destroy`        | O(n)                     |

(`n` = total points, `k` = points within range)

## 🧱 Tree Construction

- The tree should be as balanced as possible  
- Preferably use the **median-splitting** algorithm  
- Alternatively, inserting points in **random order** yields an approximately balanced tree (acceptable within 5-point tolerance)  

## 💡 Additional Hints

- Use the `location` module for comparing latitude and longitude  
- The `kdtree_helpers` module provides basic min/max logic (not optimized)  
- You can use the optional `plist` module to manage dynamic point lists  

---

This project applies spatial data structures to solve real-world geographic problems, optimizing search performance in environmental monitoring systems.
