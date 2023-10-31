#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main() {
    struct termios orig_termios, new_termios;

    // Get the current terminal settings
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        perror("tcgetattr");
        exit(1);
    }

    // Copy the original settings
    new_termios = orig_termios;

    // Set the terminal to non-canonical mode
    new_termios.c_lflag &= ~(ICANON | ECHO);

    // Apply the new settings
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios) == -1) {
        perror("tcsetattr");
        exit(1);
    }

    // Read input character by character until a specific key is pressed (e.g., 'q' to quit)
    char c;
    while (1) {
        if (read(STDIN_FILENO, &c, 1) == -1) {
            perror("read");
            break;
        }

        if (c == 'q') {
            break; // Quit the loop when 'q' is pressed
        }
    }

    // Restore the original terminal settings
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        perror("tcsetattr");
        exit(1);
    }

    return 0;
}
