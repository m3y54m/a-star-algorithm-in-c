/*-------------------------------------------------------------------------------------------
A* Algorithm Implementation in C

A very basic and easy to understand implementation of A* algorithm in C that was part of
my "Fundamentals of Programming" course at the University of Zanjan, Fall 2012.

Meysam Parvizi (http://meysamparvizi.ir)
     
The A* algorithm adopted from:
http://www.raywenderlich.com/4946/introduction-to-a-pathfinding

Special thanks to my teacher Dr. Mohsen Nourazar
--------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define DimensionOfMap 10
#define ListLength DimensionOfMap*DimensionOfMap

/* implementation of Turbo-C function getch() */
#include <termios.h>
#include <unistd.h>
/* reads from keypress, doesn't echo */
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void GetMapByKeyboard();
void GetString();
void GetMapFromFile();
void GenerateRandomMap();
void PrintMap();
void SaveMap();
int FindShortestWay();


int main(void)
{
    int map[DimensionOfMap][DimensionOfMap];
    int solved[DimensionOfMap][DimensionOfMap];
    int a;
    char c;
    
    system("cls");
    
    do
    {
        printf( "\n\n **********************************\n\n" );
        printf( "          THE A* ALGORITHM         \n\n" );
        printf( "    Programmed by MEYSAM PARVIZI   \n\n" );
        printf( " **********************************\n" );
        printf( "\n HOW DO YOU WANT TO ENTER THE MAP?\n\n" );
        printf( " 1. I want to enter the map character-by-character using keyboard.\n" );
        printf( " 2. I want to import the map from file.\n" );
        printf( " 3. I want the program to generate a random map.\n" );
        printf( "\n Please enter the number of item you want to perform (1 or 2 or 3): " );
        scanf( "%d", &a );

        switch ( a )
        {
        case 1:
            GetMapByKeyboard( map, DimensionOfMap );
            break;
        case 2:
            GetMapFromFile( map, DimensionOfMap );
            break;
        case 3:
            GenerateRandomMap( map, DimensionOfMap );
            break;
        } /* end switch*/

        printf( "\n    THIS IS THE MAP:\n" );
        PrintMap( map, DimensionOfMap );

        switch ( FindShortestWay( map, DimensionOfMap, solved ) )
        {
        case 0:
            printf( "\n    THERE IS NO WAY :(\n" );
            break;
        case 1:
            printf( "\n    THIS IS THE PATH:\n" );
            PrintMap( solved, DimensionOfMap );
            SaveMap( "SolvedMap.txt", solved, DimensionOfMap );
            printf( "\n     YOU ARRIVED :)\a\n" );
            break;
        case 2:
            printf( "\n  ERROR: The map is incomplete!\n" );
            break;
        } /* end switch*/

        getchar(); /* Pass over the "Enter (Return)" character */
        printf( "\n Do you want to restart the program (Y/N)? " );
        c = getchar();
    }
    while ( (c != 'N') && (c != 'n') );
    return 0;
}

void GetMapByKeyboard(int MyMap[][DimensionOfMap], int Dimension) /* This function gets the map using keyboard */
{
    int checkA, checkB;
    int i, j;
    char c;

    printf( "\n  Enter the map character-by-character according to the instructions below:\n\n" );
    printf( "  - This map is %dx%d.\n", Dimension, Dimension );
    printf( "  - The character 'A' or 'a' represents the original square and must be entered\n  just once.\n" );
    printf( "  - The character 'B' or 'b' represents the destination square and must be\n  entered just once.\n" );
    printf( "  - The character '#' or 'H' or 'h' represents an obstacle square and can be\n  entered several times.\n" );
    printf( "  - The other characters represent open squares and can be entered several\n  times.\n\n" );

    checkA = 0;
    checkB = 0;
    for (j = 0 ; j < Dimension ; j++)
    {
        printf( "  " );
        for (i = 0 ; i < Dimension ; i++)
        {
            c = getch();

            if ( ((c == 'A') || (c == 'a')) && checkA == 0 )
            {
                checkA = 1;
                printf( "A" );
                MyMap[i][j] = 3;
            }
            else
            {
                if ( ((c == 'B') || (c == 'b')) && checkB == 0 )
                {
                    checkB = 1;
                    printf( "B" );
                    MyMap[i][j] = 4;
                }
                else
                {
                    if ( (c == '#') || (c == 'H') || (c == 'h') )
                    {
                        printf( "#" );
                        MyMap[i][j] = 2;
                    }
                    else
                    {
                        printf( "O" );
                        MyMap[i][j] = 0;
                    }
                }
            } /* end else */
        } /* end for */
        printf( "\n" );
    } /* end for */
    SaveMap( "YourMap.txt", MyMap, Dimension );
}

void GetString(char s[]) /* This function gets an string */
{
    char c;
    int i = 0;

    while ( ( c = getchar() ) != '\n' )
    {
        s[i] = c;
        i++;
    }
    s[i] = '\0'; /* terminate string */
}

void GetMapFromFile(int MyMap[][DimensionOfMap], int Dimension) /* This function converts the map file to an array */
{
    int i, j;
    char c;
    char FileURL[255];
    FILE *MapFile;

    printf( "\n  Enter the URL of map file according to the instructions below:\n\n" );
    printf( "  - The map should be %dx%d.\n", Dimension, Dimension );
	printf( "  - The end of line sequence of file must be LF.\n");
    printf( "  - The character '@' in map file represents the original square and must be\n  entered just once.\n" );
    printf( "  - The character '$' in map file represents the destination square and must be\n  entered just once.\n" );
    printf( "  - The character '#' in map file represents an obstacle square and can be\n  entered several times.\n" );
    printf( "  - The other characters ( specially '0' ) represent open squares and can be\n  entered several times.\n" );
    printf( "  - If the map file is in the directory which this program is running, just\n  enter the name and the extention of it. (Example: myfile.txt)\n" );
    printf( "  - If the map file is not in the directory which this program is running, you\n  must enter the full URL. (Example: c:\\myfolder\\myfile.txt)\n" );
    printf( "  - If you want to use the default map of this program DefaultMap.txt, enter\n  the word DEFAULT. (It must be CAPITALIZED.)\n\n  " );

    getchar(); /* Pass over the "Enter (Return)" character */
    GetString( FileURL );

    if (strcmp( FileURL, "DEFAULT" ) == 0)
        strcpy( FileURL, "DefaultMap.txt" );

    MapFile = fopen( FileURL, "r" ); /* Open the map file */

    while (MapFile == 0)
    {
        printf( "\n  ERROR: The URL you entered is invalid!\n\n Please Try again: " );
        GetString( FileURL );
        if (strcmp( FileURL, "DEFAULT" ) == 0)
            strcpy( FileURL, "DefaultMap.txt" );
        MapFile = fopen( FileURL, "r" ); /* Open the map file */
    }
    for (j = 0 ; j < Dimension ; j++)
    {
        for (i = 0 ; i < Dimension ; i++)
        {
            c = getc(MapFile); /* Character by character reads the map file */
            switch ( c )
            {
            case '0':                  /* If the input character is '0', it is an open square which we specify it by the number 0 */
                MyMap[i][j] = 0;
                break;
            case '#':                  /* If the input character is '#', it is an obstacle (wall, lake etc.) which we specify it by the number 2 */
                MyMap[i][j] = 2;
                break;
            case '@':                  /* If the input character is '@', it is the original square which we specify it by the number 3 */
                MyMap[i][j] = 3;
                break;
            case '$':                  /* If the input character is '$', it is the destination square which we specify it by the number 4 */
                MyMap[i][j] = 4;
                break;
            default:                  /* If the input character is none of the above, it is the considered an open square which we specify it by the number 0 */
                MyMap[i][j] = 0;
                break;
            }
        } /* end for */
        getc(MapFile); /* Pass over the "Enter (Return)" character */
    } /* end for */
    fclose( MapFile ); /* Close the map file */
}

void GenerateRandomMap(int MyMap[][DimensionOfMap], int Dimension) /* This function generates a random map */
{
    int i, j;
    int RndNum;

    srand( time( NULL ) );
    for (j = 0 ; j < Dimension ; j++)
    {
        for (i = 0 ; i < Dimension ; i++)
        {
            if ( (i == 0) && (j == 0) )
            {
                MyMap[i][j] = 3; /* Always the first square is the original square which we specify it by the number 3 */
            }
            else
            {
                if ( (i == Dimension - 1) && (j == Dimension - 1) )
                {
                    MyMap[i][j] = 4; /* Always the last square is the destination square which we specify it by the number 4 */
                }
                else
                {
                    RndNum = rand() % 4 ;
                    switch ( RndNum )
                    {
                    case 0:                  /* If the random number is 0 or 1 or 2 (it has triple chance), it is an open square which we specify it by the number 0 */
                    case 1:
                    case 2:
                        MyMap[i][j] = 0;
                        break;
                    case 3:                  /* If the random number is 1, it is an obstacle (wall, lake etc.) which we specify it by the number 2 */
                        MyMap[i][j] = 2;
                        break;
                    }
                } /* end else */
            } /* end else */
        } /* end for */
    } /* end for */
    SaveMap( "RandomMap.txt", MyMap, Dimension );
}

void PrintMap(int MapArray[][DimensionOfMap], int Dimension) /* This function prints a 2D (NxN) map array in a simple frame */
{
    int i, j;
    char c;

    printf( "   " );
    for (i = 0 ; i < (2 * Dimension - 1) ; i++)
        printf( "-" );
    printf( "\n" );

    for (j = 0 ; j < Dimension ; j++)
    {
        printf( "  |" );
        for (i = 0 ; i < Dimension ; i++)
        {
            switch ( MapArray[i][j] )
            {
            case 0:                  /* If quantity of the current element of map array is 0, it represents an open square showed by a ' ' */
                c = ' ';
                break;
            case 1:                  /* If quantity of the current element of map array is 1, it represents a square of final path showed by a '1' */
                c = '+';
                break;
            case 2:                  /* If quantity of the current element of map array is 2, it represents an obstacle (wall, lake etc.) showed by a '#' */
                c = '#';
                break;
            case 3:                  /* If quantity of the current element of map array is 3, it represents the original square showed by '@' */
                c = 'A';
                break;
            case 4:                  /* If quantity of the current element of map array is 4, it represents the destination square showed by '$' */
                c = 'B';
                break;
            }
            printf( "%c", c );
            if (i <= Dimension - 2)
                printf( " " );
        }
        printf( "|\n" );
    }

    printf( "   " );
    for (i = 0 ; i < (2 * Dimension - 1) ; i++)
        printf( "-" );
    printf( "\n" );
}

void SaveMap( char URL[], int MapArray[][DimensionOfMap], int Dimension) /* This function saves a 2D (NxN) map array as a text file */
{
    int i, j;
    char c;
    FILE *theMap;

    theMap = fopen( URL, "w" );
    if (theMap != 0)
    {
        for (j = 0 ; j < Dimension ; j++)
        {
            for (i = 0 ; i < Dimension ; i++)
            {
                switch ( MapArray[i][j] )
                {
                case 0:                  /* If quantity of the current element of map array is 0, it represents an open square showed by a ' ' */
                    c = '0';
                    break;
                case 1:                  /* If quantity of the current element of map array is 1, it represents a square of final path showed by a '1' */
                    c = '1';
                    break;
                case 2:                  /* If quantity of the current element of map array is 2, it represents an obstacle (wall, lake etc.) showed by a '#' */
                    c = '#';
                    break;
                case 3:                  /* If quantity of the current element of map array is 3, it represents the original square showed by '@' */
                    c = '@';
                    break;
                case 4:                  /* If quantity of the current element of map array is 4, it represents the destination square showed by '$' */
                    c = '$';
                    break;
                }
                fprintf( theMap, "%c", c );
            }
            fprintf( theMap, "\n" );
        }
        fclose( theMap );
    }
}

int FindShortestWay(int MapArray[][DimensionOfMap], int Dimension, int SolvedMapArray[][DimensionOfMap]) /* This is the main function which finds one of the shortest ways from original square to destination square */
{
    struct XY
    {
        int x;
        int y;
    };

    struct XYScores
    {
        int G;
        int H;
        int F;
        int x;
        int y;
    };

    struct XY OriginalSquare, DestinationSquare;
    struct XY FinalPath[ListLength];
    struct XYScores ClosedList[ListLength];
    struct XYScores OpenList[ListLength];
    struct XY CurrentSquare;
    struct XY AdjacentSquares[4];
    int IsInClosedList, IsInOpenList;
    int i, j, k, m, N, T, temp;
    int TempG;
    int LowestF, PlaceOfLowestF, PlaceOfCurrentSquare;
    int IsWay = 0; /* Indicates the result of path finding: 0 means there is no way, 1 means there is at least one way, 2 means the map is incomplete */

    OriginalSquare.x = -1;      /* A sign that shows there is no original square on map */
    DestinationSquare.x = -1;   /* A sign that shows there is no destination square on map */

    /* Find the positions of original and destination squares (Begin:) */
    for (i = 0 ; i < Dimension ; i++)
    {
        for (j = 0 ; j < Dimension ; j++)
        {
            if (MapArray[i][j] == 3)
            {
                OriginalSquare.x = i;
                OriginalSquare.y = j;
            }
            if (MapArray[i][j] == 4)
            {
                DestinationSquare.x = i;
                DestinationSquare.y = j;
            }
        }
    }
    /* Find the positions of original and destination squares (End.) */

    if ( (OriginalSquare.x == -1) || (DestinationSquare.x == -1) )
    {
        IsWay = 2;
    }
    else
    {

        TempG = 0;
        T = 0; /* Counts items of Open-List */
        N = 0; /* Counts items of Closed-List */
        OpenList[0].F = -1; /* A sign that shows Open-List is empty */

        do
        {
            if (N == 0)
            {
                /* Get the square with the lowest F score */
                CurrentSquare.x = OriginalSquare.x;
                CurrentSquare.y = OriginalSquare.y;

                /* Add the current square to the Closed-List */
                ClosedList[0].x = CurrentSquare.x;
                ClosedList[0].y = CurrentSquare.y;
                ClosedList[0].H = fabs(DestinationSquare.x - OriginalSquare.x) + fabs(DestinationSquare.y - OriginalSquare.y);
                ClosedList[0].G = 0;
                ClosedList[0].F = ClosedList[0].G + ClosedList[0].H;
                N++;
            }
            else
            {
                /* Get the square with the lowest F score (Begin:) */
                LowestF = OpenList[T - 1].F; /* The Square most recently  added to the Open-List */
                PlaceOfLowestF = T - 1;
                for (m = T - 2 ; m >= 0 ; m--)
                {
                    if (OpenList[m].F < LowestF)
                    {
                        LowestF = OpenList[m].F;
                        PlaceOfLowestF = m;
                    }
                }

                CurrentSquare.x = OpenList[PlaceOfLowestF].x;
                CurrentSquare.y = OpenList[PlaceOfLowestF].y;
                /* Get the square with the lowest F score (End.) */

                /* Add the current square to the Closed-List */
                ClosedList[N].x = CurrentSquare.x;
                ClosedList[N].y = CurrentSquare.y;
                ClosedList[N].F = OpenList[PlaceOfLowestF].F;
                ClosedList[N].G = OpenList[PlaceOfLowestF].G;
                ClosedList[N].H = OpenList[PlaceOfLowestF].H;
                PlaceOfCurrentSquare = N;
                N++;

                /* Remove current square from the Open-List (Begin:) */
                TempG = OpenList[PlaceOfLowestF].G; /* Save G score of current square for future */

                if ( PlaceOfLowestF == (T - 1) )
                {
                    OpenList[T - 1].G = -1; /* -1 represents empty cell on Open-List */
                    OpenList[T - 1].H = -1;
                    OpenList[T - 1].F = -1;
                    OpenList[T - 1].x = -1;
                    OpenList[T - 1].y = -1;

                    T--;
                }
                else
                {
                    for (m = PlaceOfLowestF ; m <= (T - 2) ; m++)
                    {
                        OpenList[m].G = OpenList[m + 1].G;
                        OpenList[m].H = OpenList[m + 1].H;
                        OpenList[m].F = OpenList[m + 1].F;
                        OpenList[m].x = OpenList[m + 1].x;
                        OpenList[m].y = OpenList[m + 1].y;
                    }

                    OpenList[T - 1].G = -1;
                    OpenList[T - 1].H = -1;
                    OpenList[T - 1].F = -1;
                    OpenList[T - 1].x = -1;
                    OpenList[T - 1].y = -1;

                    T--;
                }
                /* Remove current square from the Open-List (End.) */

                /* If we added the destination to the Closed-List, we've found a path (Begin:) */
                IsInClosedList = 0;
                for (m = 0 ; m < N ; m++)
                {
                    if ( (DestinationSquare.x == ClosedList[m].x) &&
                            (DestinationSquare.y == ClosedList[m].y) )
                    {
                        IsInClosedList = 1;
                        break;
                    }
                }
                if (IsInClosedList == 1)
                {
                    /* PATH FOUND */
                    IsWay = 1;
                    break;  /* break the main loop */
                }
                /* If we added the destination to the Closed-List, we've found a path (End.) */
            } /* end else */

            /* Retrieve all its walkable adjacent squares (Begin:) */
            /* Top: */
            AdjacentSquares[0].x = CurrentSquare.x;
            AdjacentSquares[0].y = CurrentSquare.y - 1;
            /* Left: */
            AdjacentSquares[1].x = CurrentSquare.x - 1;
            AdjacentSquares[1].y = CurrentSquare.y;
            /* Bottom: */
            AdjacentSquares[2].x = CurrentSquare.x;
            AdjacentSquares[2].y = CurrentSquare.y + 1;
            /* Right: */
            AdjacentSquares[3].x = CurrentSquare.x + 1;
            AdjacentSquares[3].y = CurrentSquare.y;
            /* Retrieve all its walkable adjacent squares (End.) */

            for (k = 0 ; k <= 3 ; k++)
            {
                /* If this adjacent square is already in the Closed-List or if it is not an open square, ignore it (Begin:) */
                IsInClosedList = 0;
                for (m = 0 ; m < N ; m++)
                {
                    if ( (AdjacentSquares[k].x == ClosedList[m].x) &&
                            (AdjacentSquares[k].y == ClosedList[m].y) )
                    {
                        IsInClosedList = 1;
                        break;
                    }
                }
                if ( (AdjacentSquares[k].x < 0) ||
                        (AdjacentSquares[k].y < 0) ||
                        (AdjacentSquares[k].x >= Dimension) ||
                        (AdjacentSquares[k].y >= Dimension) ||
                        (MapArray[AdjacentSquares[k].x][AdjacentSquares[k].y] == 2) ||
                        (IsInClosedList == 1) )
                {
                    continue; /* Go to the next adjacent square */
                }
                /* If this adjacent square is already in the Closed-List or if it is not an open square, ignore it (End.) */


                IsInOpenList = 0;
                for (m = 0 ; m < T ; m++)
                {
                    if ( (AdjacentSquares[k].x == OpenList[m].x) &&
                            (AdjacentSquares[k].y == OpenList[m].y) )
                    {
                        IsInOpenList = 1;
                        temp = m;
                        break;
                    }
                }


                if (IsInOpenList != 1)   /* If its not in the Open-List */
                {
                    /* Compute its score and add it to the Open-List */
                    OpenList[T].H = fabs(DestinationSquare.x - AdjacentSquares[k].x) + fabs(DestinationSquare.y - AdjacentSquares[k].y);;
                    OpenList[T].G = TempG + 1;
                    OpenList[T].F = OpenList[T].H + OpenList[T].G;
                    OpenList[T].x = AdjacentSquares[k].x;
                    OpenList[T].y = AdjacentSquares[k].y;
                    T++;
                }
                else     /* if its already in the Open-List */
                {
                    if ( (ClosedList[PlaceOfCurrentSquare].G + 1) < OpenList[temp].G )
                    {
                        /* Update score of adjacent square that is in Open-List */
                        OpenList[temp].G = ClosedList[PlaceOfCurrentSquare].G + 1;
                        OpenList[temp].F = OpenList[temp].G + OpenList[temp].H;
                    }

                }
            } /* end for*/
        }
        while (OpenList[0].F != -1);   /* Continue until there is no more available square in the Open-List (which means there is no path) */



        if (IsWay == 1)   /* If there is at least one way to the destination square*/
        {
            /* Now all the algorithm has to do is go backwards to figure out the final path! */
            m = 0;
            CurrentSquare.x = ClosedList[N - 1].x;
            CurrentSquare.y = ClosedList[N - 1].y;
            TempG = ClosedList[N - 1].G;
            do
            {
                if (m > 0)
                {
                    FinalPath[m - 1].x = CurrentSquare.x;
                    FinalPath[m - 1].y = CurrentSquare.y;
                }

                /* Retrieve all its walkable adjacent squares (Begin:) */
                /* Top: */
                AdjacentSquares[0].x = CurrentSquare.x;
                AdjacentSquares[0].y = CurrentSquare.y - 1;
                /* Left: */
                AdjacentSquares[1].x = CurrentSquare.x - 1;
                AdjacentSquares[1].y = CurrentSquare.y;
                /* Bottom: */
                AdjacentSquares[2].x = CurrentSquare.x;
                AdjacentSquares[2].y = CurrentSquare.y + 1;
                /* Right: */
                AdjacentSquares[3].x = CurrentSquare.x + 1;
                AdjacentSquares[3].y = CurrentSquare.y;
                /* Retrieve all its walkable adjacent squares (End.) */

                for (k = 0 ; k <= 3 ; k++)
                {
                    /* If this adjacent square is not an open square, ignore it (Begin:) */
                    if ( (AdjacentSquares[k].x < 0) ||
                            (AdjacentSquares[k].y < 0) ||
                            (AdjacentSquares[k].x >= Dimension) ||
                            (AdjacentSquares[k].y >= Dimension) ||
                            (MapArray[AdjacentSquares[k].x][AdjacentSquares[k].y] == 2) )
                    {
                        continue; /* Go to the next adjacent square */
                    }
                    /* If this adjacent square is not an open square, ignore it (End.) */

                    IsInClosedList = 0;
                    for (j = 0 ; j < N ; j++)
                    {
                        if ( (AdjacentSquares[k].x == ClosedList[j].x) &&
                                (AdjacentSquares[k].y == ClosedList[j].y) )
                        {
                            IsInClosedList = 1;
                            temp = j;
                            break;
                        }
                    }

                    if (IsInClosedList == 1)   /* If this adjacent square is in the Closed-List */
                    {
                        if ( ClosedList[temp].G == (TempG - 1) )
                        {
                            m++;
                            CurrentSquare.x = ClosedList[temp].x;
                            CurrentSquare.y = ClosedList[temp].y;
                            TempG = ClosedList[temp].G;
                            break;

                        }
                    }
                } /* end for*/
            }
            while (TempG != 0);

            /* Copy MapArray to SolvedMapArray (Begin:)*/
            for (i = 0 ; i < Dimension ; i++)
            {
                for (j = 0 ; j < Dimension ; j++)
                {
                    SolvedMapArray[i][j] = MapArray[i][j];
                }
            }
            /* Copy MapArray to SolvedMapArray (End.)*/

            /* Write FinalPath on the SolvedMapArray (Begin.)*/
            for (i = 0 ; i <= (m - 2) ; i++)
            {
                SolvedMapArray[FinalPath[i].x][FinalPath[i].y] = 1;
            }
            /* Write FinalPath on the SolvedMapArray (End.)*/
        } /* end if*/
    } /* end else*/
    return IsWay;
}
