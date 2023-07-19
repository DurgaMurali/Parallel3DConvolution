#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

void readMatrixFromFile(std::string filename, int height, int width, int depth, std::vector<std::vector<std::vector<int>>> &imgMatrix);
void readMatrixFromFile(std::string filename, int height, int width, int depth, std::vector<std::vector<std::vector<float>>> &imgMatrix);
void readMatrixFromFile(std::string filename, int height, int width, int depth, std::vector<std::vector<std::vector<double>>> &imgMatrix);