#pragma once

#include <vector>
#include <complex>

std::vector<std::complex<double>>
dft(const std::vector<std::complex<double>>& input);

std::vector<std::complex<double>>
idft(const std::vector<std::complex<double>>& input);