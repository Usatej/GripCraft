Voxel mini-project (Minecraft-like) using Unreal Engine 5

Improvements:
* chunk generation on multiple threads in parallel (simple implementation ready using Task graph).
* do not render parts of blocks that hide each other (render only visible triangles) - implemented
* reduce the number of rendered vertices - currently one block has at worst 24 vertices (number of pages * 4). At the same time as reducing this number, it would be possible to combine several adjacent blocks of the same type into one so that the mesh of a single chunk is made up of several larger faces (use Greedy meshing for example).
* use octree instead of classic array to represent blocks to save memory (no need to split a "subsection" if it contains all blocks of the same type)
* chunk has dimensions x*y*maximum-height. It would be possible to split the chunk along the z-axis and only load the parts that are not completely covered or not empty. 
* reducing the size of the saved files. At the moment the saved world is made up of several files of quite large size. Each chunk is saved in its own file and is always saved in its entirety without any compression. It would be good to save compressed and only chunks that have been modified by the player. Alternatively, save only the changes made instead of the whole chunk.
