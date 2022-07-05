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
// C++ Input/Output Library, used to print lines!
#include <iostream>

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
    std::cout << "Would you like to play or connect? ";
    scanf(" %c", &mode);

    while (mode != MODE_PLAY && mode != MODE_CONNECT) {
        std::cout << "Invalid - you can either play (p) or connect (c) to a player!\n";
        std::cout << "Would you like to play or connect?\n(p/c): ";
        scanf(" %c", &mode);
    }

    if (mode == MODE_PLAY) {
        std::cout << "Hosting game server!\n";
    } else if (mode == MODE_CONNECT) {
        std::cout << "Connecting to a player!\n";
    }

    return mode;

}

////////////////////////////// SERVER FUNCTION ////////////////////////////////

char listen_for_letter(void) { 

    // Sets default socket and port
    const string server_name = "server";
    unsigned short sock_port = 1511;

    std::cout << "Starting server... ";
    server_socket server = create_server(server_name, sock_port);
    std::cout << "Server started!\n";

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
        std::cout << "Error :: Timed out!\n";
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
    std::cout << "Server closed.\n";

    /// 4) Play game
    return letter;
}

    /////////////////////////// SENDER FUNCTION ////////////////////////////
void transmit_letter(char input_letter) {

    // Sets default socket and port
    const string sock_name = "sock";
    unsigned short sock_port = 1511;

    std::cout << "Connecting to server... ";
    connection sock_con = open_connection(sock_name, my_ip(), sock_port, TCP);
    std::cout << "Connected to server!\n";

    const string message = std::to_string(input_letter);
    // printf("sending %s:\n", message.c_str());

    std::cout << "Sending letter...\n";
    bool message_sent = send_message_to(message, sock_con);

    // Verifies character was transmitted correctly
    int sent = message_sent ? 1 : 0;
    // printf("Send status: %d\n", sent);

    if (sent == 0) {
        std::cout << "Error :: Message not transmitted!\n";
    } else {
        std::cout << "Sent successfully!\n";
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
        std::cout << "What letter will the player guess? ";
        scanf(" %c", &correct_letter);
        transmit_letter(correct_letter);
        return 0;

    } else {
        std::cout << "Invalid mode!\n";
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////
    ////////////////////////// ORIGINAL CODE //////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    if ('a' <= correct_letter && correct_letter <= 'z') {
        string answer_string = "The answer is '"; 
        answer_string += std::atoi(correct_letter) + "'(ascii ";
        answer_string += std::to_string(correct_letter) + ").\n";

        std::cout << answer_string; 

        // Stage 3: Part 4: SCREEN_HEIGHT
        int i = 0;
        while (i < SCREEN_HEIGHT) {
            std::cout << "*\n";
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
            std::cout << "*\n";
            i++;
        }
    } else {
        std::cout << "The answer must be a valid letter!\n";
        return 1;
    }

    // Stage 3, Part 1: Adding in multiple guesses
    int guess_number = 1;
    // Stage 3, Part 2: Checking if the number is correct.
    int is_correct = 0;

    while (guess_number <= MAX_ROUNDS && !is_correct) {
        char guess_letter;
        std::string guess_prompt = "What is guess #";
        guess_prompt += std::to_string(guess_number) + "? ";
        //printf("What is guess #%d? ", guess_number);
        std::cout << guess_prompt;
        scanf(" %c", &guess_letter);

        // Stage 3, Part 3: Dealing with Uppercase Letters
        guess_letter = tolower(guess_letter);

        if ('a' > guess_letter || guess_letter > 'z' ) {
            std::cout << "Your guess must be a valid letter!\n";
        } else if (guess_letter == correct_letter) {
            std::cout << "Congratulations! You got the letter right!\n";
            is_correct = 1;
        } else if (guess_letter < correct_letter) {
            std::cout << "Not quite! Guess later in the alphabet.\n";
        } else {
            std::cout << "Not quite! Guess earlier in the alphabet.\n";
        }

        guess_number += 1;

    }

    return 0;

}

void print_welcome(void) {
    std::cout << "[COMP1511 Guessing Game: CS Chardle]\n\n";
    std::cout << "Welcome to the COMP1511 guessing game.\n";
    std::cout << "You will need to input a letter to guess,\n";
    std::cout << "Then let the player see the screen, and make guesses.\n";
}
