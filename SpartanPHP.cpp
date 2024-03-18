#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>
#include <vector>
#include <sstream>
#include <regex>
#include <Windows.h>
#include <conio.h>
#include <experimental/filesystem>
#include <nlohmann/json.hpp>
#include <color.hpp>
#include <base64.h>

using namespace std;

namespace fs = std::experimental::filesystem;

class encode_data
{
public:
	string code, path_project;
	bool is_completed = false;
	encode_data(const string& code, const string& path_project = "") : code(code), path_project(path_project) {}

	void set_completed(bool value) {
		is_completed = value;
	}
};

int _hard_crypt = 1000;
int _char_encode = 123;
int _quality_runtime = 5;
string _path_project, _path_environment;
string _prefix_title = "SpartanPHP";
vector<encode_data> _encode_files;

void set_title(const string& title) {
	const string real_title = title;
	size_t newsize = strlen(real_title.c_str()) + 1;
	wchar_t* wtitle = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wtitle, newsize, title.c_str(), _TRUNCATE);

	SetConsoleTitle(wtitle);
	
	delete[] wtitle;
}

string str_replace(const string& str, const string& from, const string& to) {
	string result = str;
	size_t start_pos = 0;
	while ((start_pos = result.find(from, start_pos)) != string::npos) {
		result.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return result;
}

std::vector<std::string> explode(std::string const & s, char delim) {
	std::vector<std::string> result;
	istringstream iss(s);

	for (std::string token; std::getline(iss, token, delim); ) result.push_back(std::move(token));

	return result;
}

string remove_substring(string main_str, const string &start_str, const string &end_str) {
	size_t start_pos = main_str.find(start_str);
	if (start_pos != string::npos) {
		size_t end_pos = main_str.find(end_str, start_pos + start_str.length());
		if (end_pos != string::npos) main_str.erase(start_pos, end_pos + end_str.length() - start_pos);
	}

	return main_str;
}

std::vector<char> a;
std::vector<char> d;

std::string escape(const std::string& str) {
	std::string result = str;
	size_t pos = 0;

	while ((pos = result.find_first_of("\\\"$", pos)) != std::string::npos) {
		result.insert(pos, "\\");
		pos += 2;
	}

	return result;
}

std::string generator_crypt(int n) {
	std::string result;
	std::srand(0);

	for (int i = 0; i < n; ++i) {
		char ch = (std::rand() % 2) ? a[std::rand() % a.size()] : d[std::rand() % d.size()];
		result.push_back(ch);
	}

	return escape(result);
}

std::string encode_string(const std::string& input) {
	std::srand(0);
	return generator_crypt(input.length());
}

void initialize_vectors() {
	for (int i = 0; i <= 31; ++i) a.push_back(static_cast<char>(i));
	for (int i = 32; i <= 255; ++i) d.push_back(static_cast<char>(i));
}

string date_time_to_string() {
	time_t now = time(0);
	tm* ltm = localtime(&now);
	return to_string(1 + ltm->tm_mon) + "/" + to_string(ltm->tm_mday) + "/" + to_string(1900 + ltm->tm_year) + " " + to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec);
}

string encode(const string& content) {
	string encoded_content;
	for (const char c : content) encoded_content += (char)(c + _char_encode);
	return encoded_content;
}

string decode(const string& content) {
	string decoded_content;
	for (const char c : content) decoded_content += (char)(c - _char_encode);
	return decoded_content;
}

string generate_intro_encode() {
	string intro;
	intro += " *\n";
	intro += " * This file is encoded by SpartanPHP\n";
	intro += " * Version: 1.0.0\n";
	intro += " * Author: VennDev\n";
	intro += " *\n";
	return intro;
}

string generate_hash_pass(const string& key, const string& pass) {
	return "\n ${\"" + key + "\"} = \"" + pass + "\";\n\n";
}

string generate_fake_content(const string& content, int length) {
	string fake_content;
	for (int i = 0; i < length; i++) fake_content += " * This is fake content\n";
	fake_content += content;
	return encode(fake_content);
}

string clean_string(const string& string)
{
	// remove new line
	const std::string content_no_newline = str_replace(string, "\n", "");

	// remove space
	//const std::string content_no_space = str_replace(content_no_newline, " ", "");

	// remove tab
	const std::string content_no_tab = str_replace(content_no_newline, "\t", "");

	return content_no_tab;
}

bool is_file_open_by_anotherrogram(const std::string& filename) {
	std::ifstream file(filename.c_str());
	return !file.is_open();
}

void restore_data() {
	const auto start_time = std::chrono::steady_clock::now();
	// 0.5 seconds
	const auto end_time = start_time + std::chrono::seconds(_quality_runtime);

	// This security is to prevent the program other read the file, but i dont certain this is work.
	while (std::chrono::steady_clock::now() < end_time) {
		for (auto& data : _encode_files) {
			if (is_file_open_by_anotherrogram(data.path_project)) {
				cout << "One package is completed!\n";
				data.set_completed(true);
			} else {
				if (data.is_completed == true) {
					std::ofstream restore_file(data.path_project);
					restore_file << data.code;
					restore_file.close();
				}
			}
		}
	}

	for (auto& data : _encode_files) {
		if (data.is_completed == false) {
			std::ofstream restore_file(data.path_project);
			restore_file << data.code;
			restore_file.close();
		}
	}
}

// Encode from code [php] to [ascii]
string process_encode(const string& key, const string& content)
{
	// remove <?php and ?>
	const string content_no_php = str_replace(content, "<?php", "");
	const string content_no_php2 = str_replace(content_no_php, "?>", "");
	
    std::string encoded_content;

	// remove comment
	regex pattern1("/\\*.*?\\*/");
	const string content_no_php3 = regex_replace(content_no_php2, pattern1, "");

	regex pattern2("//.*?$", regex_constants::ECMAScript);
	const string content_no_php4 = regex_replace(content_no_php3, pattern2, "");

	regex pattern3("/\\*\\*.*?\\*/", regex_constants::ECMAScript);
	const string content_no_php5 = regex_replace(content_no_php4, pattern3, "");

	regex pattern4("/\\*\\*.*?\\*\\*/", regex_constants::ECMAScript);
	const string content_no_php6 = regex_replace(content_no_php5, pattern4, "");

	regex pattern5("/\\s*/\\*.*?\\*/", regex_constants::ECMAScript);
	const string content_no_php7 = regex_replace(content_no_php6, pattern5, "");
	
	regex pattern6("/\\s*/\\*\\*.*?\\*/", regex_constants::ECMAScript);
	const string content_no_php8 = regex_replace(content_no_php7, pattern6, "");

	// remove declare strict_types
	std::regex pattern("(declare\\s*\\(\\s*strict_types\\s*[^;]*\\)\\s*;?)");
	std::string content_no_php9 = std::regex_replace(content_no_php8, pattern, "");

	// remove new line
	const std::string content_clean = clean_string(content_no_php9);

	string fake_content = generate_fake_content(key, _hard_crypt);
	string encode_fake_content = encode_string(fake_content);
	string hash_pass = generate_hash_pass(_prefix_title, generate_fake_content(_prefix_title, _hard_crypt));
	const string fake_code_a = "/**\n" + generate_intro_encode() + fake_content + "**/";
	const string fake_code_b = "/**" + fake_content + "**/";
	
    encoded_content = base64_encode(content_clean);
	encoded_content = "eval" + fake_code_b + "(" + fake_code_b + "base64_decode" + fake_code_b + "(\"" + base64_encode(fake_content) + encoded_content + "\"));";
    
    return "<?php \n" + hash_pass + fake_code_a + encoded_content + fake_code_b + " ?>";
}

std::string get_filename_from_path(const std::string& path) {
	std::stringstream ss(path);
	std::string filename;

	while (std::getline(ss, filename, '/')) {
		// do nothing
	}
    
	return filename;
}

// Decode from [ascii] to code [php]
string process_decode(const string& key, const string& content)
{
	std::regex pattern("\\(\"([^\"]+)\"\\)");
	std::smatch matches;

	std::string decoded_content;

	if (std::regex_search(content, matches, pattern)) {
		decoded_content = matches[1].str();
	}
	else
	{
		cerr << "Failed to decode content! please check your code.\n";
		system("pause");
	}

	// check have eval
	if (content.find("/eval/") == std::string::npos || content.find("/base64_decode/") == std::string::npos) {
		cerr << "Failed to decode content! please check your code.\n";
		restore_data();
		system("pause");
	}

	string fake_content = generate_fake_content(key, _hard_crypt);
	string encode_fake_content = encode_string(fake_content);
	string hash_pass = generate_hash_pass(_prefix_title, generate_fake_content(_prefix_title, _hard_crypt));
	const string fake_code_a = "/**\n" + generate_intro_encode() + fake_content + "**/";
	const string fake_code_b = "/**" + fake_content + "**/";
	
	decoded_content = str_replace(decoded_content, base64_encode(fake_content), "");
	decoded_content = "eval" + fake_code_b + "(" + fake_code_b + "base64_decode" + fake_code_b + "(\"" + decoded_content + "\"));";
	
	const string real_code = "<?php \n" + hash_pass + fake_code_a + decoded_content + fake_code_b + " ?>";
    
    return real_code;
}

void run_decode(const std::string& path_project) {
	std::ifstream in_file(path_project);
	std::string encoded_code((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
	in_file.close();

	string final_name = get_filename_from_path(path_project);
	std::string decoded_code = process_decode(final_name, encoded_code);

	std::ifstream original_file(path_project);
	std::string original_code((std::istreambuf_iterator<char>(original_file)), std::istreambuf_iterator<char>());
	original_file.close();

	std::ofstream out_file(path_project);
	out_file << decoded_code;
	out_file.close();

	// save original code to vector
	_encode_files.push_back(encode_data(original_code, path_project));
}

void run_encode(const std::string& path_project) {
	std::ifstream in_file(path_project);
	std::string code((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
	in_file.close();

	string final_name = get_filename_from_path(path_project);
	std::string encoded_code = process_encode(final_name, code);

	std::ofstream out_file(path_project);
	out_file << encoded_code;
	out_file.close();
}

void process_php_file(const fs::directory_entry& entry, const std::function<void(const std::string&)>& content_processor) {
	if (entry.path().extension() == ".php") {
		std::ifstream in_file(entry.path());
		if (in_file.is_open()) {
			in_file.close();
			content_processor(entry.path().string());
		}
		else {
			cerr << "Unable to open file: " << entry.path() << "\n";
		}
	}
}

void run_process(const std::string& path_project, const std::function<void(const std::string&)>& content_processor) {
	for (const auto& entry : fs::directory_iterator(path_project)) {
		if (is_directory(entry.status())) {
			run_process(entry.path().string(), content_processor);
		}
		else if (entry.path().extension() == ".php") {
			process_php_file(entry, content_processor);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

void run_environment() {
	// convert string to wchar_t
	size_t newsize = strlen(_path_environment.c_str()) + 1;
	wchar_t* wcommand = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcommand, newsize, _path_environment.c_str(), _TRUNCATE);

	HINSTANCE result = ShellExecute(NULL, L"open", wcommand, NULL, NULL, SW_SHOWNORMAL);

	if ((int)result <= 32) std::cerr << "Failed to open environment.\n";
	
	delete[] wcommand;
}

std::string get_cmd() {
	std::string cmd;
	char ch;

	while (true) {
		ch = _getch();
		
		if (ch == 13) break;

		// backspace
		if (ch == 8 && cmd.length() > 0) cmd.pop_back();
		
		cmd.push_back(ch);

		set_title(_prefix_title + " - You are typing... '" + cmd + "'");
	}

	return cmd;
}

void show_intro() {
	cout << dye::green("Welcome to SpartanPHP!\n\n");
	cout << dye::yellow("Your settings:\n");
	cout << dye::aqua("- Path project: ") + dye::white(_path_project) + "\n";
	cout << dye::aqua("- Path environment: ") + dye::white(_path_environment) + "\n\n";
}

void on_process() {
	system("cls");

	show_intro();
	
	cout << dye::aqua("What are your choice?\n"); 
	cout << dye::yellow("1 > Run Environment.\n");
	cout << dye::yellow("2 > Run Encode.\n\n");

	short choice = std::stoi(get_cmd());

	auto content_decoded_processor = [](const std::string& path_project) {
		run_decode(path_project);
	};

	auto content_encoded_processor = [](const std::string& path_project) {
		run_encode(path_project);
	};

	switch (choice) {
	case 1:
		std::cout << dye::aqua("Environment is running...\n");
		
		run_process(_path_project, content_decoded_processor);
		run_environment();
		
		cout << dye::yellow("Please wait checking quality runtime... don't close this program!\n");
		restore_data();
		
		break;
	case 2:
		cout << dye::aqua("Running encode...\n");
		
		run_process(_path_project, content_encoded_processor);
		
		std::this_thread::sleep_for(std::chrono::seconds(2));

		cout << dye::green("Encode is done!\n");

		std::this_thread::sleep_for(std::chrono::seconds(1));
		break;
	default:
		std::cerr << dye::red("Please type id your choose true!\n");
	}

	on_process();
}

BOOL CtrlHandler(DWORD fdwCtrlType) {
	switch (fdwCtrlType) {
	case CTRL_CLOSE_EVENT:
		for (auto& data : _encode_files) {
			if (data.is_completed == false) {
				std::ofstream restore_file(data.path_project);
				restore_file << data.code;
				restore_file.close();
			}
		}
		return TRUE;

	default:
		return FALSE;
	}
}

int main()
{
	// set title
	set_title(_prefix_title);

	initialize_vectors();

	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE)) {
		std::cerr << "Failed to set control handler.\n";
		return 1;
	}
	
	string file_settings = "settings.json";

	ifstream file(file_settings);
	bool file_exists = file.good();
	file.close();

	if (!file_exists) {
		ofstream output_file(file_settings);
		if (output_file.is_open()) {
			nlohmann::json json_data;

			json_data["path_environment"] = "start.cmd";
			json_data["path_project"] = "your_path";
			json_data["quality_runtime"] = _quality_runtime;
			json_data["hard_crypt"] = _hard_crypt;

			output_file << setw(4) << json_data;
			output_file.close();

			cout << dye::green("Creating one json data...\n");

			std::this_thread::sleep_for(std::chrono::seconds(1));
			
			cout << dye::aqua("Now you can run this program again!\n");

			system("pause");
		}
		else
		{
			std::cerr << "Failed in creating one json data!\n";
		}
	}
	else
	{
		ifstream input_file(file_settings);
		if (input_file.is_open()) {
			nlohmann::json json_data;
			input_file >> json_data;
			input_file.close();
			if (
				json_data.contains("path_environment") &&
				json_data.contains("path_project") &&
				json_data.contains("quality_runtime") &&
				json_data.contains("hard_crypt")
			) {
				_path_project = json_data["path_project"];
				_path_environment = json_data["path_environment"];
				_quality_runtime = json_data["quality_runtime"];
				_hard_crypt = json_data["hard_crypt"];
			}
		}
		else
		{
			std::cerr << "Failed to open file for reading.\n";
		}
	}

	show_intro();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	on_process();

	system("pause");
}