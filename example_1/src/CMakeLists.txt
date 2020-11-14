set(HEADERS
#	
)

set(SOURCES
	plot_unstructured_grid.cpp
)

	add_executable(a.out main.f90 ${SOURCES} ${HEADERS})
	target_link_libraries(a.out
		${VTK_LIBRARIES}
)

