! This is the main Fortran program
! This program is system called from C++ server
! It performs the calculations, passes them to C++ server through a shared memory segment,
! and continues with the calculation.
!
! the data will be passed to shared memory only when shared memory exits;

program main
implicit NONE

double precision, dimension(:,:,:), allocatable :: data
integer :: ijk
integer :: i,j,k
integer :: N
integer :: id
integer :: iterator

double precision :: r
integer :: num_args
character(LEN=100) :: arg1, arg2

    print*, "Main Fortran program has started ..."

    ! Read command line arguments:
    num_args = command_argument_count()
    print*, "There are ", num_args, " command line arguments."

    CALL GETARG(1,arg1)
    CALL GETARG(2,arg2)    
    READ(arg1,*) N
    READ(arg2,*) id

    print*, "N = ", N
    print*, "id = ", id
    
    allocate( data (N,N,N))
    

    ! Perform some calculations in a loop
    DO,ijk=1,10,1
        iterator = ijk             

        DO, i=1,N
        DO, j=1,N
        DO, k=1,N
    
            ! simple random number generator
            CALL RANDOM_NUMBER(r)
            data(i,j,k) = r*10.0
            !data(i,j,k) = iterator*1.0;

            iterator = iterator + 1;

        ENDDO
        ENDDO
        ENDDO

        ! Send the result to serve to plot through the shared memory segment
        CALL send_data_to_plot(data, N, id)


        ! Pause for 1 second
        ! This pause is only to slowly see how the data are plotted
        ! Remove this from the main code!
        print*, "Pause for one second"
        CALL SLEEP(1)




    ENDDO




    print*, achar(27)//"[1m"//achar(27)//"[33mMain Fortran program has successfully finished!"//achar(27)//"[0m"


end program main
