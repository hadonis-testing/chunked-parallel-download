#include <iostream>
#include <httplib.h>

#define SERVER "localhost"
#define PORT 8080
#define FILE_FOLDER "files/"

void handle_get_info(const httplib::Request& req, httplib::Response& res)
{
	const std::string file_name =  req.get_param_value("file");
	const std::string file_path = FILE_FOLDER + file_name;

	std::cout << "GET /info - " << file_name << std::endl;

	std::ifstream file(file_path, std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "File not found: " << file_name << std::endl;

		res.status = 404;
		res.set_content("File not found", "text/plain");
		return;
	}

	int content_length = 0;
	char c;
	while (file.get(c))
	{
		content_length++;
	}

	file.close();

	res.set_content("", "");
	res.set_header("Accept-Ranges", "bytes");
	res.set_header("Content-Length", std::to_string(content_length));
	res.set_header("Content-Type", "application/octet-stream");

	std::cout << "Content-Length: " << content_length << std::endl;
}

void handle_get_download(const httplib::Request& req, httplib::Response& res)
{
    const std::string file_name = req.get_param_value("file");
    const std::string file_path = FILE_FOLDER + file_name;

    std::cout << "GET /download - " << file_name << std::endl;

    std::ifstream file(file_path, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "File not found: " << file_name << std::endl;

        res.status = 404;
        res.set_content("File not found", "text/plain");
        return;
    }

    std::streampos start = 0;
    std::streampos end = 0;
    std::string range = req.get_header_value("Range");

    if (!range.empty())
    {
        if (sscanf(range.c_str(), "bytes=%lld-%lld", &start, &end) != 2)
        {
            res.status = 400;
            res.set_content("Invalid Range header format", "text/plain");
            file.close();
            return;
        }

        file.seekg(0, std::ios::end);
        std::streampos file_size = file.tellg();

        if (start >= file_size || (end != 0 && end >= file_size))
        {
            res.status = 416;
            res.set_content("Range Not Satisfiable", "text/plain");
            file.close();
            return;
        }

        if (end == 0 || end >= file_size)
        {
            end = file_size - static_cast<std::streampos>(1);
        }

        file.seekg(start);

        std::streampos content_length = (end - start + 1);

        res.status = 206;
        res.set_header("Content-Range", "bytes " + std::to_string(start) + "-" + std::to_string(end));
        res.set_header("Accept-Ranges", "bytes");

        std::vector<char> buffer(content_length);
        file.read(buffer.data(), content_length);
        res.set_content(buffer.data(), buffer.size(), "application/octet-stream");

        std::cout << "Content-Range: " << start << "-" << end << std::endl;

        file.close();
    }
}

int main()
{
	httplib::Server svr;

	svr.Get("/info", handle_get_info);

	svr.Get("/download", handle_get_download);

    std::cout << "Server started at " << SERVER << ":" << PORT << std::endl;

	svr.listen(SERVER, PORT);

    return EXIT_SUCCESS;
}