#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

const char* get_file_extension(char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

uint16_t char_to_port(char *port_nb) {
    char *output;
    long port = strtol(port_nb, &output, 10);

    return htons((uint16_t) port); // htons reverse hexa value of port number
}