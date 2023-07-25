#include "ReadMatrix.h"

int ReadMatrix::findFileDatatype(std::string filename)
{
    std::ifstream file(filename);
    if(file)
        std::cout << "File found" << std::endl;
    std::string line;
    std::getline(file, line);
    int pos = line.find(",");

    if(pos != std::string::npos)
    {
        std::string value = line.substr(0, pos);
        //std::cout << "value = " << value << std::endl;
        int decimalPos = value.find(".");
        if(decimalPos != std::string::npos)
            return DOUBLE;
        else
            return INT;

    }
}