#include <iostream>
#include <httplib.h>

int get_content_length(httplib::Client& cli, const std::string& file_name)
{
    auto res = cli.Head("/info?file=" + file_name);

    return std::atoi(res->get_header_value("Content-Length").c_str());
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: client.exe <file name> <number of chunk>" << std::endl;

        return EXIT_FAILURE;
    }

    const std::string file_name = argv[1];
    const int number_of_chunk = std::atoi(argv[2]);

    httplib::Client cli("localhost", 8080);

    int length = get_content_length(cli, file_name);

    return EXIT_SUCCESS;
}