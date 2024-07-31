#include <RequestHandler.hpp>
#include <Parser.hpp>
#include <iostream>

RequestHandler::RequestHandler(const std::string& serialPortPath) : serial(boost::asio::serial_port(io, serialPortPath)), availableCommands(
    Requests::defaultAvailable) {
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
        // const std::string startSequence = "41" + std::string(command).substr(2, 2);
        // const auto startIndex = response.find(startSequence) + startSequence.size();
        // if (startIndex < 5 || startIndex + 4 * 2 - 1 >= response.size()) {
        //     return;
        // }
        // const auto encodedAvailability = response.substr(startIndex, 4 * 3 - 1);
        const auto encodedAvailability = Parser::parseAvailability(response, command);

        auto wordOffset = 0;
        for (const auto value : encodedAvailability) {
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
    std::cout << "    Engine load: " << isCommandAvailable(Requests::ENGINE_LOAD::toString()) << std::endl;
    std::cout << "    Engine RPM: " << isCommandAvailable(Requests::ENGINE_RPM::toString()) << std::endl;
    std::cout << "    Vehicle speed: " << isCommandAvailable(Requests::VEHICLE_SPEED::toString()) << std::endl;
    std::cout << "   Throttle position: " << isCommandAvailable(Requests::THROTTLE_POSITION::toString()) << std::endl;
    std::cout << "    Uptime: " << isCommandAvailable(Requests::UPTIME::toString()) << std::endl;
    std::cout << "    Fuel level: " << isCommandAvailable(Requests::FUEL_LEVEL::toString()) << std::endl;
    std::cout << "    Absolute load: " << isCommandAvailable(Requests::ABSOLUTE_LOAD::toString()) << std::endl;
    std::cout << "    Relative throttle position: " << isCommandAvailable(Requests::RELATIVE_THROTTLE_POSITION::toString()) << std::endl;
    std::cout << "    Engine fuel rate: " << isCommandAvailable(Requests::ENGINE_FUEL_RATE::toString()) << std::endl;
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
        get<Requests::ENGINE_FUEL_RATE>(),
    };
}

std::size_t RequestHandler::availableData() {
    int available = 0;

    // Convert the Boost.Asio serial port to a native handle
    int fd = serial.native_handle();

    // Use ioctl to check the number of bytes available for reading
    if (ioctl(fd, FIONREAD, &available) < 0) {
        std::cerr << "Error in ioctl: " << strerror(errno) << std::endl;
        return 0;
    }

    return static_cast<std::size_t>(available);
}

std::string RequestHandler::readAllAvailableData() {
    std::string result;
    std::size_t available = availableData();

    if (available > 0) {
        std::vector<char> buffer(available);

        // Read the available data from the serial port
        boost::asio::read(serial, boost::asio::buffer(buffer, available));

        result.assign(buffer.begin(), buffer.end());
    }

    return result;
}

std::string RequestHandler::request(const std::string& rawCommand) {
    if (!isCommandAvailable(rawCommand)) {
        return "";
    }
    std::string command = rawCommand + Requests::requestDelimiter;
    boost::asio::write(serial, boost::asio::buffer(command));

    boost::asio::read_until(serial, inputBuffer, Requests::responseDelimiter);

    std::istream inputStream(&inputBuffer);
    std::string rawResponse;
    std::getline(inputStream, rawResponse);

    // std::string rawResponse = readAllAvailableData();

    std::string response;
    // Strip garbage
    std::ranges::copy_if(rawResponse, std::back_inserter(response),
        [](unsigned char c) { return std::isalnum(c); });
    // std::cout << "--- Raw response ---" << std::endl << response << std::endl << "--- End of raw response ---" << std::endl;
    return response;
}

bool RequestHandler::isCommandAvailable(const std::string& command) const {
    return availableCommands.contains(command) && availableCommands.at(command);
}