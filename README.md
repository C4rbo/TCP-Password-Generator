# TCP Password Generator - Client/Server Application

This project is a simple TCP client/server application where the server acts as a password generator, responding to client requests with passwords generated according to specified criteria.

## Features

- The client automatically connects to the server using a predefined IP address and port.
- Upon connection, the server logs the client’s IP address and port (e.g., `New connection from xxx.xxx.xxx.xxx:yyyyy`).
- The client can request different types of passwords:
  - `n`: Numeric password (only digits).
  - `a`: Alphabetic password (lowercase letters only).
  - `m`: Mixed password (lowercase letters and digits).
  - `s`: Secure password (uppercase letters, lowercase letters, digits, and symbols).
- The client specifies the password type and length in a single input, e.g., `n 8` for an 8-character numeric password.
- The server generates the requested password and sends it back to the client, which then displays it.
- The client can continue making password requests or terminate the connection by entering `q`.

## Requirements

- A shared application protocol is defined in a `.h` header file.
- Password generation functions:
  - `generate_numeric()`
  - `generate_alpha()`
  - `generate_mixed()`
  - `generate_secure()`
- Input for password requests must be entered as a single line (e.g., `n 8`).
- Default IP address and port are used for both client and server, requiring no command-line arguments at startup.
- Password length must be between 6 and 32 characters.
- Code and comments are written in English.

## Technical Details

- The server can handle up to 5 simultaneous client connections (queue length: 5).
- The server runs indefinitely, waiting for new connections.
- The client terminates its process after disconnecting from the server using the `q` command.

## How to Run

1. Start the server:
   ```cmd
   server.exe
   ```
2. Start the client:
   ```cmd
   client.exe
   ```
3. Follow the client prompts to request passwords.

## Example Interaction

1. The client connects to the server:
   ```
   Connected to the server at xxx.xxx.xxx.xxx:port
   ```
2. The client sends a request:
   ```
   Input: n 8
   ```
3. The server responds:
   ```
   Password: 45237891
   ```
4. The client can make another request or disconnect using `q`:
   ```
   Input: q
   Disconnected from the server.
   ```

This program was developed using **Eclipse** and tested on a Windows environment.
## Legal Disclaimer

**Use responsibly and legally.** This tool is designed for educational purposes and authorized security testing only. Unauthorized scanning of networks and devices is illegal and unethical. Always obtain explicit permission from the network owner before using this tool.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

## Contributing

Contributions are welcome! If you have suggestions for improvements or find bugs, please open an issue or submit a pull request on GitHub.

## Author

- C4rbo (https://github.com/C4rbo)
```