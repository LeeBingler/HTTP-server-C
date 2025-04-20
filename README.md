## HTTP-server-C

A lightweight HTTP server built from scratch in C, designed for educational purposes and to demonstrate fundamental networking concepts.

## Features

- Handles basic HTTP GET requests
- Serves static files from a specified directory
- Implements minimal HTTP request parsing
- Provides appropriate HTTP response headers

## Getting Started

### Prerequisites

- GCC compiler
- Make utility
- POSIX-compliant operating system (e.g., Linux, macOS)

### Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/LeeBingler/HTTP-server-C.git
   cd HTTP-server-C
   ```

2. Build the project

    ```bash
    make
    ```

This will compile the source files and generate the executable.

### Usage

    Usage: my-server [OPTIONS]
    Open a web server on PORT with ROOT_DIR as root.
    Options:\n"
        -p, --port [PORT]       Specify the port number (default: 8080)
        -r, --root [ROOT_DIR]   Set the root directory (default: ./)
        --help                  Show this help message
    Examples:
        ./my-server -p 8080 -r ./www/

## Project Structure

- src/: Contains the source code files.

- include/: Contains header files.

- Makefile: Build configuration.

- README.md: Project documentation.​

## License

This project is licensed under the MIT License. See the LICENSE file for details.