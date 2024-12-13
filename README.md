# RayLib Project - 8 Queens 🚀  
<img src="/8-queens.png" alt="Project Logo" width="300"/>
  
[![Contributors](https://img.shields.io/github/contributors/dubystev/8-queens-RayLib)](https://github.com/dubystev/8-queens-RayLib/graphs/contributors)
[![License](https://img.shields.io/github/license/dubystev/8-queens-RayLib)](LICENSE)  
[![Stars](https://img.shields.io/github/stars/dubystev/8-queens-RayLib)](https://github.com/dubystev/8-queens-RayLib/stargazers)  

---

## Table of Contents
1. [About the Project](#about-the-project)  
2. [Features](##features)  
3. [Getting Started](#getting-started)  
   - [Prerequisites](#prerequisites)  
   - [Installation](#installation)  
4. [Short Explanation](#short-explanation)
   - [Evolutionary Process](#evolutionary-process)
   - [Animation](#animation)
   - [Computing the Fitness of a Chromosome](#computing-the-fitness-of-a-chromosome)

---

## About the Project 💡
**8-queens-RayLib** is a is an implementation of the Genetic Algorithm (GA) to solve the 8-Queens problem. The 8-Queens problem is visualized using **raylib**. Each queen is represented 
by an image (3399_black-queen.png), and the chessboard is drawn on the screen, updating after each generation. The goal is to place eight (8) queens on a chess board, each one 
occupies a column, such that [there is no attacking pair](https://en.wikipedia.org/wiki/Eight_queens_puzzle). 

A depiction of a solution without any conflict (i.e., no attacking pair) can be found in the next image.

![Project Screenshot](/FourthCapture.PNG)

### Built With 🛠️  
- [C - Indirectly](https://www.iso-9899.info/wiki/The_Standard)
- [C++ 17](https://en.cppreference.com/w/cpp/17)
- [RayLib](https://github.com/raysan5/raylib)
- [CLion IDE](https://www.jetbrains.com/clion/)

---

## Features ✨  
- Feature 1: The basic Genetic algorithm optimiser from scratch, implementing major steps of the popular evolutionary algorithm
- Feature 2: Animation of board configurations after every generation  
- Feature 3: Very limited statistics of the evolutionary process  

---

## Getting Started 🚀  

### Prerequisites 📋  
Before you begin, ensure you have the following installed:  
- **Requirement 1**: CLion or Visual Studio Code (did not test it on VS Code though)  
- **Requirement 2**: Compiled with C++ 17

### Installation ⚙️  
Follow these steps to install and set up the project (for CLion): 
1. First, open your terminal.
2. Clone the GitHub repository to your local machine:
   ```bash
   git clone https://github.com/dubystev/8-queens-RayLib.git
3. Open the Project in CLion
4. Build/Run the Project (Dependencies might be installed for the first time)

## Short Explanation
This provides a non-exhaustive description of the source code within the [main souce file](https://github.com/dubystev/8-queens-RayLib/blob/main/_8_queens_problem.cpp)

### Evolutionary Process
This is the main driver of the chess board animation. The chess board is displayed based on the current configuration in the placements[] array. For example, four snapshots of a sample 
run have been provided below; where one of them (the last one) represents the configuration where no queen attacks another.

<img src="/FirstCapture.PNG" alt="Config 1" width="300"/> <img src="/SecondCapture.PNG" alt="Config 2" width="300"/> <br/>
<img src="/ThirdCapture.PNG" alt="Config 3" width="300"/> <img src="/FourthCapture.PNG" alt="Config 4" width="300"/>

The code snippet below demonstrates the structure of the placement array, usually captured by the **best_ind**, i.e., best individual variable. The last four expressions typify the
contents of the array for the four depicted generations of the GA run respetively.

```Cpp
constexpr int chromosome_length = 8;
int best_ind[chromosome_length]; // represents the best chromosome evolved so far
best_ind[chromosome_length] = {3, 5, 1, 2, 6, 7, 3, 0} // 4 attacking pairs at Generation 1
best_ind[chromosome_length] = {5, 2, 0, 5, 7, 4, 1, 3} // 1 attacking pair at Generation 65
best_ind[chromosome_length] = {5, 2, 0, 5, 7, 4, 1, 3} // 1 attacking pair at Generation 100
best_ind[chromosome_length] = {5, 3, 6, 0, 2, 4, 1, 7} // 0 attacking pair at Generation 200
```

### Animation
After drawing the chess board and the tiles, the placeQueens function is responsible for drawing the queen image on the tiles of the chess board based on the contents of the 
**placements[]** array.

```Cpp
/**
 * @brief The function places the eight queens on the chess board based on the current configuration captured in the
          @placements parameter
 * @param theTexture the texture to be rendered on the tiles of the chess board
 * @param tileW tile width
 * @param tileH tile height
 * @param placements the current configuration of a (partial) solution to the 8-queens problem
*/
void placeQueens(const Texture& theTexture, const int placements[], int startX, int startY, int tileW, int tileH)
{
    int textureWidth = theTexture.width;
    int textureHeight = theTexture.height;
    int delta_x = (tileW - textureWidth) / 2;
    int delta_y = (tileH - textureHeight) / 2;

    for(int i = 0; i < 8; i++)
    {
        int row_id = placements[i];
        int textureX = delta_x + (i * tileW + startX);
        int textureY = delta_y + (row_id * tileH + startY);
        DrawTexture(theTexture, textureX, textureY, BLACK);
    }
}
```

The first two lines of the function capture the width and height of the texture (the queen image is captured by the **theTexture** variable). The next two variables 
**delta_x** and **delta_y** stores the **initial** x- and y-coordinates for the texture. The values were computed with the goal of placing the queen image at the centre of a tile.

However, the actual placement of each queen depends on the value within the **placement[]** array for that queen. the variable **row_id** captures the row where a queen in the i-th
column is placed. The for-loop iterates through the placement array, typifying that there are eight queens on the chess board. Therefore, from the array: {5, 2, 0, 5, 7, 4, 1, 3}, we 
can deduce that queen 3 (in column 2) has a row id of 0 while queen 8 (in column 7) has a row id of 3.

To compute textureX (the x-coordinate of each texture) and textureY (the y-coordinate of each texture), we use **delta_x**, **i**, **tileW**, and **startX**. The last two variables 
representing variables passed in as arguments. For instance, the value of textureX and textureY values for the third queen will be computed as follows if the following expressions are
true:

```Cpp
startX = 0 //the x-coordinate of the very first tile on each row (the far-left tile)
startY = 100 //the y-coordinate of the very first tile on each column (the topmost tile)
tileW = 43 // the width of each tile
tileH = 37 // the height of each tile
```

```Cpp
textureX = delta_x + (i * tileW + startX)
delta_x = (tileW - textureWidth) / 2
delta_x = (43 - 25) / 2 = 18 / 2 = 9
textureX = 9 + (2 * 43 + 0) = 9 + 86 = 95
```

The expression: **i * tileW** makes sure the two previous tiles (**when i = 0 and i = 1**) are put into consideration when determining the x-coordinate of the queen texture. Since the 
width of the tile (tileW) is 43 px, we would want to make sure that the queen placement for the third tile has its X coordinates based on the combined widths of the first two tiles
(**2 * 43**), and at the same time  occupying the centre of the third tile (determined by delta_x). Therefore, we arrive at a value of 95 for third queen (at column2, row 0).

The startX variable (value is 0 for this program) just represents the the x-coordinate of the first tile on every row. This means that there is no reserved space for any object on the
drawing screen. Reason why the first blue/white patch for every row sits at the extreme left of the screen. See the previous sample snapshots of the animation screen to confirm this.

```Cpp
textureY = delta_y + (row_id * tileH + startY)
delta_y = (tileH - textureHeight) / 2
delta_y = (37 - 25) / 2 = 12 / 2 = 6
textureY = 6 + (0 * 37 + 100) = 6 + (0 + 100)
textureY = 6 + 100 = 106
```

A similar procedure is employed to compute the y-coordinate for the texture (textureY) as shown in the worked example above. First, delta_y (to determine the amount of vertical shifts
in order to vertically centralise the queen texture) is computed and the expression evaluates to 6. Eventually **row_id * tileH** helps to determine the amount of pixels to traverse
downwards; in this case, it is 37 px multiplied by the amount of existing tiles before the current tile along the vertical axis (in this case, **0**, since there was no tile before 
this one).

Conversely, for region 2 with a **row id** of 3, textureY would be 6 + (3 * 37 + 100) = 6 + 211 = 217 px due to the fact that there were three tiles before the tile in **region 2**. 
With their combined heights of 3 * 37 = 111 px, there would be a need to avoid placement of the queen within the 0...211 px space of the vertical axis. We add 100 px to 111 px because of
the value of startY = 100 px (which signifies that the first tile on the vertical axis has a y-coordinate of 100 px), makes sense that to factor that in for the subsequent placements of
more tiles along the vertical axis.

For region 1, circled by red ink in the image below, the queen texture will be placed at the coordinate (95, 106)
<img src="/illustration.PNG" alt="Illustration" width="300"/>

### Computing the Fitness of a Chromosome
The fitness of a chromosome is computed as follows:
```Cpp
/**
 * @brief The function computes the fitness of a chromosome representing a solution to the 8-queens problem.
          It is defined as #pairs - #attacking_pairs
 * @param placements the current configuration of a (partial) solution to the 8-queens problem
 * @return an integer, representing the fitness value of the chromosome @placements
 */
int computeFit(int placements[])
{
    int no_of_pairs = 28;
    int no_attack_pairs = 0;  // if no_attack_pairs = 0 is the optimal situation
    for(int i = 0; i <= 7; i++)
    {
        for(int j = i + 1; j <=7; j++)
        {
            if(placements[i] == placements[j] || same_diagonal(placements, i, j))  // same row attack
                no_attack_pairs += 1;
        }
    }

    return no_of_pairs - no_attack_pairs;
}
```

The final expression that stands out is **return no_of_pairs - no_attack_pairs;**, which subtracts the number of attacking pairs from the actual number of pairs (number of ways to 
select a pair of values in the set {0, 1, 2, ..., 7}). The total number of pairs is 28, therefore, any solution with zero (0) attacking pairs would result in a fitness value of 28.
In this case, we have an optimal solution. Computing the number of attacking pairs have been dealt with in another material. I have provided a link to the material below:
[Visit Link](https://github.com/dubystev/Teaching/blob/main/AI%20class/Explanation%20Sheet%20-%20SA%20implementation.pdf)

Evolution stops when we finally evolve an individual with a fitness value of 28 (see the fourth figure under the [Evolutionary Process](#evolutionary-process) section) or we fail to 
find a new global-best solution for 700 trials (defined by the **max_stuck** variable in the source code).
