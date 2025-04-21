#include <strings.h>

const char* get_mime_type(const char *ext) {
    if (!ext) return "application/octet-stream";

    // Texts
    if (strcasecmp(ext, "html") == 0) return "text/html";
    if (strcasecmp(ext, "htm")  == 0) return "text/html";
    if (strcasecmp(ext, "txt")  == 0) return "text/plain";
    if (strcasecmp(ext, "css")  == 0) return "text/css";
    if (strcasecmp(ext, "csv")  == 0) return "text/csv";

    // Images
    if (strcasecmp(ext, "jpg")  == 0) return "image/jpeg";
    if (strcasecmp(ext, "jpeg") == 0) return "image/jpeg";
    if (strcasecmp(ext, "png")  == 0) return "image/png";
    if (strcasecmp(ext, "gif")  == 0) return "image/gif";
    if (strcasecmp(ext, "svg")  == 0) return "image/svg+xml";
    if (strcasecmp(ext, "ico")  == 0) return "image/x-icon";

    // Scripts
    if (strcasecmp(ext, "js")   == 0) return "application/javascript";
    if (strcasecmp(ext, "json") == 0) return "application/json";
    if (strcasecmp(ext, "xml")  == 0) return "application/xml";

    // Fonts
    if (strcasecmp(ext, "woff") == 0) return "font/woff";
    if (strcasecmp(ext, "woff2")== 0) return "font/woff2";
    if (strcasecmp(ext, "ttf")  == 0) return "font/ttf";

    // PDF / documents
    if (strcasecmp(ext, "pdf")  == 0) return "application/pdf";
    if (strcasecmp(ext, "zip")  == 0) return "application/zip";

    return "application/octet-stream"; // default
}