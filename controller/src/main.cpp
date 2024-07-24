#include <iostream>
#include <string>
#include <boost/asio.hpp>

typedef const std::string OBD2Command;

class Requests {
public:
    static OBD2Command RESET;
    static OBD2Command ECHO_OFF;
    static OBD2Command ENGINE_RPM;
private:
    Requests() = default;
};

OBD2Command Requests::RESET = "ATZ";
OBD2Command Requests::ECHO_OFF = "ATE0";
OBD2Command Requests::ENGINE_RPM = "010C";

class OBD2Handler {
public:
    explicit OBD2Handler(const std::string& serialPortPath) : io(), serial(boost::asio::serial_port(io, serialPortPath)) {
        serial.set_option(boost::asio::serial_port_base::baud_rate(38400));
        serial.set_option(boost::asio::serial_port_base::character_size(8));
        serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    }

public:
    std::string reset() {
        // For some reason the command is always echoed back
        auto response = request(Requests::RESET);
        auto commandLength = Requests::RESET.size() + requestDelimiter.size();
        response = response.substr(commandLength, response.size() - commandLength);
        return response;
    }

    std::string echoOff() {
        return request(Requests::ECHO_OFF);
    }

    int getRPM() {
        const auto response = request(Requests::ENGINE_RPM);
        int RPM;
        try {
            const auto A = std::stoi(response.substr(11, 2), nullptr, 16);
            const auto B = std::stoi(response.substr(14, 2), nullptr, 16);
            RPM = ((A * 256) + B) / 4;
        }
        catch (...) {
            RPM = -1;
        }
        return RPM;
    }

private:
    std::string request(OBD2Command& rawCommand) {
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
        return response;
    }

    boost::asio::io_service io;
    boost::asio::serial_port serial;
    boost::asio::streambuf inputBuffer;
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
    std::cout << "Resetting" << std::endl;
    std::cout << "Response: " << handler.reset() << std::endl;
    std::cout << "Requesting RPM" << std::endl;
    std::cout << "Response: " << handler.getRPM() << std::endl;

    return 0;
}