#include <iostream>
#include <sqlite3.h>
#include <string>
using namespace std;

sqlite3* db;

void createDatabase() {
    int exit = sqlite3_open("social_network.db", &db);
    if (exit) {
        cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Database opened successfully!" << endl;
    }

    string sql_users = "CREATE TABLE IF NOT EXISTS users ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "username TEXT UNIQUE, "
                            "password TEXT);";

    string sql_posts = "CREATE TABLE IF NOT EXISTS posts ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "user_id INTEGER, "
                            "content TEXT, "
                            "FOREIGN KEY(user_id) REFERENCES users(id));";

    sqlite3_exec(db, sql_users.c_str(), nullptr, nullptr, nullptr);
    sqlite3_exec(db, sql_posts.c_str(), nullptr, nullptr, nullptr);
}


bool registerUser(const string& username, const string& password) {
    string sql = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "');";
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK) {
        cout << "User registered successfully!\n";
        return true;
    }
    cout << "Registration failed. Username might be taken.\n";
    return false;
}

bool loginUser(const string& username, const string& password) {
    string sql = "SELECT * FROM users WHERE username='" + username + "' AND password='" + password + "';";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "Login successful!\n";
            sqlite3_finalize(stmt);
            return true;
        }
    }
    
    cout << "Invalid username or password.\n";
    return false;
}
bool createPost(int userID, const string& content) {
    string sql = "INSERT INTO posts (user_id, content) VALUES (" + to_string(userID) + ", '" + content + "');";
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK) {
        cout << "Post shared successfully!\n";
        return true;
    }
    cout << "Failed to share post.\n";
    return false;
}

void showPosts() {
    string sql = "SELECT users.username, posts.content FROM posts JOIN users ON users.id = posts.user_id;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << sqlite3_column_text(stmt, 0) << ": " << sqlite3_column_text(stmt, 1) << endl;
        }
    }
    sqlite3_finalize(stmt);
}

int main() {
    createDatabase();

    int choice;
    string username, password, content;
    int userID = 1; // Temporary user ID for demo

    while (true) {
        cout << "\n1. Register\n2. Login\n3. Post Content\n4. View Posts\n5. Exit\nEnter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "Enter username: ";
                getline(cin, username);
                cout << "Enter password: ";
                getline(cin, password);
                registerUser(username, password);
                break;

            case 2:
                cout << "Enter username: ";
                getline(cin, username);
                cout << "Enter password: ";
                getline(cin, password);
                if (loginUser(username, password)) userID = 1; // Assign ID (Would be dynamic in real app)
                break;

            case 3:
                cout << "Enter your post: ";
                getline(cin, content);
                createPost(userID, content);
                break;

            case 4:
                showPosts();
                break;

            case 5:
                sqlite3_close(db);
                return 0;

            default:
                cout << "Invalid choice!\n";
        }
    }
}
