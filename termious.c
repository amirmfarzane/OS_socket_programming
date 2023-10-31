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

    // Set an alarm for 5 seconds (adjust as needed)
    alarm(5);

    // Lock the input
    input_locked = 1;
    printf("Input is locked for 5 seconds...\n");

    // Set the signal handler for the alarm signal
    signal(SIGALRM, alarm_handler);

    char c;
    while (input_locked) {
        if (read(STDIN_FILENO, &c, 1) == -1) {
            perror("read");
            exit(1);
        }
        // You can add code here to process the input, if needed.
    }

    // Restore the original terminal settings
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        perror("tcsetattr");
        exit(1);
    }
