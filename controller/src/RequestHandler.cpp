#include "RequestHandler.hpp"
#include "Requests.hpp"
#include "Parser.hpp"
#include <iostream>

const std::string RequestHandler::requestDelimiter = "\r";
const std::string RequestHandler::responseDelimiter = "\r\r>";

RequestHandler::RequestHandler(const std::string& serialPortPath) : io(), serial(boost::asio::serial_port(io, serialPortPath)) {
    serial.set_option(boost::asio::serial_port_base::baud_rate(38400));
    serial.set_option(boost::asio::serial_port_base::character_size(8));
    serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    get<Requests::RESET>();
    get<Requests::ECHO_OFF>();
    get<Requests::HEADER_OFF>();
    get<Requests::LINEFEED_OFF>();
    get<Requests::SPACE_OFF>();

    loadAvailableDataCommands();
}

template <class TCommand>
typename TCommand::Type RequestHandler::get() {
    const auto command = TCommand::toString();
    const auto response = request(command);
    return Parser::parse<TCommand>(response);
}

void RequestHandler::loadAvailableDataCommands() {
    auto commandOffset = 0;
    for (auto& command : Requests::AVAILABLE_DATA_COMMANDS) {
        const auto response = request(command);
        const std::string startSequence = "41 " + std::string(command).substr(2, 2) + " ";
        const auto startIndex = response.find(startSequence) + startSequence.size();
        if (startIndex < 5 || startIndex + 4 * 3 - 1 >= response.size()) {
            return;
        }
        const auto encodedAvailability = response.substr(startIndex, 4 * 3 - 1);

        auto wordOffset = 0;
        for (const auto character : encodedAvailability) {
            if (character == ' ') {
                continue;
            }
            const auto value = std::stoi(std::string(1, character), nullptr, 16);

            for (int bit = 0; bit < 4; bit++) {
                const auto commandAsInt = 1 + bit + wordOffset + commandOffset;
                const auto decodedCommand = "01" + std::format("{:0{}X}", commandAsInt, 2);
                const bool isAvailable = value & (1 << (3 - bit));
                availableCommands[decodedCommand] = isAvailable;
            }

            wordOffset += 4;
        }
        commandOffset += 32;
    }
}

void RequestHandler::printAvailableCommands() const {
    std::cout << "Available commands:" << std::endl;
    for (const auto& [key, isAvailable] : availableCommands) {
        std::cout << key << ": " << isAvailable << std::endl;
    }
}

void RequestHandler::printAvailableForDataFrame() const {
    std::cout << "Available for data frame:" << std::endl;
    std::cout << "    Engine load: " << availableCommands.at(Requests::ENGINE_LOAD::toString()) << std::endl;
    std::cout << "    Engine RPM: " << availableCommands.at(Requests::ENGINE_RPM::toString()) << std::endl;
    std::cout << "    Vehicle speed: " << availableCommands.at(Requests::VEHICLE_SPEED::toString()) << std::endl;
    std::cout << "   Throttle position: " << availableCommands.at(Requests::THROTTLE_POSITION::toString()) << std::endl;
    std::cout << "    Uptime: " << availableCommands.at(Requests::UPTIME::toString()) << std::endl;
    std::cout << "    Fuel level: " << availableCommands.at(Requests::FUEL_LEVEL::toString()) << std::endl;
    std::cout << "    Absolute load: " << availableCommands.at(Requests::ABSOLUTE_LOAD::toString()) << std::endl;
    std::cout << "    Relative throttle position: " << availableCommands.at(Requests::RELATIVE_THROTTLE_POSITION::toString()) << std::endl;
    std::cout << "    Engine fuel rate: " << availableCommands.at(Requests::ENGINE_FUEL_RATE::toString()) << std::endl;
}

DataFrame RequestHandler::getDataFrame() {
    return DataFrame{ get<Requests::ENGINE_LOAD>(),
        get<Requests::ENGINE_RPM>(),
        get<Requests::VEHICLE_SPEED>(),
        get<Requests::THROTTLE_POSITION>(),
        get<Requests::UPTIME>(),
        get<Requests::FUEL_LEVEL>(),
        get<Requests::ABSOLUTE_LOAD>(),
        get<Requests::RELATIVE_THROTTLE_POSITION>(),
        get<Requests::ENGINE_FUEL_RATE>() };
}

std::string RequestHandler::request(const std::string& rawCommand) {
    if (!availableCommands.contains(rawCommand) || !availableCommands.at(rawCommand)) {
        return "";
    }
    std::string command = rawCommand + requestDelimiter;
    write(serial, boost::asio::buffer(command));

    boost::asio::read_until(serial, inputBuffer, responseDelimiter);

    std::istream inputStream(&inputBuffer);
    std::string rawResponse;
    std::getline(inputStream, rawResponse);

    std::string response;
    std::ranges::copy_if(rawResponse, std::back_inserter(response),
        [](unsigned char c) { return std::isalnum(c); });
    // std::cout << "--- Raw response ---" << std::endl << response << std::endl << "--- End of raw response ---" << std::endl;
    return response;
}