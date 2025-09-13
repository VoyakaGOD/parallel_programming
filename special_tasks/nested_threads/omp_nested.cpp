#include <iostream>
#include <omp.h>

// omp_get_level()
// omp_get_team_size(level) / omp_get_num_threads()
// omp_get_ancestor_thread_num(level) / omp_get_thread_num()

void navigate()
{
    int id = omp_get_thread_num();
    int local_size = omp_get_num_threads();
    int level = omp_get_level();
    std::cout << "Thread " << id << "/" << local_size << " at level " << level << std::endl;
    std::cout << "*    Hierarchy:";
    for(int i = 1; i < level; i++)
        std::cout << " " << omp_get_ancestor_thread_num(i) << "/" << omp_get_team_size(i);
    std::cout << std::endl;
}

int main()
{
    omp_set_nested(true);

    #pragma omp parallel num_threads(3)
    {
        #pragma omp critical
        navigate();

        #pragma omp parallel num_threads(2)
        {
            #pragma omp critical
            navigate();

            #pragma omp parallel num_threads(2)
            {
                #pragma omp critical
                navigate();
            }
        }
    }

    return 0;
}
