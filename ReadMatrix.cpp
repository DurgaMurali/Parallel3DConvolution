#include "ReadMatrix.h"

void readMatrixFromFile(std::string filename, int height, int width, int depth, std::vector<std::vector<std::vector<int>>> &imgMatrix)
{
    std::cout<<"File = " << filename<<", Height = " << height << ", width = " << width << ", depth = " << depth << std::endl;
    std::ifstream file(filename);
    if(file)
        std::cout << "File found" << std::endl;
    std::string line;
    int i = 0, j = 0, k = 0;
    
    while(std::getline(file, line))
    {
        int pos = line.find(",");

        while(pos != std::string::npos)
        {
            std::string value = line.substr(0, pos);
            line.erase(0, pos+1);
            pos = line.find(",");
            imgMatrix[k][i][j] = std::stoi(value);
            j++;
            
            if(j == width)
            {
                j = 0;
                i++;
            }
            if(i == height)
            {
                i = 0;
                k++;
            }
        }
        if(!line.empty())
        {
            pos = line.find("\n");
            std::string value = line.substr(0, pos);
            imgMatrix[k][i][j] = std::stoi(value);
            j++;

            if(j == width)
            {
                j = 0;
                i++;
            }
            if(i == height)
            {
                i = 0;
                k++;
            }
        }
    }

    file.close();
}

void readMatrixFromFile(std::string filename, int height, int width, int depth, std::vector<std::vector<std::vector<float>>> &imgMatrix)
{
    std::cout<<"File = " << filename<<", Height = " << height << ", width = " << width << ", depth = " << depth << std::endl;
    std::ifstream file(filename);
    if(file)
        std::cout << "File found" << std::endl;
    std::string line;
    int i = 0, j = 0, k = 0;
    
    while(std::getline(file, line))
    {
        int pos = line.find(",");

        while(pos != std::string::npos)
        {
            std::string value = line.substr(0, pos);
            line.erase(0, pos+1);
            pos = line.find(",");
            imgMatrix[k][i][j] = std::stof(value);

            j++;
            
            if(j == width)
            {
                j = 0;
                i++;
            }
            if(i == height)
            {
                i = 0;
                k++;
            }
        }
        if(!line.empty())
        {
            pos = line.find("\n");
            std::string value = line.substr(0, pos);
            imgMatrix[k][i][j] = std::stof(value);
            j++;

            if(j == width)
            {
                j = 0;
                i++;
            }
            if(i == height)
            {
                i = 0;
                k++;
            }
        }
    }

    file.close();
}

void readMatrixFromFile(std::string filename, int height, int width, int depth, std::vector<std::vector<std::vector<double>>> &imgMatrix)
{
    std::cout<<"File = " << filename<<", Height = " << height << ", width = " << width << ", depth = " << depth << std::endl;
    std::ifstream file(filename);
    if(file)
        std::cout << "File found" << std::endl;
    std::string line;
    int i = 0, j = 0, k = 0;
    
    while(std::getline(file, line))
    {
        int pos = line.find(",");

        while(pos != std::string::npos)
        {
            std::string value = line.substr(0, pos);
            line.erase(0, pos+1);
            pos = line.find(",");
            imgMatrix[k][i][j] = std::stod(value);
            j++;
            
            if(j == width)
            {
                j = 0;
                i++;
            }
            if(i == height)
            {
                i = 0;
                k++;
            }
        }
        if(!line.empty())
        {
            pos = line.find("\n");
            std::string value = line.substr(0, pos);
            imgMatrix[k][i][j] = std::stod(value);
            j++;

            if(j == width)
            {
                j = 0;
                i++;
            }
            if(i == height)
            {
                i = 0;
                k++;
            }
        }
    }

    file.close();
}