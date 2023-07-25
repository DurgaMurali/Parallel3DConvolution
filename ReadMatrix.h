#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

enum dataType
{
    INT = 1,
    FLOAT = 2,
    DOUBLE = 3
};

class ReadMatrix
{
public:
    int findFileDatatype(std::string filename);

    template <typename T>
    void readMatrixFromFile(std::string filename, int height, int width, int depth, std::vector<std::vector<std::vector<T>>> &imgMatrix)
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
     
                if(std::is_same<T, int>::value)
                    imgMatrix[k][i][j] = std::stoi(value);
                else if(std::is_same<T, double>::value)
                    imgMatrix[k][i][j] = std::stod(value);
                else
                    imgMatrix[k][i][j] = static_cast<T>(0);

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
                
                if(std::is_same<T, int>::value)
                    imgMatrix[k][i][j] = std::stoi(value);
                else if(std::is_same<T, double>::value)
                    imgMatrix[k][i][j] = std::stod(value);
                else
                    imgMatrix[k][i][j] = static_cast<T>(0);

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
    //void readMatrixFromFile(std::string filename, int height, int width, int depth, std::vector<std::vector<std::vector<double>>> &imgMatrix);

    template <typename T>
    void readMatrixFromFile(std::string filename, int size, T *imgMatrix)
    {
        std::cout<<"File = " << filename<<", Size = " << size << std::endl;
        std::ifstream file(filename);
        if(file)
            std::cout << "File found" << std::endl;
        std::string line;
        int index = 0;
        
        while(std::getline(file, line))
        {
            int pos = line.find(",");

            while(pos != std::string::npos)
            {
                std::string value = line.substr(0, pos);
                line.erase(0, pos+1);
                pos = line.find(",");

                if(std::is_same<T, int>::value)
                    imgMatrix[index] = std::stoi(value);
                else if(std::is_same<T, double>::value)
                    imgMatrix[index] = std::stod(value);
                else
                    imgMatrix[index] = static_cast<T>(0);

                index++;
            }
            if(!line.empty())
            {
                pos = line.find("\n");
                std::string value = line.substr(0, pos);  

                if(std::is_same<T, int>::value)
                    imgMatrix[index] = std::stoi(value);
                else if(std::is_same<T, double>::value)
                    imgMatrix[index] = std::stod(value);
                else
                    imgMatrix[index] = static_cast<T>(0);
                
                index++;
            }
        }

        file.close();
    }
    //void readMatrixFromFile(std::string filename, int size, double *imgMatrix);
};