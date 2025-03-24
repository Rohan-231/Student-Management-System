#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

using namespace std;

void sendRequest(int sock, const string& request) {
    send(sock, request.c_str(), request.size(), 0);
    char buffer[1024] = {0};
    int valread = read(sock, buffer, sizeof(buffer));
    if (valread > 0) {
        cout << "Response from server:\n" << buffer << endl;
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Creating socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Socket creation error\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address/ Address not supported\n";
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed\n";
        return -1;
    }

    cout << "Connected to the server.\n";

    while (true) {
        cout << "\nChoose an operation: \n";
        cout << "1. ADD\n2. UPDATE\n3. DELETE\n4. DISPLAY\n5. COMMIT\n6. EXIT\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        string request;
        switch (choice) {
            case 1: {
                // ADD
                int id;
                string name, course;
                cout << "Enter ID: ";
                cin >> id;
                cout << "Enter Name: ";
                cin >> name;
                cout << "Enter Course (2 characters): ";
                cin >> course;
                request = "ADD " + to_string(id) + " " + name + " " + course + "\n";
                break;
            }
            case 2: {
                // UPDATE
                int id;
                string name, course;
                cout << "Enter ID to update: ";
                cin >> id;
                cout << "Enter New Name: ";
                cin >> name;
                cout << "Enter New Course (2 characters): ";
                cin >> course;
                request = "UPDATE " + to_string(id) + " " + name + " " + course + "\n";
                break;
            }
            case 3: {
                // DELETE
                int id;
                cout << "Enter ID to delete: ";
                cin >> id;
                request = "DELETE " + to_string(id) + "\n";
                break;
            }
            case 4:
                // DISPLAY
                request = "DISPLAY\n";
                break;
            case 5:
                // COMMIT
                request = "COMMIT\n";
                break;
            case 6:
                // EXIT
                cout << "Exiting client.\n";
                close(sock);
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
                continue;
        }

        // Send the request to the server
        sendRequest(sock, request);
    }

    return 0;
}
