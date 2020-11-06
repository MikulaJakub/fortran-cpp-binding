! main program

program main

implicit none

integer, dimension(:), allocatable :: data
integer :: i
integer :: N

    print*, "main program has started"

    write(*,'(a)', advance="no") "type number of grid points in one direction (int): "
    read(*,'(I4)') N    

    write(*,'(a)', advance="no") "chosen N = " 
    write(*,*) N

    ! allocate the array 'data'
    allocate(data(N*N*N))

    ! define data to be plotted by vtk function
    ! for simplicity, i increases linearly
    do i=1,N*N*N
        data(i) = i
    enddo

    ! call a c++ function that will be using vtk to plot the data
    print*, "calling c++ function to plot data"
 
    ! plot_unstructure_grid(pointer to data, (int) size in one direction)
    CALL plot_unstructured_grid(data, N)
 
    print*, achar(27)//"[1m"//achar(27)//"[33mmain program has finished"//achar(27)//"[0m"

end program main
