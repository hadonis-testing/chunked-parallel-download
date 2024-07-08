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

int main()
{
	httplib::Server svr;

	svr.Get("/info", handle_get_info);

	svr.listen(SERVER, PORT);

    return EXIT_SUCCESS;
}