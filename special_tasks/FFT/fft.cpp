#include <require.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <omp.h>

const double pi = M_PI;
typedef std::complex<double> value_t;

#define THRESHOLD 64

std::vector<value_t> fft(std::vector<value_t> &signal)
{
    int n = signal.size();
    if(n == 1)
        return signal;

    std::vector<value_t> even_points(n / 2), odd_points(n / 2);
    for(int i = 0; 2 * i < n; i++)
    {
        even_points[i] = signal[2 * i];
        odd_points[i] = signal[2 * i + 1];
    }
    std::vector<value_t> Em = fft(even_points);
    std::vector<value_t> Om = fft(odd_points);

    std::vector<value_t> result(n);
    double angle = -2 * pi / n;
    value_t w(1), wn(cos(angle), sin(angle));
    int n2 = n / 2;
    for(int i = 0; i < n2; i++)
    {
        result[i] = Em[i] + w * Om[i];
        result[i + n2] = Em[i] - w * Om[i];
        w *= wn;
    }

    return result;
}

std::vector<value_t> impl__fft_omp(std::vector<value_t> &signal)
{
    int n = signal.size();
    if(n == 1)
        return signal;

    std::vector<value_t> even_points(n / 2), odd_points(n / 2);
    for(int i = 0; 2 * i < n; i++)
    {
        even_points[i] = signal[2 * i];
        odd_points[i] = signal[2 * i + 1];
    }

    std::vector<value_t> Em;
    std::vector<value_t> Om;
    #pragma omp task shared(Em) if(n > THRESHOLD)
    Em = impl__fft_omp(even_points);
    #pragma omp task shared(Om) if(n > THRESHOLD)
    Om = impl__fft_omp(odd_points);
    #pragma omp taskwait

    std::vector<value_t> result(n);
    double angle = -2 * pi / n;
    value_t w(1), wn(cos(angle), sin(angle));
    int n2 = n / 2;
    for(int i = 0; i < n2; i++)
    {
        result[i] = Em[i] + w * Om[i];
        result[i + n2] = Em[i] - w * Om[i];
        w *= wn;
    }

    return result;
}

std::vector<value_t> fft_omp(std::vector<value_t> &signal, int threads)
{
    std::vector<value_t> result;

    #pragma omp parallel num_threads(threads)
    #pragma omp single
    result = impl__fft_omp(signal);

    return result;
}

std::vector<double> read_csv(const std::string &filename)
{
    std::ifstream file(filename);
    require(file.is_open(), "Can't open source file....");

    std::vector<double> data;
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream stream(line);
        double value;
        while (stream >> value)
        {
            data.push_back(value);
            if (stream.peek() == ',' || stream.peek() == ';')
                stream.ignore();
        }
    }
    return data;
}

void write_csv(const std::string &filename, const std::vector<value_t> &spectrum)
{
    std::ofstream file(filename);
    require(file.is_open(), "Can't open destination file....");

    for (auto &x : spectrum)
        file << std::abs(x) << "\n";
}

int main(int argc, char **argv)
{
    require(argc >= 3, "Usage: fft (in) (out) [o(threads)]");
    int threads = 0;
    if((argc == 4) && (argv[3][0] == 'o'))
        threads = std::atoi(argv[3] + 1);
    if(threads > 0)
        std::cout << "omp threads: " << threads << std::endl;

    std::string input_file = argv[1];
    std::string output_file = argv[2];

    std::vector<double> signal_values = read_csv(input_file);
    int n = signal_values.size();
    require(n > 0, "There is no points");
    require((n & (n - 1)) == 0, "Count of points should be power of 2");
    std::vector<value_t> signal(signal_values.begin(), signal_values.end());
    double start = omp_get_wtime();
    std::vector<value_t> spectrum;
    if(threads > 0)
        spectrum = fft_omp(signal, threads);
    else
        spectrum = fft(signal);
    double end = omp_get_wtime();
    write_csv(output_file, spectrum);

    std::cout << "time " << (end - start) << "s." << std::endl;
    return 0;
}
