#include <iostream>
#include <cmath>
#include <httplib.h>

int get_content_length(httplib::Client& cli, const std::string& file_name)
{
	auto res = cli.Head("/info?file=" + file_name);

	return std::atoi(res->get_header_value("Content-Length").c_str());
}

void download_chunk(httplib::Client& cli, const std::string& file_name, int start, int end, const std::string& temp_file_name)
{
	std::string range = std::to_string(start) + "-" + std::to_string(end);
	auto res = cli.Get("/download?file=" + file_name, { {"Range", "bytes=" + range} });

	if (res && res->status == 206)
	{
		std::ofstream temp_file(temp_file_name, std::ios::out | std::ios::binary | std::ios::ate);
		if (!temp_file)
		{
			std::cerr << "Thread: Failed to open temporary file for writing." << std::endl;
			return;
		}

		temp_file.write(res->body.data(), res->body.size());

		temp_file.close();

		std::cout << "Thread: Chunk " << start << " - " << end << " downloaded and saved to '" << temp_file_name << "'." << std::endl;
	}
	else
	{
		std::cerr << "Thread: Failed to download chunk " << start << " - " << end << ". Status code: " << (res ? res->status : -1) << std::endl;
	}
}

void merge_files(const std::vector<std::string>& temp_file_names, const std::string& output_file_name)
{
	std::ofstream output_file(output_file_name, std::ios::out | std::ios::binary);
	if (!output_file) {
		std::cerr << "Failed to open output file for writing." << std::endl;
		return;
	}

	for (const auto& temp_file_name : temp_file_names)
	{
		std::ifstream temp_file(temp_file_name, std::ios::in | std::ios::binary);
		if (!temp_file)
		{
			std::cerr << "Failed to open temporary file '" << temp_file_name << "' for reading." << std::endl;
			continue;
		}

		output_file << temp_file.rdbuf();

		temp_file.close();

		std::remove(temp_file_name.c_str());
	}

	output_file.close();

	std::cout << "Merged file saved as '" << output_file_name << "'." << std::endl;
}

void download_chunked_file_parallel(httplib::Client& cli, const std::string& file_name, int num_chunks, int max_length)
{
	std::vector<std::thread> threads;
	std::string temp_file_name = "temp";
	std::vector<std::string> temp_file_names;

	int chunk_size = std::ceil(max_length / (double)num_chunks);

	for (int i = 0; i < num_chunks; ++i)
	{
		int start = i * chunk_size;
		int end = (i + 1) * chunk_size - 1;
		end = std::min(end, max_length - 1);

		temp_file_names.push_back(temp_file_name + std::to_string(i));

		threads.emplace_back(std::thread(download_chunk, std::ref(cli), file_name, start, end, temp_file_name + std::to_string(i)));
	}

	for (auto& thread : threads)
	{
		thread.join();
	}

	merge_files(temp_file_names, file_name);

	std::cout << "All chunks downloaded." << std::endl;
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

	download_chunked_file_parallel(cli, file_name, number_of_chunk, length);

	return EXIT_SUCCESS;
}