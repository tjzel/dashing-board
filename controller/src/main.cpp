#include <iostream>
#include <string>
#include <map>
#include <format>
#include <boost/asio.hpp>

typedef const std::string OBD2Command;

class Requests {
public:
    static OBD2Command RESET;
    static OBD2Command ECHO_OFF;
    static OBD2Command ENGINE_RPM;
    static OBD2Command AVAILABLE_DATA_COMMANDS;
private:
    Requests() = default;
};

OBD2Command Requests::RESET = "ATZ";
OBD2Command Requests::ECHO_OFF = "ATE0";
OBD2Command Requests::ENGINE_RPM = "010C";
OBD2Command Requests::AVAILABLE_DATA_COMMANDS = "0100";

class OBD2Handler {
public:
    explicit OBD2Handler(const std::string& serialPortPath) : io(), serial(boost::asio::serial_port(io, serialPortPath)) {
        serial.set_option(boost::asio::serial_port_base::baud_rate(38400));
        serial.set_option(boost::asio::serial_port_base::character_size(8));
        serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

        reset();
        loadAvailableDataCommands();
    }

public:
    std::string reset() {
        // For some reason this command is always echoed back.
        auto response = request(Requests::RESET);
        auto commandLength = Requests::RESET.size() + requestDelimiter.size();
        response = response.substr(commandLength, response.size() - commandLength);
        return response;
    }

    std::string echoOff() {
        // Disabled by default.
        return request(Requests::ECHO_OFF);
    }

    int parseRPM(const std::string& response) {
        int RPM = -1;
        try {
            if (response.size() >= 16) {
                const auto A = std::stoi(response.substr(11, 2), nullptr, 16);
                const auto B = std::stoi(response.substr(14, 2), nullptr, 16);
                RPM = ((A * 256) + B) / 4;
            }
        }
        catch (...) { }
        return RPM;
    }

    int getRPM() {
        const auto response = request(Requests::ENGINE_RPM);
        return parseRPM(response);
    }

    void loadAvailableDataCommands() {
        const auto response = request(Requests::AVAILABLE_DATA_COMMANDS);
        const std::string startSequence = "41 00";
        auto startIndex = response.find(startSequence) + startSequence.size();
        if (startIndex < 5 || startIndex + 4 * 3 >= response.size()) {
            return;
        }

        for (int i = 0; i < 4; i++) {
            const auto A = std::stoi(response.substr(startIndex + i * 3, 2), nullptr, 16);
            std::cout << "A: " << A << std::endl;
            for (int j = 0; j < 8; j++) {
                const auto key = std::format("{:0{}X}", i * 8 + j, 2);
                availableCommands[key] = A & (1 << (7 - j));
            }

        }
    }

    void printAvailableCommands() {
        std::cout << "Available commands:" << std::endl;
        for (auto& i : availableCommands) {
            std::cout << i.first << ": " << i.second << std::endl;
        }
    }

private:
    std::string request(OBD2Command& rawCommand) {
        if (availableCommands.find(rawCommand) == availableCommands.end()) {
            return "";
        }
        std::string command = rawCommand + requestDelimiter;
        write(serial, boost::asio::buffer(command));

        auto responseLength = read_until(serial, inputBuffer, responseDelimiter);
        std::istream inputStream(&inputBuffer);
        std::string response;
        std::getline(inputStream, response);
        response = response.substr(0, responseLength - responseDelimiter.size());
        for (char& i : response) {
            if (i == '\r') {
                i = '\n';
            }
        }
        std::cout << "--- Raw response ---" << std::endl << response << std::endl << "--- End of raw response ---" << std::endl;
        return response;
    }

    boost::asio::io_service io;
    boost::asio::serial_port serial;
    boost::asio::streambuf inputBuffer;
    std::map<std::string, bool> availableCommands{ (std::make_pair(Requests::AVAILABLE_DATA_COMMANDS, true)) };

    static const std::string requestDelimiter;
    static const std::string responseDelimiter;
};

const std::string OBD2Handler::requestDelimiter = "\r";
const std::string OBD2Handler::responseDelimiter = "\r\r>";

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <serial port>" << std::endl;
        return 1;
    }
    const std::string serialPort = argv[1];
    std::cout << "Using serial port: " << serialPort << std::endl;
    OBD2Handler handler(serialPort);
    std::cout << "Requesting RPM" << std::endl;
    std::cout << "Response: " << std::endl << handler.getRPM() << std::endl;
    handler.printAvailableCommands();

    return 0;
}