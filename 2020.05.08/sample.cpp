#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

class Shell {

public:

	Shell() {
		char *cmd = new char[256];
		const char *safe = "safe_program ";
		int returnCode;
		while (1) {
			cout << "Enter command: ";
			cin.getline(cmd, 256);
			if (strncmp(cmd, safe, strlen(safe)) != 0) {
				cout << "Unsafe command entered\n";
				break;
			}
			returnCode = system(cmd);
			cout << "Command returned " << returnCode << '\n';
		}
	}
};

int main() {
	new Shell();
	return 0;
}

