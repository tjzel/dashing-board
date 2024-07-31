#include <iostream>
#include <string>
#include "RequestHandler.hpp"

int main(const int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <serial port>" << std::endl;
        return 1;
    }
    const std::string serialPort = argv[1];
    std::cout << "Using serial port: " << serialPort << std::endl;

    RequestHandler requestHandler(serialPort);

    requestHandler.printAvailableForDataFrame();

    std::cout << "Requesting data frame" << std::endl;
    const auto dataFrame = requestHandler.getDataFrame();
    std::cout << dataFrame.toString() << std::endl;

    return 0;
}