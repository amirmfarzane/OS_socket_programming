#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void unlockInput(int signum) {
    // Restore the original terminal settings
    struct termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

    printf("Input unlocked!\n");
    exit(0);
}

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

    // Set an alarm for 5 seconds
    alarm(5);

    // Set up a signal handler for SIGALRM
    signal(SIGALRM, unlockInput);

    printf("Input is locked for 5 seconds...\n");

    // Read input character by character
    char c;
    while (1) {
        if (read(STDIN_FILENO, &c, 1) == -1) {
            perror("read");
            break;
        }
        // Do something with the input, if needed
    }

    return 0;
}
