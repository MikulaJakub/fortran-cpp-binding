# Binding fortran and C++ codes

This repository contains examples that demonstrate simple programs written using both fortran and c++ languages. While the popularity of c++ is nowadays indesputably higher (mainly because of the wealth of freely available libraries and the ease of their integration with c++ codes), many high performance computing simulations (including mines) still remain written in fortran (the language 99.9% of my friends never heard of before). Therefore finding ways for binding the two languages is a great way how to get the best of both of them.

# Examples

## example_1 (fortran calls c++ function)
In this example, the main program is written in fortran. The fortran code generates data (here a simple sequence of numbers from `1` to `N*N*N`) and calls a c++ function to which it passes the data to visualize them on a cube using VTK libraries. 

### compile the code
`mkdir build && cd build && cmake .. && make`

### run the code
`cd ../bin && ./a.out`

The program will ask for an integer `N`. In the demonstration below, `N=5`, the grid of 5x5x5 points is visualized.

![](figures/example_1.gif)

## example_2 (fortran calls c++ functions for ipc using shared memory)

