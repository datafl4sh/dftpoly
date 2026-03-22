#include <vector>
#include <complex>
#include <cmath>

std::vector<std::complex<double>>
dft(const std::vector<std::complex<double>>& input)
{
    const std::size_t N = input.size();
    const double PI = std::acos(-1);

    std::vector<std::complex<double>> output(N);

    for (std::size_t k = 0; k < N; ++k) {
        std::complex<double> sum(0.0, 0.0);

        for (std::size_t n = 0; n < N; ++n) {
            double angle = -2.0 * PI * k * n / static_cast<double>(N);
            std::complex<double> w(std::cos(angle), std::sin(angle));

            sum += input[n] * w;
        }

        output[k] = sum;
    }

    return output;
}

std::vector<std::complex<double>>
idft(const std::vector<std::complex<double>>& input)
{
    const std::size_t N = input.size();
    const double PI = std::acos(-1);

    std::vector<std::complex<double>> output(N);

    for (std::size_t n = 0; n < N; ++n) {
        std::complex<double> sum(0.0, 0.0);

        for (std::size_t k = 0; k < N; ++k) {
            double angle = 2.0 * PI * k * n / static_cast<double>(N);
            std::complex<double> w(std::cos(angle), std::sin(angle));

            sum += input[k] * w;
        }

        output[n] = sum / static_cast<double>(N);
    }

    return output;
}