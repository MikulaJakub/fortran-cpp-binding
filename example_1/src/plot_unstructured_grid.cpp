// This program plots a cuboidal grid based on VTK unstructured grid
// data_ptr - data pointer
// size - number of points in one direction

// COMPILE:
// g++ -c plot_unstructured_grid.cpp -I /usr/local/include/vtk-8.2/
// where -I defines the directory containing the VTK header files

#include <stdio.h>
#include <iostream>

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkHexahedron.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

#include <vtkRenderWindow.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkAutoInit.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLookupTable.h>
#include <vtkDataArray.h>
#include <vtkScalarBarActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>

// common block
extern "C" {
	void plot_unstructured_grid_(int *data_ptr, int*size);
}


VTK_MODULE_INIT(vtkRenderingOpenGL2)
// otherwise I received error: no override found for ... 

VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)	//this is required to plot the colorbar!

void plot_unstructured_grid_(int *data_ptr, int*size)
{

	int n_points = *size;
	int n_cells = n_points-1;

	std::cout << "n_points = " << n_points << std::endl;
	std::cout << "n_cells = " << n_cells << std::endl;

	// allocate the VTK grid
	std::cout << "Allocate vtk grid" << std::endl;
	vtkSmartPointer<vtkUnstructuredGrid> grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	grid->Allocate(n_cells*n_cells*n_cells);
	
	// define points
	std::cout << "Define points" << std::endl;
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for (int k=0; k<n_points; k++)
	for (int j=0; j<n_points; j++)
	for (int i=0; i<n_points; i++)
		points->InsertNextPoint(i,j,k);

	grid->SetPoints(points);

	// define cells
	std::cout << "Define cells" << std::endl;
	vtkSmartPointer<vtkHexahedron> hex = vtkSmartPointer<vtkHexahedron>::New();
	for (int k=0; k<n_cells; k++)
	for (int j=0; j<n_cells; j++)
	for (int i=0; i<n_cells; i++)
	{
		hex->GetPointIds()->SetId(0,i+n_points*j+n_points*n_points*k);
		hex->GetPointIds()->SetId(1,i+n_points*j+n_points*n_points*k + 1);
		hex->GetPointIds()->SetId(2,i+n_points*j+n_points*n_points*k + n_points+1);
		hex->GetPointIds()->SetId(3,i+n_points*j+n_points*n_points*k + n_points);
		hex->GetPointIds()->SetId(4,i+n_points*j+n_points*n_points*k + n_points*n_points);
		hex->GetPointIds()->SetId(5,i+n_points*j+n_points*n_points*k + n_points*n_points+1);
		hex->GetPointIds()->SetId(6,i+n_points*j+n_points*n_points*k + n_points*n_points + n_points+1);
		hex->GetPointIds()->SetId(7,i+n_points*j+n_points*n_points*k + n_points*n_points + n_points);

		grid->InsertNextCell(hex->GetCellType(),hex->GetPointIds());

	}

	// define scalars to be plotted
	std::cout << "Define scalars" << std::endl;
	vtkSmartPointer<vtkDoubleArray> data = vtkSmartPointer<vtkDoubleArray>::New();
	data->SetName("data");
	data->SetNumberOfComponents(1);
	data->SetNumberOfTuples(n_points*n_points*n_points);

	// copy data passed from the fortran code
	for (int i=0; i<n_points*n_points*n_points; i++)
	{
		data->SetComponent(i, 0, *(data_ptr+i));
	}


	double range[2];
	std::cout << "Set scalars" << std::endl;
	grid->GetPointData()->SetScalars(data);
	grid->GetPointData()->SetActiveScalars("data");	
	grid->GetPointData()->GetArray("data")->GetRange(range, 0);
	std::cout << "range[0] = " << range[0] << " , range[1] = " << range[1] << std::endl;

	// define lookup table
	vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
	hueLut->SetTableRange (range[0], range[1]);
	hueLut->SetHueRange (0.667, 0.0);
	hueLut->SetSaturationRange (1, 1);
	hueLut->SetValueRange (1, 1);
	hueLut->Build();

	// plot data
	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(grid);
	mapper->ScalarVisibilityOn();
	mapper->SetScalarModeToUsePointData();
	mapper->SetColorModeToMapScalars();
	mapper->SetScalarRange(range[0], range[1]);

	vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
	scalarBar->SetLookupTable(mapper->GetLookupTable());
//	scalarBar->SetTitle("Colorbar");
	scalarBar->SetNumberOfLabels(4);

	mapper->SetLookupTable(hueLut);
	scalarBar->SetLookupTable(hueLut);


	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->EdgeVisibilityOn();

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetWindowName("Data");
	renderWindow->AddRenderer(renderer);
		
	renderer->AddActor(actor);
	renderer->AddActor2D(scalarBar);
	renderer->SetBackground(0.1, 0.3, 0.7);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// render the scene
	renderWindow->Render();
	renderWindowInteractor->Start();

	return;

}

