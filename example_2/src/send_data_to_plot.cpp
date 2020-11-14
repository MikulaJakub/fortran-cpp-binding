// This is a C++ function that will be run by main Fortran program

#include <stdio.h>
#include <iostream>

#include <sys/ipc.h>
#include <sys/shm.h>

// common block
extern "C" {
    void send_data_to_plot_(double* data_ptr, int* N_ptr, int* id_ptr);
    
    // data_ptr - data pointer
    // N - size of array in one dimension
    // id - shared memory id
}

// `double* data_ptr` because passing double data
void send_data_to_plot_(double* data_ptr, int* N_ptr, int* id_ptr)
{

    std::cout << "Passing data to shared memory (C++) ..." << std::endl;

    int N = *N_ptr;     //value pointe to by pointer
    int id = *id_ptr;

    std::cout << "N = " << N << std::endl;
    std::cout << "id = " << id << std::endl;

    // create a pointer to shared memory
    int* shared_memory;
    shared_memory = (int*) shmat(id, NULL, 0666);

    std::cout << "Shared memory attached at address " << shared_memory << std::endl;
    
    struct shmid_ds shmbuffer;
    shmctl(id, IPC_STAT, &shmbuffer);
    int segment_size = shmbuffer.shm_segsz;
    std::cout << "Segment size = " << segment_size << std::endl;

    // Load data into the shared memory (number by number)
    for (int i=0; i<N*N*N; i++)
    {
        *(shared_memory+i) = *(data_ptr+i); 
//        std::cout << *(data_ptr+i) << std::endl;

    } 
   
    
    std::cout << "Data has been passed to shared memory!" << std::endl;

    // send a keyword to standard output to let the server know that it should look into the shared memory to read the data
    std::cout << "output_data" << std::endl;

    return;
}


