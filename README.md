# Large-Volume-Data-Visualization
Interactive Visualisation of Large-scale Medical Datasets on Low-level GPUs using Octrees, Cuda C &amp; OpenGL

### Setting up
- Download cuda-toolkit from `https://developer.nvidia.com/cuda-75-downloads-archive` as Cuda-7.5 is supported.
- Actual dataset file with name `VH_1760_1024_1878.raw` which was used in pre-processing is a 3.4 GB sized dataset protected under confidentiality clause.
- _"pre-processing"_: This module converts this base dataset file into octree-based files structures.
- _"visualisation"_: This module uses openGL and cuda rendering to perform visualisation of the entire dataset on run-time.
