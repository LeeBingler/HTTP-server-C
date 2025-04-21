# HTTP-server-C

A lightweight HTTP server built from scratch in C, designed for educational purposes and to demonstrate fundamental networking concepts.

## Features

- Handles basic HTTP GET, POST, PUT, DELETE, HEAD requests
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

3. Run test

   ```bash
   make test
   ```

### Usage

    Usage: http-server-c [OPTIONS]
    Open a web server on PORT with ROOT_DIR as root.
    Options:\n"
        -p, --port [PORT]       Specify the port number (default: 8080)
        -r, --root [ROOT_DIR]   Set the root directory (default: ./)
        --help                  Show this help message
    Examples:
        ./my-server -p 8080 -r ./www/

##  Project Structure

```sh
└── HTTP-server-C/
    ├── LICENSE
    ├── Makefile
    ├── README.md
    ├── include
    │   ├── config.h
    │   ├── core
    │   ├── http
    │   ├── request
    │   └── utils
    ├── src
    │   ├── core
    │   ├── http
    │   ├── request
    │   └── utils
    └── tests
        ├── integration
        └── run_tests.sh
```

## License

This project is licensed under the MIT License. See the LICENSE file for details.