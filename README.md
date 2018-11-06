[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)


# HYPE
Hypergraph partitioner based on the idea of neighborhood expansion that processes very large hypergraphs (with up to billions of vertices) using only a single thread. The source code is written in C++.

## How to build 
To build HYPE, make sure you have [Boost](https://www.boost.org/), [CMake](https://cmake.org) and a C++17 compatible
compiler, such as clang or gcc installed.

Then run the following to build HYPE:
```sh
git clone https://github.com/mayerrn/HYPE
cd HYPE
mkdir build && cd build
cmake ..
make
```
Another option is to use the `build.sh` script which automaticaly creates the `build/` 
folder, runs cmake and make in it for you.

## How to Use
To start the partitioner, follow the commands provided in the main file. The following parameters can be set:

Parameter | Effect
----------- | -----------
`help,h` | display help message
`raw,r` | if set, output is formatted in csv to make it easier to plot directly. If not set, the output is more verbose.
`input,i`| input hypergraph file
`format,f` | specify the input format of the hypergraph file
`partitions,p` | number of partitions
`sset-size,s` | maximum size of the secondary set (called 'fringe' in the paper); in paper, this is set to 10
`percent-of-edges-ignored,e` | how many percent of the biggest hyperedges will be removed; experimental, set to 0 to reproduce results from paper
`heuristic-calc-method,c` | Switch to choose between exact and cached calculation for the node heuristic 
`seed,x` | Seed used to initialize random number generators if used
`node-select-mode,m` | specifies how the a node will be choosen to when S-set is empty; in paper, `next-best` is used
`nh-expand-candidates,n` | number of candidates explored during neighbourhood expantion. Using other values than 2 is not recommended. To reproduce the results from the paper don't use this option at all or set it to 2.

## Input Formats
HYPE supports different input formats for the hypergraphs to make it easy to use.

### Bipartite
HYPE is able to read in bipartite graphs and transform them directly into hypergraphs.
To do so add the `-f bipartite` parameter when calling HYPE and make sure your input file
has the following structure:
```
vtx_id    vtx_id
vtx_id    vtx_id
...
```
which means each line models a edge from the vertex with the id on the 
left side to the vertex with the id on the right side.
Be aware that this input format is the slowest to read a hypergraph into HYPE

### hMetis
When HYPE is called with `-f hmetis`, HYPE expects the input to have the [hMetis input file fomat](http://glaros.dtc.umn.edu/gkhome/fetch/sw/hmetis/manual.pdf)
In our test the hMetis input format was the fastest to read in using HYPE.

### Edgelist
Per default HYPE expects a file in the edgelist format.
In the edgelist format each line models a node followed by a comma 
seperated list of edges it is connected to. 
For example:
```
4: 1, 2, 3, 4, 5
2: 1
5: 3, 5, 7
```
This would be a hypergraph with node `4` connected to edges `1,2,3,4,5` and so on.
This file format was used in the paper.
