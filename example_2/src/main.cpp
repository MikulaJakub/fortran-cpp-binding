// (c) 2020, Jakub Mikula
// mikula_jakub@centrum.sk

/* DESCRIPTION:
	Main program creates a shared memory segments.
	It then calls an external executable (that might have been written in Fortran) 
	using system call with the input argument containing the id of the shared memory.
	Fortran calls a C++ function to which it passes the data and the c++ code shares 
	the data on the shared memory segment. The main program can then read the data
	and visualize them using the VTK libraries.

	This approach requires only a few lines of code to be added to Fortran code.

*/
	
#include <stdio.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>

#include <memory>
#include <stdexcept>
#include <string>
#include <array>

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

#include <thread>
#include <unistd.h>

#define RESET       "\033[0m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDYELLOW  "\033[1m\033[33m"


VTK_MODULE_INIT(vtkRenderingOpenGL2)
// otherwise I received error: no override found for ... 

VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)	//this is required to plot the colorbar!


using namespace std;

void run();
std::string exec(const char* cmd);
void plot_unstructured_grid();
int id; // shared memory id
int n_points;
vtkSmartPointer<vtkDoubleArray> data; 
vtkSmartPointer<vtkUnstructuredGrid> grid;
vtkSmartPointer<vtkRenderer> renderer;
vtkSmartPointer<vtkRenderWindow> renderWindow;
vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor; 

int main(int argc, char *argv[])
{

    cout << ' ' << endl;
	cout <<BOLDYELLOW<< "Main program has started ..." <<RESET<< endl;
        
    int n_cells = 0;
    char* p;
    if (argc > 1)
        n_cells = strtol(argv[1], &p, 10);
    else
    {
        cout << "Number of arguments must be more than one!" << endl;
        cout <<BOLDRED<< "Process terminated!" <<RESET<< endl;
        exit(1); 
    }

	n_points = n_cells + 1; // number of grid points

    // (-1) prepare the window to plot the data
	renderer = vtkSmartPointer<vtkRenderer>::New();
	
	renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetWindowName("Data from main Fortran");
	renderWindow->AddRenderer(renderer);

	renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

    // (0)  allocate VTK grid    
	std::cout << "Allocate vtk grid" << std::endl;
	grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
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
	data = vtkSmartPointer<vtkDoubleArray>::New();
	data->SetName("data");
	data->SetNumberOfComponents(1);
	data->SetNumberOfTuples(n_points*n_points*n_points);

// Run on a separate thread to interacto with GUI
// I commented this feature now, as VTK is not multithread safe
	run();
//	thread myThread(run);

	renderWindowInteractor->Initialize();
	renderWindowInteractor->Start();
	

	cout <<BOLDGREEN<< "Main program has finished!" <<RESET<< endl;

	return EXIT_SUCCESS;
}

void run()
{

	// Wait to make sure that render window is initialized in time
	sleep(1);

	// (1) 	create a shared memory segment
	key_t key;
	size_t size;
	int shmflg;

	// size of shared memory segment
	// here, we'll be sharing an array N*N*N of doubles
	size = n_points*n_points*n_points*sizeof(double);	

	// create a shared memory segment
	key = IPC_PRIVATE;	//let the kernel choose a new key
	shmflg = 0666|IPC_CREAT;
	
	cout << "Requesting shared memory ... " << endl;
	id = shmget(key, size, shmflg);

	cout << "Shared memory segment created:" << endl;
	cout << "id = " << id << endl;
	cout << "key = " << key << endl;
	cout << "size = " << size << endl;
	cout << "shmflg = " << shmflg << endl;

	// (2) 	call the Fortran program
	// 		Fotran program will do the calculations and call a C++ function
	//		to write data into this shared memory segment.

    // specify the name of the program and all command line arguments
    std::string program;
    program.append("./a_fortran.out");
    program.append(" ");
    program.append(std::to_string(n_points));
    program.append(" ");
    program.append(std::to_string(id));

//	system(program.c_str());	
    exec(program.c_str());  // this function will return the standard output of the executed program
 

	// destroy the shared memory segment
	shmctl(id,IPC_RMID,NULL);	
}

// Read standard output of the executable file
std::string exec(const char* cmd)
{
    std::string keywr = "output_data\n";

    std::array<char,128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe){
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) 
    {
        result += buffer.data();
        std::cout << buffer.data(); 

        std::string thisString = buffer.data();

        // if registered the keyword, plot the data ... 
        if ( thisString.compare(keywr) == 0 )
        {
            // the keyword here is "output_data"
            std::cout <<BOLDBLUE<< "Keyword to plot data registered" <<RESET<< endl;

            // plot data
            plot_unstructured_grid();

        }


    }    
    return result;
} 

void plot_unstructured_grid()
{

    std::cout << "Look into the shared memory to read the data to plot ... " << std::endl;

    int* shared_memory;
    shared_memory = (int*) shmat(id, NULL, 0666);    


	// copy data passed from the fortran code
	for (int i=0; i<n_points*n_points*n_points; i++)
	{
		data->SetComponent(i, 0, *(shared_memory+i));
        //std::cout << *(shared_memory+i) << std::endl;
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
	scalarBar->SetTitle("Colorbar");
	scalarBar->SetNumberOfLabels(4);

	mapper->SetLookupTable(hueLut);
	scalarBar->SetLookupTable(hueLut);


	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->EdgeVisibilityOn();

		
	renderer->AddActor(actor);
	renderer->AddActor2D(scalarBar);
	renderer->SetBackground(0.1, 0.3, 0.7);

	// render the scene
	renderer->ResetCamera();
	renderWindow->Render();

    return;
}
