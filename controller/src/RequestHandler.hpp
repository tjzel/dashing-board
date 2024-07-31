#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP
#include <string>
#include <map>
#include <boost/asio.hpp>
#include "DataFrame.hpp"

class RequestHandler {
public:
    explicit RequestHandler(const std::string& serialPortPath);

    template <class TCommand>
    typename TCommand::Type get();

    void loadAvailableDataCommands();

    void printAvailableCommands() const;

    void printAvailableForDataFrame() const;

    DataFrame getDataFrame();

private:
    std::size_t availableData();
    std::string readAllAvailableData();

    std::string request(const std::string& rawCommand);
    bool isCommandAvailable(const std::string& command) const;

    boost::asio::io_service io;
    boost::asio::serial_port serial;
    boost::asio::streambuf inputBuffer;
    std::map<const std::string, bool> availableCommands;

};
#define REQUESTHANDLER_HPP

#endif //REQUESTHANDLER_HPP
