#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <cmath>
#include <iomanip>

int main(int argc, char** argv) {
  const std::string file_name = argv[1];
  std::string N_str = argv[2];
  std::string seed_str = argv[3];
  const unsigned int N = std::stoi(N_str);
  const unsigned int seed = std::stoi(seed_str);
  const double min = -std::pow(2, -4);
  const double max = std::pow(2, -4);
  const double nonzero_probability = 1.0;
  const unsigned int significant_figures = 15;

  std::ofstream data_file (file_name);
  if (!data_file.is_open()) return 1;
  data_file << N << std::endl;

  std::mt19937 engine(seed);
  std::uniform_real_distribution<double> dist(min, max);
  std::binomial_distribution<> dist_bin(1, nonzero_probability);

  for (int i = 0; i < N; ++i) {
    data_file << std::setprecision(significant_figures) << dist(engine) * dist_bin(engine);
    for (int j = 0; j < N-1; ++j) {
      data_file << " " << std::setprecision(significant_figures) << dist(engine) * dist_bin(engine);
    }
    data_file << std::endl;
  }
  return 0;
}