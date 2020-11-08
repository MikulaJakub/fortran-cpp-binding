// (c) 2020, Jakub Mikula
// mikula_jakub@centrum.sk

/* DESCRIPTION:
	Main program creates a shared memory segments.
	It then calls an external executable (that might have been written in fortran) 
	using system call with the input argument containing the id of the shared memory.
	Fortran calls a C++ function to which it passes the data and the c++ code shares 
	the data on the shared memory segment. The main program can then read the data
	and visualize them using the VTK libraries.
*/
	

#include <iostream>

using namespace std;

main int()
{

	cout << "Main program has started ..." << endl;



	cout << "Main program has finished!" << endl;

	return EXIT_SUCCESS;
} 
