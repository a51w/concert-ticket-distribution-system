#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <Windows.h>


#define SEAT_COUNT 5
#define MAX_TICKETS 1000
#define MAX_SIZE 100


typedef struct {
    char Username[50];
    char zone;
    int Amount;
    time_t timestamp;
} Ticket;


Ticket ticketList[MAX_TICKETS];
int numTickets = 0;


typedef struct {
    Ticket data[MAX_SIZE];
    int front, rear;
} Queue;


void initQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}


bool isEmpty(Queue *q) {
    return (q->front == -1 && q->rear == -1);
}


bool isFull(Queue *q) {
    return (q->rear + 1) % MAX_SIZE == q->front;
}


void enqueue(Queue *q, Ticket ticket) {
    if (isFull(q)) {
        printf("Queue is full.\n");
        return;
    }
    if (isEmpty(q)) {
        q->front = q->rear = 0;
    } else {
        q->rear = (q->rear + 1) % MAX_SIZE;
    }
    q->data[q->rear] = ticket;
}


Ticket dequeue(Queue *q) {
    Ticket ticket;
    if (isEmpty(q)) {
        printf("Queue is empty.\n");
        strcpy(ticket.Username, "");
        ticket.zone = '\0';
        ticket.Amount = 0;
    } else if (q->front == q->rear) {
        ticket = q->data[q->front];
        q->front = q->rear = -1;
    } else {
        ticket = q->data[q->front];
        q->front = (q->front + 1) % MAX_SIZE;
    }
    return ticket;
}


void addTicket(char Username[], char zone, int Amount) {
    if (numTickets >= MAX_TICKETS) {
        printf("Ticket list is full.\n");
        return;
    }


    Ticket newTicket;
    strcpy(newTicket.Username, Username);
    newTicket.zone = zone;
    newTicket.Amount = Amount;
    newTicket.timestamp = time(NULL);


    ticketList[numTickets++] = newTicket;


    // Sort the ticket list based on timestamp
    for (int i = 0; i < numTickets - 1; i++) {
        for (int j = i + 1; j < numTickets; j++) {
            if (ticketList[i].timestamp > ticketList[j].timestamp) {
                Ticket temp = ticketList[i];
                ticketList[i] = ticketList[j];
                ticketList[j] = temp;
            }
        }
    }


    printf("Ticket for %s (Zone: %c, Amount: %d) added successfully.\n", Username, zone, Amount);
}


void displayTickets() {
    printf("\nAll tickets:\n");
    for (int i = 0; i < numTickets; i++) {
        struct tm* timeinfo = localtime(&ticketList[i].timestamp);
        char timeStr[50];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
        printf("Username: %s, Zone: %c, Amount: %d, Time: %s\n", ticketList[i].Username, ticketList[i].zone, ticketList[i].Amount, timeStr);
    }
}


void join(Queue *q, char Username[]) {
    Ticket newTicket;
    strcpy(newTicket.Username, Username);
    newTicket.timestamp = time(NULL);
    enqueue(q, newTicket);
    printf("Join successful\n");
}


void calculateWaitTime(Queue *q, char Username[], int *prevQueue, int *waitTime) {
    *prevQueue = 0;
    *waitTime = 0;
    bool found = false;
    Queue tempQueue;
    initQueue(&tempQueue);


    while (!isEmpty(q)) {
        Ticket t = dequeue(q);
        if (strcmp(t.Username, Username) == 0) {
            found = true;
            break;
        }
        enqueue(&tempQueue, t);
        (*prevQueue)++;
    }


    while (!isEmpty(&tempQueue)) {
        enqueue(q, dequeue(&tempQueue));
    }


    if (found) {
        *waitTime = (*prevQueue);
    } else {
        printf("User not found in the queue.\n");
    }
}


int checkSeatAvailability(char zone, int num_seats) {
    int num_reserved_seats = 0;
    int available_seats = SEAT_COUNT;


    for (int i = 0; i < numTickets; i++) {
        if (ticketList[i].zone == zone) {
            num_reserved_seats += ticketList[i].Amount;
        }
    }


    if (num_reserved_seats >= SEAT_COUNT) {
        printf("Zone %c is full.\n", zone);
        return 0;
    }


    available_seats -= num_reserved_seats;


    if(available_seats >= num_seats){
        return available_seats;
    }
    return 0;
}


void displayPreviousQueue(Queue *q, char Username[]) {
    int prevQueue = 0, waitTime = 0;
    calculateWaitTime(q, Username, &prevQueue, &waitTime);


    if (prevQueue == 0) {
        printf("No users joined the queue before %s.\n", Username);
    } else {
        printf("Previous queue:\n");
        printf("%d users joined the queue before %s.\n", prevQueue, Username);
        printf("Waited time: %d minutes\n", waitTime);
    }
    waitTime++;
}


// Function prototype
int waitForInputOrTimeout(DWORD timeout_ms);


// Function to print tickets and initiate countdown for purchase
void purchase(char zone, int num_seats) {
    //printf("\nDo you want to purchase the ticket? (y/n): ");
    fflush(stdout); // Flush the output buffer
    printf("\nPlease pay in 60 seconds\n");


            // Countdown for purchase
            for (int i = 60; i >= 0; i--) {
                printf("\rTime remaining: %d seconds", i);
                fflush(stdout);
                Sleep(1000); // Wait for 1 second
            }
            printf("\n");
            printf("\nHave you finished paying? (y/n): ");


    // Record the starting time
    time_t startTime = time(NULL);


    char choice;
    int ready = waitForInputOrTimeout(60000); // Timeout set to 60 seconds


    if (ready > 0) {
        // within time limit
        scanf(" %c", &choice);


        if (choice == 'y' || choice == 'Y') {


            printf("\nPurchase success!\n");


            // Output
            printf("Your ticket details:\n");
            printf("Zone: %c\n", zone);
            printf("Number of seats: %d\n", num_seats);
            printf("\nCountdown 5 seconds\n");


            // Countdown for purchase
            for (int i = 5; i >= 0; i--) {
                printf("\rTime remaining: %d seconds", i);
                fflush(stdout);
                Sleep(1000); // Wait for 1 second
            }
            printf("\n");


            // Purchase completed
            printf("Purchase completed!\n");


        } else if (choice == 'n' || choice == 'N') {


            printf("Purchase unsuccessful!\n");


        } else {


            printf("Invalid choice. Purchase unsuccessful!\n");


        }
    } else if (ready == 0) {
        // Not in time limit
        printf("\nPurchase unsuccessful! You did not make a selection within the time limit.\n");


    } else {
        // Error
        printf("Error in input. Purchase unsuccessful!\n");


    }
}


// Function to wait for user input or timeout
int waitForInputOrTimeout(DWORD timeout_ms) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        printf("Error getting standard input handle.\n");
        return -1;
    }


    INPUT_RECORD ir;
    DWORD numEvents;
    DWORD startTick = GetTickCount();


    while (1) {
        DWORD elapsedTime = GetTickCount() - startTick;
        if (elapsedTime >= timeout_ms) {
            return 0; // Timeout
        }


        if (!PeekConsoleInput(hStdin, &ir, 1, &numEvents)) {
            printf("Error peeking console input.\n");
            return -1;
        }


        if (numEvents > 0) {
            return 1; // Input is available
        }


        // Sleep for a short time to avoid busy-waiting
        Sleep(10);
    }
}






int main() {
    Queue queue;
    initQueue(&queue);


    char menu[100];
    char Username[50], zone;
    int Amount, Available;
    char choice[5];
    bool joined = false;


    while (1) {
        printf("\n==========================\n");
        printf("Concert ticket shop\n");
        printf("==========================\n");
        printf("1 Join\n");
        printf("2 Buy\n");
        printf("3 Display\n");
        printf("4 Quit\n");
        printf("==========================\n");
        printf("Enter your choice: ");
        scanf("%s", menu);


        if (!joined && (strcmp(menu, "2") == 0 || strcmp(menu, "3") == 0)) {
            printf("You need to join the queue first.\n");
        } else if (strcmp(menu, "2") == 0) {
            printf("\n************** BUY SECTION **************\n");
            printf("Enter your Username: ");
            scanf("%s", Username);


            bool userFound = false;
            for (int i = queue.front; i != (queue.rear + 1) % MAX_SIZE; i = (i + 1) % MAX_SIZE) {
                if (strcmp(queue.data[i].Username, Username) == 0) {
                    userFound = true;
                    break;
                }
            }


            if (!userFound) {
                printf("You need to join the queue first.\n");
                continue;
            }


            do {
                printf("Enter zone (A, B, C): ");
                scanf(" %c", &zone);
                if (zone != 'A' && zone != 'a' && zone != 'B' && zone != 'b' && zone != 'C' && zone != 'c') {
                    printf("Invalid Zone. Please enter A, B, or C.\n");
                }
            } while (zone != 'A' && zone != 'a' && zone != 'B' && zone != 'b' && zone != 'C' && zone != 'c');


            int availableSeat = checkSeatAvailability(zone, 0);


            if (!availableSeat) {
                printf("No available seats in zone %c.\n", zone);
                continue;
            }
            else{
                printf("Available Seats: %d\n", availableSeat);
            }


            do {
                printf("Enter Amount (up to 2 tickets): ");
                scanf("%d", &Amount);
                if (Amount > 2) {
                    printf("One user can purchase no more than 2 tickets. Please enter the number of tickets you want again.\n");
                }
            } while (Amount > 2);


            if (!checkSeatAvailability(zone, Amount)) {
                printf("No available seats in zone %c.\n", zone);
                continue;
            }


            displayPreviousQueue(&queue, Username);


            printf("Do you want to continue? (Yes/No): ");
            scanf("%s", choice);


            if (!(strcmp(choice, "Yes") == 0 || strcmp(choice, "Y") == 0 || strcmp(choice, "yes") == 0 || strcmp(choice, "y") == 0)) {
                printf("Exiting program.\n");
                break;
            }
            purchase(zone, Amount); // Call purchase function with selected zone and number of seats
            addTicket(Username, zone, Amount); // Add the purchased tickets to the ticket list


        } else if (strcmp(menu, "3") == 0) {
            printf("\n************** DISPLAY SECTION **************\n");
            displayTickets();
        } else if (strcmp(menu, "4") == 0) {
            printf("Exiting the program.\n");
            break;
        } else if (strcmp(menu, "1") == 0) {
            printf("\n************** JOIN SECTION **************\n");
            printf("Enter your name: ");
            scanf("%s", Username);
            join(&queue, Username);
            joined = true;
            printf("Do you want to continue? (Yes/No): ");
            scanf("%s", choice);
            if (!(strcmp(choice, "Yes") == 0 || strcmp(choice, "Y") == 0 || strcmp(choice, "yes") == 0 || strcmp(choice, "y") == 0)) {
                printf("Exiting program.\n");
                break;
            }
        } else {
            printf("Invalid option. Please try again.\n");
        }
    }


    return 0;
}
