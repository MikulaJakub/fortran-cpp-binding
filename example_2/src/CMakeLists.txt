set(HEADERS
#	
)

set(SOURCES
	send_data_to_plot.cpp
)

	add_executable(a_fortran.out main.f90 ${SOURCES})

	add_executable(a.out main.cpp ${SOURCES} ${HEADERS})
	target_link_libraries(a.out
		${VTK_LIBRARIES}

)

