/* social_chardle.c - written by Zac Saber (z5418202)
 * for cs1511 Flex Week lecture, in July of 2022!
 */

#include <stdio.h>
#include <ctype.h>

// A beginner's all-purpose software toolkit, written in C++
// Used to enable social functionality!
#include "splashkit.h"
// POSIX library, used to allow server to wait for a connection
#include <unistd.h>

#define MAX_ROUNDS 10
#define SCREEN_HEIGHT 10

#define MODE_PLAY 'p'
#define MODE_CONNECT 'c'
#define SERVER_ERROR '\n'
#define TRUE 1
#define FALSE 0

void print_welcome(void);

char choose_mode(void) {

    char mode = ' ';
    printf("Would you like to play or connect? ");
    scanf(" %c", &mode);

    while (mode != MODE_PLAY && mode != MODE_CONNECT) {
        printf("Invalid - you can either play (p) or connect (c) to a player!\n");
        printf("Would you like to play or connect?\n(p/c): ");
        scanf(" %c", &mode);
    }

    if (mode == MODE_PLAY) {
        printf("Hosting game server!\n");
    } else if (mode == MODE_CONNECT) {
        printf("Connecting to a player!\n");
    }

    return mode;

}

////////////////////////////// SERVER FUNCTION ////////////////////////////////

char listen_for_letter(void) { 

    // Sets default socket and port
    const string server_name = "server";
    unsigned short sock_port = 1511;

    printf("Starting server... ");
    server_socket server = create_server(server_name, sock_port);
    printf("Server started!\n");

    // Checks for new connection
    int i = 0, wait_time = 30;
    while (i < wait_time) {
        check_network_activity();
        if (server_has_new_connection(server)) {
            break;
        }
        sleep(1);
    }
    if (i == wait_time) {
        printf("Error :: Timed out!\n");
        return SERVER_ERROR;
    }

    /// Reads message from server
    string response = read_message_data(server);
    // printf("received: %s\n", response.c_str());

    // Receives ascii value as number
    long ascii_value = strtol(response.c_str(), NULL, 10);
    // printf("ascii long: %ld\n", ascii_value);
    // printf("char: %c\n", (char)ascii_value);
    char letter = (char) ascii_value;

    close_server(server);
    printf("Server closed.\n");

    /// 4) Play game
    return letter;
}

    /////////////////////////// SENDER FUNCTION ////////////////////////////
void transmit_letter(char input_letter) {

    // Sets default socket and port
    const string sock_name = "sock";
    unsigned short sock_port = 1511;

    printf("Connecting to server... ");
    connection sock_con = open_connection(sock_name, my_ip(), sock_port, TCP);
    printf("Connected to server!\n");

    const string message = std::to_string(input_letter);
    // printf("sending %s:\n", message.c_str());

    printf("Sending letter...\n");
    bool message_sent = send_message_to(message, sock_con);

    // Verifies character was transmitted correctly
    int sent = message_sent ? 1 : 0;
    // printf("Send status: %d\n", sent);

    if (sent == 0) {
        printf("Error :: Message not transmitted!\n");
    } else {
        printf("Sent successfully!\n");
    }

    return;
}

    /////////////////////////// MAIN FUNCTION /////////////////////////////
int main(void) {

    // Stage 1
    print_welcome();
    char mode = choose_mode();
    char correct_letter = ' ';

    if (mode == MODE_PLAY) {
        // Runs server to receive letter, then executes game code below
        correct_letter = listen_for_letter();
        if (correct_letter == SERVER_ERROR) {
            return 1;
        }

    } else if (mode == MODE_CONNECT) {
        // Scans in character for receiving player to guess, then returns
        printf("What letter will the player guess? ");
        scanf(" %c", &correct_letter);
        transmit_letter(correct_letter);
        return 0;

    } else {
        printf("Invalid mode!\n");
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////
    ////////////////////////// ORIGINAL CODE //////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    if ('a' <= correct_letter && correct_letter <= 'z') {
        /*printf("The answer is '%c' (ascii %d).\n",
                correct_letter, correct_letter);*/

        // Stage 3: Part 4: SCREEN_HEIGHT
        int i = 0;
        while (i < SCREEN_HEIGHT) {
            printf("*\n");
            i++;
        }

    } else if ('A' <= correct_letter && correct_letter <= 'Z') {
        correct_letter = tolower(correct_letter);
        // Stage 3, Part 3
        printf("The answer is '%c' (ascii %d).\n",
                correct_letter, correct_letter);

        // Stage 3: Part 4: SCREEN_HEIGHT
        int i = 0;
        while (i < SCREEN_HEIGHT) {
            printf("*\n");
            i++;
        }
    } else {
        printf("The answer must be a valid letter!\n");
        return 1;
    }

    // Stage 3, Part 1: Adding in multiple guesses
    int guess_number = 1;
    // Stage 3, Part 2: Checking if the number is correct.
    int is_correct = 0;

    while (guess_number <= MAX_ROUNDS && !is_correct) {
        char guess_letter;
        printf("What is guess #%d? ", guess_number);
        scanf(" %c", &guess_letter);

        // Stage 3, Part 3: Dealing with Uppercase Letters
        guess_letter = tolower(guess_letter);

        if ('a' > guess_letter || guess_letter > 'z' ) {
            printf("Your guess must be a valid letter!\n");
        } else if (guess_letter == correct_letter) {
            printf("Congratulations! You got the letter right!\n");
            is_correct = 1;
        } else if (guess_letter < correct_letter) {
            printf("Not quite! Guess later in the alphabet.\n");
        } else {
            printf("Not quite! Guess earlier in the alphabet.\n");
        }

        guess_number += 1;

    }

    return 0;

}

void print_welcome(void) {
    printf("[COMP1511 Guessing Game: CS Chardle]\n\n");
    printf("Welcome to the COMP1511 guessing game.\n");
    printf("You will need to input a letter to guess,\n");
    printf("Then let the player see the screen, and make guesses.\n");
}
