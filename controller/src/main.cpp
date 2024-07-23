#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

// Function to flush the serial port buffer
void flush_serial_buffer(serial_port& serial) {
    boost::asio::streambuf buf;
    boost::system::error_code ec;

    // Read until no more data is available
    while (boost::asio::read(serial, buf.prepare(256), transfer_at_least(1), ec)) {
        buf.consume(buf.size());
    }
}

void write_command(serial_port& serial, const std::string& cmd) {
    std::string delimiter = "\r";
    write(serial, buffer(cmd + delimiter));
}

std::string read_response(serial_port& serial, const size_t skipFromStart = 0) {
    boost::asio::streambuf buf;
    std::string delimiter = "\r\r>";
    auto delimiter_length = delimiter.size();
    auto lenght = read_until(serial, buf, delimiter);
    std::istream is(&buf);
    std::string response;
    std::getline(is, response);
    return response.substr(skipFromStart, lenght - delimiter_length - skipFromStart);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <serial port>" << std::endl;
        return 1;
    }
    const std::string serialPort = argv[1];
    std::cout << "Using serial port: " << serialPort << std::endl;
    try {
        io_service io;
        serial_port serial(io, serialPort);  // Adjust to your device
        serial.set_option(serial_port_base::baud_rate(38400));
        serial.set_option(serial_port_base::character_size(8));
        serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
        serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));

        // flush_serial_buffer(serial);
        // Initialize OBD-II communication
        std::cout << "Sending ATZ" << std::endl;
        write_command(serial, "ATZ");  // Reset
        std::cout << "Response: " << read_response(serial, 4) << std::endl;
        // std::cout << "Sending ATE0" << std::endl;
        // write_command(serial, "ATE0");  // Echo off
        // std::cout << "Response: " << read_response(serial) << std::endl;

        // Request engine RPM
        std::cout << "Sending 010C" << std::endl;
        write_command(serial, "010C");
        std::string response = read_response(serial, 5);

        // Process response (assuming form 41 0C XX YY)
        const std::string substr = response.substr(0, 4);
        if (response.substr(0, 4) == "41 0C") {
            int A = std::stoi(response.substr(5, 2), nullptr, 16);
            int B = std::stoi(response.substr(8, 2), nullptr, 16);
            int rpm = ((A * 256) + B) / 4;
            std::cout << "RPM: " << rpm << std::endl;
        }
        else {
            std::cout << "Unexpected response: " << response << std::endl;
        }
    }
    catch (boost::system::system_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}