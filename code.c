#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define BOARD_SIZE 10

int snakeHead[] = {99, 95, 92, 62, 64, 47, 56, 49, 16};
int snakeTail[] = {78, 75, 88, 19, 60, 26, 53, 11, 6};

int ladderBottom[] = {2, 7, 8, 15, 21, 28, 36, 51, 71};
int ladderTop[]    = {38, 14, 31, 26, 42, 84, 44, 67, 91};

int checkSnake(int pos)
{
    for (int i = 0; i < 9; i++)
    {
        if (pos == snakeHead[i])
            return snakeTail[i];
    }

    return pos;
}

int checkLadder(int pos)
{
    for (int i = 0; i < 9; i++)
    {
        if (pos == ladderBottom[i])
            return ladderTop[i];
    }

    return pos;
}

/* Convert board number to screen position */
Vector2 getPosition(int number, float boardX, float boardY, float cell)
{
    int n = number - 1;

    int row = n / 10;
    int col = n % 10;

    /* Zig-zag numbering */
    if (row % 2 == 1)
        col = 9 - col;

    float x = boardX + col * cell + cell / 2;
    float y = boardY + (9 - row) * cell + cell / 2;

    return (Vector2){x, y};
}

void drawBoard(float boardX, float boardY, float cell)
{
    for (int row = 0; row < 10; row++)
    {
        for (int col = 0; col < 10; col++)
        {
            int actualCol = col;

            if (row % 2 == 1)
                actualCol = 9 - col;

            int number = row * 10 + actualCol + 1;

            float x = boardX + col * cell;
            float y = boardY + (9 - row) * cell;

            DrawRectangleLines(
                x,
                y,
                cell,
                cell,
                BLACK
            );

            DrawText(
                TextFormat("%d", number),
                x + cell * 0.08,
                y + cell * 0.08,
                cell * 0.25,
                BLACK
            );
        }
    }
}

void drawSnakes(float boardX, float boardY, float cell)
{
    for (int i = 0; i < 9; i++)
    {
        Vector2 head =
            getPosition(snakeHead[i], boardX, boardY, cell);

        Vector2 tail =
            getPosition(snakeTail[i], boardX, boardY, cell);

        DrawLineEx(head, tail, cell * 0.10, RED);

        DrawCircleV(head, cell * 0.18, RED);

        /* Eyes */
        DrawCircle(
            head.x - cell * 0.06,
            head.y - cell * 0.05,
            cell * 0.035,
            WHITE
        );

        DrawCircle(
            head.x + cell * 0.06,
            head.y - cell * 0.05,
            cell * 0.035,
            WHITE
        );
    }
}

void drawLadders(float boardX, float boardY, float cell)
{
    for (int i = 0; i < 9; i++)
    {
        Vector2 bottom =
            getPosition(ladderBottom[i], boardX, boardY, cell);

        Vector2 top =
            getPosition(ladderTop[i], boardX, boardY, cell);

        float offset = cell * 0.15;

        Vector2 leftBottom =
            {bottom.x - offset, bottom.y};

        Vector2 leftTop =
            {top.x - offset, top.y};

        Vector2 rightBottom =
            {bottom.x + offset, bottom.y};

        Vector2 rightTop =
            {top.x + offset, top.y};

        /* Ladder sides */
        DrawLineEx(
            leftBottom,
            leftTop,
            cell * 0.06,
            DARKGREEN
        );

        DrawLineEx(
            rightBottom,
            rightTop,
            cell * 0.06,
            DARKGREEN
        );

        /* Ladder rungs */
        for (int j = 1; j < 7; j++)
        {
            float t = j / 7.0f;

            float x1 =
                leftBottom.x +
                (leftTop.x - leftBottom.x) * t;

            float y1 =
                leftBottom.y +
                (leftTop.y - leftBottom.y) * t;

            float x2 =
                rightBottom.x +
                (rightTop.x - rightBottom.x) * t;

            float y2 =
                rightBottom.y +
                (rightTop.y - rightBottom.y) * t;

            DrawLineEx(
                (Vector2){x1, y1},
                (Vector2){x2, y2},
                cell * 0.045,
                DARKGREEN
            );
        }
    }
}

int main()
{
    /* Full screen */
    SetConfigFlags(FLAG_FULLSCREEN_MODE);

    InitWindow(
        GetMonitorWidth(0),
        GetMonitorHeight(0),
        "Snake and Ladder"
    );

    SetTargetFPS(60);

    srand(time(NULL));

    int player1 = 1;
    int player2 = 1;

    int currentPlayer = 1;
    int dice = 0;

    int winner = 0;

    while (!WindowShouldClose())
    {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        /*
         * Leave some space at the bottom
         * for game information.
         */
        float infoHeight = screenHeight * 0.12f;

        float boardSize = screenHeight - infoHeight;

        /*
         * Keep board square.
         */
        if (boardSize > screenWidth)
            boardSize = screenWidth;

        float boardX =
            (screenWidth - boardSize) / 2;

        float boardY = 0;

        float cell =
            boardSize / 10.0f;

        /* Restart */
        if (IsKeyPressed(KEY_R))
        {
            player1 = 1;
            player2 = 1;

            currentPlayer = 1;
            dice = 0;
            winner = 0;
        }

        /* Roll dice */
        if (IsKeyPressed(KEY_SPACE) && winner == 0)
        {
            dice = rand() % 6 + 1;

            if (currentPlayer == 1)
            {
                /*
                 * If roll reaches or passes 100,
                 * Player 1 wins.
                 */
                if (player1 + dice >= 100)
                {
                    player1 = 100;
                    winner = 1;
                }
                else
                {
                    player1 += dice;

                    player1 = checkLadder(player1);
                    player1 = checkSnake(player1);

                    if (player1 == 100)
                        winner = 1;
                    else
                        currentPlayer = 2;
                }
            }
            else
            {
                if (player2 + dice >= 100)
                {
                    player2 = 100;
                    winner = 2;
                }
                else
                {
                    player2 += dice;

                    player2 = checkLadder(player2);
                    player2 = checkSnake(player2);

                    if (player2 == 100)
                        winner = 2;
                    else
                        currentPlayer = 1;
                }
            }
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        /* Board */
        drawBoard(
            boardX,
            boardY,
            cell
        );

        /* Ladders */
        drawLadders(
            boardX,
            boardY,
            cell
        );

        /* Snakes */
        drawSnakes(
            boardX,
            boardY,
            cell
        );

        /* Player 1 */
        Vector2 p1 =
            getPosition(
                player1,
                boardX,
                boardY,
                cell
            );

        /* Player 2 */
        Vector2 p2 =
            getPosition(
                player2,
                boardX,
                boardY,
                cell
            );

        /*
         * If both players are on the
         * same square, separate them.
         */
        if (player1 == player2)
        {
            p1.x -= cell * 0.15f;
            p2.x += cell * 0.15f;
        }

        DrawCircleV(
            p1,
            cell * 0.18f,
            BLUE
        );

        DrawCircleV(
            p2,
            cell * 0.18f,
            PURPLE
        );

        /* Information area */
        float infoY = boardSize;

        DrawRectangle(
            0,
            infoY,
            screenWidth,
            infoHeight,
            LIGHTGRAY
        );

        DrawText(
            TextFormat(
                "PLAYER 1: %d",
                player1
            ),
            20,
            infoY + 15,
            25,
            BLUE
        );

        DrawText(
            TextFormat(
                "PLAYER 2: %d",
                player2
            ),
            20,
            infoY + 50,
            25,
            PURPLE
        );

        DrawText(
            TextFormat(
                "DICE: %d",
                dice
            ),
            screenWidth * 0.40f,
            infoY + 30,
            30,
            BLACK
        );

        if (winner == 0)
        {
            DrawText(
                TextFormat(
                    "PLAYER %d - PRESS SPACE",
                    currentPlayer
                ),
                screenWidth * 0.55f,
                infoY + 15,
                22,
                BLACK
            );

            DrawText(
                "R = RESTART",
                screenWidth * 0.55f,
                infoY + 50,
                18,
                DARKGRAY
            );
        }
        else
        {
            DrawText(
                TextFormat(
                    "PLAYER %d WINS!",
                    winner
                ),
                screenWidth * 0.55f,
                infoY + 15,
                30,
                RED
            );

            DrawText(
                "Press R to play again",
                screenWidth * 0.55f,
                infoY + 50,
                18,
                BLACK
            );
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
