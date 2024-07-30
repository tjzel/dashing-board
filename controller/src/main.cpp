#include <iostream>
#include <string>
#include <map>
#include <boost/asio.hpp>
#include <algorithm>
#include <cctype>
#include "requests.h"
#include "parser.h"

class DataFrame {
private:
    double engineLoad;
    int engineRPM;
    int vehicleSpeed;
    double throttlePosition;
    int uptime;

    double fuelLevel;

    double absoluteLoad;
    double relativeThrottlePosition;
    double engineFuelRate;
public:
    DataFrame(const double engineLoad, const int engineRPM, const int vehicleSpeed, const double throttlePosition, const int uptime, const double fuelLevel, const double absoluteLoad, const double relativeThrottlePosition, const double engineFuelRate) : engineLoad(engineLoad), engineRPM(engineRPM), vehicleSpeed(vehicleSpeed), throttlePosition(throttlePosition), uptime(uptime), fuelLevel(fuelLevel), absoluteLoad(absoluteLoad), relativeThrottlePosition(relativeThrottlePosition), engineFuelRate(engineFuelRate) { }

    [[nodiscard]] std::string toString() const {
        std::stringstream ss;
        ss << "Data frame:" << "\n";
        ss << "    Engine load: " << engineLoad << "\n";
        ss << "    Engine RPM: " << engineRPM << "\n";
        ss << "    Vehicle speed: " << vehicleSpeed << "\n";
        ss << "    Throttle position: " << throttlePosition << "\n";
        ss << "    Uptime: " << uptime << "\n";
        ss << "    Fuel level: " << fuelLevel << "\n";
        ss << "    Absolute load: " << absoluteLoad << "\n";
        ss << "    Relative throttle position: " << relativeThrottlePosition << "\n";
        ss << "    Engine fuel rate: " << engineFuelRate << "\n";
        return ss.str();
    }
};

class OBD2Handler {
public:
    explicit OBD2Handler(const std::string& serialPortPath) : io(), serial(boost::asio::serial_port(io, serialPortPath)) {
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
    typename TCommand::Type get() {
        const auto command = TCommand::toString();
        const auto response = request(command);
        return Parser::parse<TCommand>(response);
    }

    void loadAvailableDataCommands() {
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

    void printAvailableCommands() const {
        std::cout << "Available commands:" << std::endl;
        for (const auto& [key, isAvailable] : availableCommands) {
            std::cout << key << ": " << isAvailable << std::endl;
        }
    }

    void printAvailableForDataFrame() const {
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

    DataFrame getDataFrame() {
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

private:
    std::string request(const std::string& rawCommand) {
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

    boost::asio::io_service io;
    boost::asio::serial_port serial;
    boost::asio::streambuf inputBuffer;
    std::map<std::string, bool> availableCommands{ {Requests::RESET::toString(), true}, {Requests::ECHO_OFF::toString(), true}, {Requests::AVAILABLE_DATA_COMMANDS_00_1F::toString(), true} };

    static const std::string requestDelimiter;
    static const std::string responseDelimiter;

};

const std::string OBD2Handler::requestDelimiter = "\r";
const std::string OBD2Handler::responseDelimiter = "\r\r>";



int main(const int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <serial port>" << std::endl;
        return 1;
    }
    const std::string serialPort = argv[1];
    std::cout << "Using serial port: " << serialPort << std::endl;

    OBD2Handler handler(serialPort);

    handler.printAvailableForDataFrame();

    std::cout << "Requesting data frame" << std::endl;
    const auto dataFrame = handler.getDataFrame();
    std::cout << dataFrame.toString() << std::endl;

    return 0;
}