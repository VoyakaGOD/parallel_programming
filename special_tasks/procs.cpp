#include <iostream>
#include <cstdlib>
#include <omp.h>

// P - performance cores, E - efficiency cores

/* export OMP_PLACES = threads => omp_get_num_places() = 
                                  = omp_get_num_procs() = logical cores count = 2P + E*/

// export OMP_PLACES = cores => omp_get_num_places() = physical cores count = P + E

// export OMP_PLACES = sockets => omp_get_num_places() = CPU's count

int main()
{
    std::cout << "Num procs: " << omp_get_num_procs() << std::endl;
    const char* places = std::getenv("OMP_PLACES");
    if(places == nullptr)
        places = "default";
    std::cout << "Num places(OMP_PLACES = " << places << "): ";
    std::cout << omp_get_num_places() << std::endl;
}
