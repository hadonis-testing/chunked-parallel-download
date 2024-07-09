#include <iostream>
#include <httplib.h>

int get_content_length(httplib::Client& cli, const std::string& file_name)
{
    auto res = cli.Head("/info?file=" + file_name);

    return std::atoi(res->get_header_value("Content-Length").c_str());
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <file_name>" << std::endl;

        return EXIT_FAILURE;
    }

    const std::string file_name = argv[1];

    httplib::Client cli("localhost", 8080);

    int length = get_content_length(cli, file_name);

    return EXIT_SUCCESS;
}