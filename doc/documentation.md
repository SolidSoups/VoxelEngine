# 2026-03-23

Today I'm working optimizing faces.

I spawned in a sphere, at (16, 16, 16) with a radius of 10. That generates 4169 voxels, which equivelates to 50028 triangles.

The first fix is cull all inside faces. I do this by keeping arrays bitsets for three axis, +X, +Z, +Y. Each bitset array stores a **CHUNK_SIZE * CHUNK_SIZE** of columns or rows, and tell me if the cell is empty or filled.

We can use these bitset arrays to find faces in a axis for both directions. For +Y columns, we can find top and bottom faces using bit-operations. For example:

```
// Get the bottom faces of a bitset named 'x'
// (with y=0 being the least significant bit)
// (assuming a CHUNK_SIZE of 8)

x = 1011 0101
bottomFaces = x & ~(x << 1)
bottomFaces = 1011 0101 & ~(1011 0101 << 1)
bottomFaces = 1011 0101 & ~(0110 1010)
bottomFaces = 1011 0101 & 1001 0101
bottomFaces = 1001 0101

// we now know where to put bottom faces

x           = 1011 0101
              ^  ^  ^ ^
bottomFaces = 1001 0101

```

## Update later
Finally got the shell mesh generation working. It was going really good for a while, using the yColumns, xRows and zRows to first generate 6 lists of vectors for the positions of all 6 faces of a cube, then using that data to determine where put the different faces. 

But then i ran into a problem where the faces weren't facing the right way ? So for an hour i was just switching orientations on faces, swapping names, until i realized that the faces we're swapped in the Culling method for right and left faces.

But anyways, it looks great. Now i need to clean it up and make it nice:w

# 2026-03-24
After adding some logging, we got this output:

```
Building Mesh. Voxel Count: 17074, Face Count: 4770, Triangle Count: 9540
```

This is of course with a larger sphere with a radius of 16 voxels.

With the original approach, we would have rendered:

```
// 12 triangles per voxel
triangles = 17074 * 12 = 204888
```

204 888 triangles. With the new approach we are rendering 9540 triangles. That's an decrease of triangles by *95.343798%* !!! Insane!


# 2025-03-25
Finally!! Binary greedy meshing is implemented.

For a voxel sphere, placed at (16,16,16) with a "radius" of 15, generates 169´764 triangles, if naively meshed with cubes placed at each voxel spot.

If we instead build a culled mesh, where we never put triangles inside a group of voxels, we get that count down to 8508 triangels.

And finally, when we binary greedy mesh, that count goes down to 4056.

## Comparatively
Naive approach - **169 764** triangels
Culled approach - **8 508** triangels (reduced by 95% compared to naive)
Greedy approach - **4 056** triangels (reduced by 97% compared to naive, 52% compared to culled)

Culling gives the biggest win here. Greedy meshing removes about half of the remaining triangles from there on, but it does take 1ms all together to do this. Maybe there are some optimizations that could be done, but right now it's not eating too much of the budget. This means that I can greedy mesh 16 chunks at the same time and still achieve a frame rate of 60fps.
