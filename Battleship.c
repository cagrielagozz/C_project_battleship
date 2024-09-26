#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Maksimum oyun tahtası boyutu
#define MAX_BOARD_SIZE 26

// Oyun tahtaları
char player1Board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
char player2Board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];

// Oyuncuların gemilerinin konumu
int player1Ships;
int player2Ships;

// Oyuncu tarafından yerleştirilecek gemi sayısı
int numShips;

// Oyuncunun gemilerinin başlangıç konumları
typedef struct {
    int row;
    int col;
} ShipPosition;

// Oyuncunun gemilerinin başlangıç konumları
ShipPosition *player1ShipPositions;
ShipPosition *player2ShipPositions;

// Oyun tahtasını başlatma
void initializeBoard(char board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int boardSize) {
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            board[i][j] = ' ';
        }
    }
}

// Oyun tahtasını ekrana yazdırma (gizli gemileri göstermez)
void printBoard(char board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int boardSize, int hideShips) {
    printf("\n   ");
    for (char c = 'A'; c < 'A' + boardSize; c++) {
        printf("%c ", c);
    }
    printf("\n");

    for (int i = 0; i < boardSize; i++) {
        printf("%02d ", i + 1);
        for (int j = 0; j < boardSize; j++) {
            if (hideShips && board[i][j] != 'X') {
                printf("~ "); // Gizli gemileri gösterme
            } else {
                printf("%c ", board[i][j]);
            }
        }
        printf("\n");
    }
}

// Gemiyi tahtaya yerleştirme
void placeShip(char board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int row, int col, int size, char ship) {
    for (int i = 0; i < size; i++) {
        // Kontrol ekleyerek tahtanın sınırlarını aşmamayı ve gemilerin taşmamasını sağla
        if (row < MAX_BOARD_SIZE && col + i < MAX_BOARD_SIZE && board[row][col + i] == ' ') {
            board[row][col + i] = ship;
        } else {
            printf("Invalid ship placement. Exiting...\n");
            exit(1);
        }
    }
}

// Oyunun durumunu kontrol etme
int checkGameOver(int playerShips) {
    return (playerShips <= 0);
}

// Oyuncudan gemi yerleştirme isteği alınması
void getShipPlacements(ShipPosition *playerShipPositions, int player, int boardSize) {
    printf("Player %02d, enter ship placements:\n", player);

    for (int i = 0; i < numShips; i++) {
        printf("Ship %02d:\n", i + 1);

        // Gemi boyutunu kullanıcıdan al
        int shipSize;
        printf("Enter ship size(1-5): ");
        scanf("%d", &shipSize);

        // Gemi başlangıç pozisyonunu kullanıcıdan al
        printf("Row (01-%02d): ", boardSize);
        scanf("%d", &playerShipPositions[i].row);
        playerShipPositions[i].row--;  // Kullanıcı girişini 0-(boardSize-1) aralığına dönüştür

        printf("Column (A-%c): ", 'A' + boardSize - 1);
        char colChar;
        scanf(" %c", &colChar);  // Boşluk karakterini atlamak için
        playerShipPositions[i].col = colChar - 'A';  // Kullanıcı girişini 0-(boardSize-1) aralığına dönüştür

        // Gemi tahtaya yerleştir
        if (player == 1) {
            placeShip(player1Board, playerShipPositions[i].row, playerShipPositions[i].col, shipSize, 'D');
        } else {
            placeShip(player2Board, playerShipPositions[i].row, playerShipPositions[i].col, shipSize, 'D');
        }
    }
}

// Oyunun başlangıcını hazırlama
void initializeGame(int boardSize) {
    initializeBoard(player1Board, boardSize);
    initializeBoard(player2Board, boardSize);

    printf("Enter the number of ships for each player: ");
    scanf("%d", &numShips);

    player1Ships = numShips;
    player2Ships = numShips;

    // Dynamically allocate memory for ship positions
    player1ShipPositions = (ShipPosition *)malloc(numShips * sizeof(ShipPosition));
    player2ShipPositions = (ShipPosition *)malloc(numShips * sizeof(ShipPosition));

    getShipPlacements(player1ShipPositions, 1, boardSize);
    getShipPlacements(player2ShipPositions, 2, boardSize);
}

// Oyuncunun hamlesini işleme
int processMove(char board[MAX_BOARD_SIZE][MAX_BOARD_SIZE], int row, int col, int *playerShips) {
    if (row < 0 || row >= MAX_BOARD_SIZE || col < 0 || col >= MAX_BOARD_SIZE) {
        printf("Invalid move. Please choose a valid target.\n");
        return -1;  // Geçersiz hamle
    }

    if (board[row][col] == ' ') {
        printf("Missed!\n");
        return 0;  // İsabetsiz atış
    } else if (board[row][col] == 'X' || board[row][col] == 'O') {
        printf("You have fired at this target before. Please choose another target.\n");
        return -1;  // Geçersiz hamle
    } else {
        if (*playerShips == 0) {
            printf("Hit! All ships sunk!\n");
        } else {
            printf("Hit!\n");
        }
        board[row][col] = 'X';
        (*playerShips)--;  // Gemiyi batırma

        // Oyun durumunu kontrol etme
        if (checkGameOver(*playerShips)) {
            printf("All ships of the player are sunk!\n");
            return 2;  // Oyuncunun gemileri batmış durumda
        } else {
            return 1;  // İsabetli atış
        }
    }
}

int main() {
    // Set the seed for random number generation
    srand(time(NULL));

    // Oyun tahtasının boyutunu kullanıcıdan al
    int boardSize;
    printf("Enter the size of the game board (01-26): ");
    scanf("%d", &boardSize);

    // Kontrol etmek için oyun tahtası boyutunu sınırla
    if (boardSize < 1 || boardSize > MAX_BOARD_SIZE) {
        printf("Invalid board size. Exiting...\n");
        return 1;
    }

    // Start the game
    initializeGame(boardSize);

    int totalMoves = 0;  // Toplam hamle sayısı

    while (1) {
        // Oyuncu 1'in tahtasını göster
        printf("\n  Player 1's Board:\n");
        printBoard(player1Board, boardSize, 1);  // Gizli gemileri gösterme

        // Oyuncu 1'den hamle al
        printf("Player 1 -> Enter a move(e.g., A5): ");
        char move[3];
        scanf("%s", move);

        // Koordinatları çözme
        int row = move[1] - '1';
        int col = move[0] - 'A';

        // Oyuncu 1'in hamlesini işleme
        int result = processMove(player2Board, row, col, &player2Ships);
        if (result == -1) {
            continue;  // Geçersiz hamle, tekrar hamle iste
        } else if (result == 2) {
            // Oyun durumu kontrol etme
            if (checkGameOver(player2Ships)) {
                printf("Player 1 wins!\n");
                break;
            }
        }

        // Oyuncu 2'nin tahtasını göster
        printf("\n  Player 2's Board:\n");
        printBoard(player2Board, boardSize, 1);  // Gizli gemileri gösterme

        // Oyuncu 2'den hamle al
        printf("Player 2 -> Enter a move(e.g., A5): ");
        scanf("%s", move);

        // Koordinatları çözme
        row = move[1] - '1';
        col = move[0] - 'A';

        // Oyuncu 2'nin hamlesini işleme
        result = processMove(player1Board, row, col, &player1Ships);
        if (result == -1) {
            continue;  // Geçersiz hamle, tekrar hamle iste
        } else if (result == 2) {
            // Oyun durumu kontrol etme
            if (checkGameOver(player1Ships)) {
                printf("Player 2 wins!\n");
                break;
            }
        }

        // Toplam hamle sayısını artırma
        totalMoves++;
    }

    // Free dynamically allocated memory
    free(player1ShipPositions);
    free(player2ShipPositions);

    printf("Total move number: %02d\n", totalMoves + 1);

    return 0;
}