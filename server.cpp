//----------- Student Information Management System -----------//

//-----------Server Side------------------//

#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <algorithm>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

// Student structure to store information on the student
struct Student {
    int id;
    std::string name;
    std::string course;

    // only 2 Characters are allowed for course
    bool isValid() const {
        return course.length() == 2;
    }
};

// linked list 
std::list<Student> students;

// fetching data from file to temp memory
void loadFromFile() {
    std::ifstream infile("students.dat");
    if (infile) {
        Student student;
        while (infile >> student.id >> student.name >> student.course) {
            students.push_back(student);
        }
    }
}

// commiting changes in the file
void saveToFile() {
    std::ofstream outfile("students.dat");
    for (const auto& student : students) {
        outfile << student.id << " " << student.name << " " << student.course << "\n";
    }
    std::cout << "Data committed to file.\n";
}

// networking 
void handleClient(int client_socket) {
    char buffer[1024];
    int n;
    while ((n = read(client_socket, buffer, sizeof(buffer))) > 0) {
        buffer[n] = '\0';
        cout << "Listening !!"<<endl;
        std::string request(buffer);

        std::istringstream iss(request);
        std::string command;
        iss >> command;

        // Adding new Student
        if (command == "ADD") {
            Student student;
            iss >> student.id >> student.name >> student.course;
            if (student.isValid()) {
                students.push_back(student);
                std::string response = "Student added successfully.\n";
                write(client_socket, response.c_str(), response.length());
            } else {
                std::string response = "Invalid course code. Must be 2 characters.\n";
                write(client_socket, response.c_str(), response.length());
            }
        } 
        
        // Updating the info of the existing student
        else if (command == "UPDATE") {
            int id;
            iss >> id;
            auto it = std::find_if(students.begin(), students.end(), [id](const Student& s) { return s.id == id; });
            if (it != students.end()) {
                Student& student = *it;
                iss >> student.name >> student.course;
                if (student.isValid()) {
                    std::string response = "Student updated successfully.\n";
                    write(client_socket, response.c_str(), response.length());
                } else {
                    std::string response = "Invalid course code. Must be 2 characters.\n";
                    write(client_socket, response.c_str(), response.length());
                }
            } else {
                std::string response = "Student not found.\n";
                write(client_socket, response.c_str(), response.length());
            }
        }
        
        // Deleting Request
         else if (command == "DELETE") {
            int id;
            iss >> id;
            auto it = std::find_if(students.begin(), students.end(), [id](const Student& s) { return s.id == id; });
            if (it != students.end()) {
                students.erase(it);
                std::string response = "Student deleted successfully.\n";
                write(client_socket, response.c_str(), response.length());
            } else {
                std::string response = "Student not found.\n";
                write(client_socket, response.c_str(), response.length());
            }
        } 
        
        // Display all the students
        else if (command == "DISPLAY") {
            std::ostringstream oss;
            for (const auto& student : students) {
                oss << "ID: " << student.id << ", Name: " << student.name << ", Course: " << student.course << "\n";
            }
            std::string response = oss.str();
            write(client_socket, response.c_str(), response.length());
        } 
        
        // commiting changes to the file
        else if (command == "COMMIT") {
            saveToFile();
            std::string response = "Data committed to file.\n";
            write(client_socket, response.c_str(), response.length());
        }
    }
    close(client_socket);
}

// driver code
int main() {
    loadFromFile();

    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while ((client_socket = accept(server_fd, (struct sockaddr )&address, (socklen_t)&addrlen)) >= 0) {
        handleClient(client_socket);
    }

    close(server_fd);
    return 0;
}
