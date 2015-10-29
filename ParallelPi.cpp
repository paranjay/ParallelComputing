
#include "omp.h"
#include <iostream>



int num_steps = 100000;
double step;

using namespace std;


int main () {
    int i;
    double x, pi, sum = 0.0;
    step = 1.0 / (double) num_steps;
    #pragma omp parallel for reduction ( + : sum)
    {
//        int t_id = omp_get_thread_num();
//        int b_size = 1000 / omp_get_num_threads();

        for (int i = 0; i < num_steps; i++)
        {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }
    }
    pi = step * sum;
    cout << pi;
}

//
//#include <iostream>
//#include <omp.h>
//
//using namespace std;
//
//int main() {
//    int my_choice;
//    cin >> my_choice;
//    int num_steps = 100000;
//    double step;
//    int i;
//
//    double pi, sum = 0.0;
//    step = 1.0/(double) num_steps;
//
//    #pragma omp parallel num_threads(my_choice)
//    {
//        int t_id = omp_get_thread_num();
//        int n = omp_get_num_threads();
//
//        int b_size = num_steps/n;
//
//        for (int i = (t_id-1) * b_size; i < t_id * b_size; i ++)
//        {
//            double x = (i+0.5)*step;
//            double a = sum;
//            sum = a + 4.0/(1.0+x*x);
//        }
//    }
//
//    pi = step * sum;
//    cout<<pi<<endl;
//}