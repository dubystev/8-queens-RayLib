//
// Created by Stephen A. Adubi on 08/12/2024.
//

#include <algorithm>
#include <iostream>
#include <random>
#include <array>
#include "raylib.h"

int best_; // the index of the local-best chromosome
int gen = 0; // number of generations elapsed so far
int stuck = 0; // the number of consecutive iterations no new global-best chromosome was found
int max_stuck = 700; // number of stuck(s) allowed before termination of the GA program
int fit_best_; // fitness of the global-best chromosome
constexpr int chromosome_length = 8;
constexpr int popSize = 50; // population size
constexpr float mu_rate = 0.2; // mutation rate
int xover_point; // the cross-over point when copying the chromosome information into offspring
int best_ind[chromosome_length]; // represents the best chromosome evolved so far
int chromosomes[popSize][chromosome_length]; /* captures the row-position of each queen in a column for @popSize individuals */
std::pair<int, int> fitness[popSize]; // fitness of the @popSize individuals, the 'key' is the index of the chromosome
std::pair<int, int> fitness_off[popSize/2]; // fitness of the @popSize/2 individuals, the offspring of several re-combinations

// random variate generation
std::random_device rd;                    // Random device (non-deterministic)
std::mt19937 generator(rd());            // Mersenne Twister RNG seeded with random device

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

/**
 * @brief Checks if there is a diagonal attack between two queens in a current 8-queens configuration
 * @param placements the current configuration of a (partial) solution to the 8-queens problem
 * @param i the position of the first queen in the check operation
 * @param j the position of the second queen in the check operation
 * @return a boolean value representing if the two queens in positions i and j diagonally attack each other
 */
bool same_diagonal(const int placements[], int i, int j)
{
    int abs_diff_a = i - placements[i];
    int abs_diff_b = j - placements[j];
    int sum_a = i + placements[i];
    int sum_b = j + placements[j];
    if (abs_diff_a == abs_diff_b || sum_a == sum_b)
        return true;
    return false;
}

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

/**
 * @brief The function implements roulette-wheel selection for the GA procedure.
 *
 * The function is usually called twice for a single crossover operation. During the first call, the default value
 * of the default parameter is used. During the second call, a value is passed in for the parameter: @first to avoid
 * the chromosome represented by @first from being selected again.
 *
 * The operation described in the previous paragraph makes sure two separate individuals are chosen as parents during
 * every crossover operation.
 * @param first a default parameter with a default value of popSize (the size of the population).

 * @return an integer, representing the index of a selected chromosome
 */
int rws_select(int first = popSize)
{
    std::uniform_real_distribution floatDist(0.0, 1.0);
    int sum_fitness = 0;
    for(int i = 0; i < popSize; i++)
    {
        if(i == first)
            continue;
        sum_fitness += fitness[i].first;
    }

    int select_i = 0;
    if(first == 0)
        select_i++;
    int accumulation = fitness[select_i].first;
    const float rndT = floatDist(generator);
    const float cut_off = rndT * static_cast<float>(sum_fitness);
    while(static_cast<float>(accumulation) < cut_off)
    {
        select_i++;
        if(select_i == first)
            select_i++;
        accumulation += fitness[select_i].first;
    }

    return select_i;
}

/**
 * @brief The function implements a simple mutation procedure for the GA.
 * @param chromosome the chromosome to be mutated
 */
void mutate(int chromosome[chromosome_length])
{
    //mutate (by a probability of @mut_rate)
    std::uniform_int_distribution intDist(0, 7); // Range [0, 7]
    std::uniform_real_distribution floatDist(0.0, 1.0);
    for(int i = 0; i < chromosome_length; i++)
    {
        float rand_value = floatDist(generator);
        if(rand_value < mu_rate)
        {
            int new_val = intDist(generator);
            while(new_val == chromosome[i])
                new_val = intDist(generator);
            chromosome[i] = new_val;
        }
    }
}

/**
 * @brief The function implements the crossover (aka recombination) procedure, utilising the single-point exchange.
 * @param p1 the first parent
 * @param p2 the second parent
 * @return a 2 X CL array is returned, where each sub-array of size CL represents the chromosome of each offspring and
 * CL represents the number of genes in the chromosome (i.e. chromosome_length)
 */
std::array<std::array<int, chromosome_length>, 2> crossover(const int p1[chromosome_length], const int p2[chromosome_length])
{
    std::array<std::array<int, chromosome_length>, 2> offspring = {}; // Initialize offspring

    // perform crossover -- section 1
    for(int i = 0; i < xover_point; i++)
    {
        offspring[0][i] = p1[i];
        offspring[1][i] = p2[i];
    }

    // perform crossover -- section 2
    for(int i = xover_point; i < chromosome_length; i++)
    {
        offspring[0][i] = p2[i];
        offspring[1][i] = p1[i];
    }

    return offspring;
}

/**
 * @brief A utility function that sorts the fitness values of a set of chromosomes in descending order
 * @param arr an array of pairs where each pair<A: int, B:int> is composed of the fitness value of a chromosome (A) and
 * the index of the chromosome (B) that has the value A. The sorting is carried out on the array of pairs based on A.
 * @param size the length of the array
 *
 * For example
 * @code sort({<90, 0>, <78, 1>, <87, 2>, <88, 3>})@endcode
 * produces @code {<90, 0>, <88, 3>, <87, 2>, <78, 1>}@endcode
 */
void sort(std::pair<int, int> arr[popSize/2], const int size)
{
    // Sort in descending order (the array of pairs) based on the first component
    std::sort(arr, arr + size, [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.first > b.first;
    });
}

/**
 * @brief A version of @code EvaluatePop()@endcode function that computes the fitness values of offspring after the
 * crossover operation
 * @param chromosomes a 2D array that represents the chromosomes to be evaluated
 */
inline void EvaluatePop2(int chromosomes[popSize/2][chromosome_length]) // for offspring evaluation
{
    for(int i = 0; i < popSize/2; i++)
    {
        fitness_off[i].first = computeFit(chromosomes[i]);
        fitness_off[i].second = i;
    }
}

/**
 * @brief A function that computes the fitness values of the initial population, only called once.
 * @param chromosomes a 2D array that represents the chromosomes to be evaluated
 */
void EvaluatePop(int chromosomes[popSize][chromosome_length])
{
    int _fit = -1;
    for(int i = 0; i < popSize; i++)
    {
        fitness[i].first = computeFit(chromosomes[i]);
        fitness[i].second = i;
        if(fitness[i].first > _fit)
        {
            _fit = fitness[i].first;
            fit_best_ = _fit;
            best_ = i;
        }
    }
}

/**
 * @brief The function combines the previous population with the offspring population strictly based on their fitness
 * values (i.e., a single member of @code new_pop@endcode might not make it into the next generation
 *
 * You might modify the rules for selecting the members of the next generation if you wish to avoid extreme elitism
 * @param new_pop the 2D array representing the set of offspring to be merged with the existing population
 */
void merge_population(int new_pop[popSize/2][chromosome_length])
{
    EvaluatePop2(new_pop); // evaluate the fitness of the newly created offspring.
    if(gen == 1)
        sort(fitness, popSize); // sort the fitness of the previous generation
    sort(fitness_off, popSize/2); // sort the fitness of the offspring generation
    int old_p = 0; // pointer to the current item in the old population
    int new_p = 0; // pointer to the current item in the offspring population
    int chromosomes_copy[popSize][chromosome_length];
    std::pair<int, int> fitness_copy[popSize]; // create a copy of the fitness structure

    // create copies of the chromosomes and corresponding fitness structures
    for (size_t i = 0; i < popSize; ++i)
    {
        std::copy(chromosomes[i], chromosomes[i] + chromosome_length, chromosomes_copy[i]);
        fitness_copy[i].first = fitness[i].first;
        fitness_copy[i].second = fitness[i].second;
    }

    for(int i = 0; i < popSize; i++)
    {
        if(new_p > popSize/2 - 1 || fitness_copy[old_p].first >= fitness_off[new_p].first)
        {
            std::copy(chromosomes_copy[fitness_copy[old_p].second], chromosomes_copy[fitness_copy[old_p].second] +
                chromosome_length, chromosomes[i]);
            fitness[i].first = fitness_copy[old_p].first;
            old_p++;
        }
        else
        {
            std::copy(new_pop[fitness_off[new_p].second], new_pop[fitness_off[new_p].second] + chromosome_length,
                chromosomes[i]);
            fitness[i].first = fitness_off[new_p].first;
            new_p++;
        }

        if(i == 0)
        {
            best_ = i;
            std::copy(chromosomes[best_], chromosomes[best_] + chromosome_length, best_ind);
            if(fit_best_ == fitness[i].first) stuck++;
            else stuck = 0;
            fit_best_ = fitness[i].first;
        }
        fitness[i].second = i;
    }
}

/**
 * @brief Carries out the re-combination, mutation, and population-merger
 * (@code merge_population()@endcode function) steps for the GA
 */
void reproduce()
{
    int new_pop[popSize/2][chromosome_length]; // create a new set of individuals, half the size of the current population
    int reproduce_count = 0;
    while(reproduce_count < popSize / 2)
    {
        const int first = rws_select();
        const int second = rws_select(first);
        xover_point = 1 + rand() % (chromosome_length - 2);
        // the offsprings container is of type std::array x 2
        auto offsprings = crossover(chromosomes[first], chromosomes[second]);
        std::copy(offsprings[0].begin(), offsprings[0].end(), new_pop[reproduce_count++]);
        mutate(new_pop[reproduce_count - 1]); // attempt mutation
        if(reproduce_count == popSize/2)
            break; // stop spawning when the children population is full
        std::copy(offsprings[1].begin(), offsprings[1].end(), new_pop[reproduce_count++]);
        mutate(new_pop[reproduce_count - 1]); // attempt mutation
    }

    merge_population(new_pop);
    gen++; // update the number of generations completed so far
}

int main()
{
    // set the genetic algorithm variables
    best_ = 0; // assume the first chromosome is the best stuck = 0; gen = 0;

    // set utility variables
    std::uniform_int_distribution dist(0, 7); // Range [0, 7]

    for(int i = 0; i < popSize; i++)
        for(int j = 0; j < chromosome_length; j++)
            chromosomes[i][j] = dist(generator); // set the location for queen i;
    EvaluatePop(chromosomes);
    std::copy(chromosomes[best_], chromosomes[best_] + chromosome_length, best_ind);
    gen++;

    // Initialize the window
    const int screenWidth = 350;
    const int screenHeight = 400;
    const int statDisplayHeight = 100;
    const int res_Height = screenHeight - statDisplayHeight;
    const Color tile_colour[2] = {BLUE, WHITE}; // create an array of possible tile colours
    int colour_idx = 0;
    int tileHeight = res_Height / 8;
    int tileWidth = screenWidth / 8;
    InitWindow(screenWidth, screenHeight, "8-Queens Problem with GA");
    Image image = LoadImage(ASSETS_PATH"3399_black-queen.png"); // Load an image from file

    // Original dimensions
    int originalWidth = image.width;
    int originalHeight = image.height;

    // New dimensions (scale down by 0.1)
    int newWidth = originalWidth / 10;
    int newHeight = originalHeight / 10;

    ImageResize(&image, newWidth, newHeight); // Resize the image
    Texture queen = LoadTextureFromImage(image); // Convert the image to a texture for rendering

    // Unload the original image from memory (no longer needed after creating the texture)
    UnloadImage(image);
    SetTargetFPS(60); // Set the FPS (frames per second)

    // Main game/animation loop
    while (!WindowShouldClose()) {
        int number_of_conflicts = 28 - fit_best_;
        std::string text1 = "Generation: " + std::to_string(gen);
        std::string text2 = "Fitness: " + std::to_string(fit_best_);
        std::string text3 = "Number of Conflicts: " + std::to_string(number_of_conflicts);
        const char* displayText[3];
        displayText[0] = text1.c_str();
        displayText[1] = text2.c_str();
        displayText[2] = text3.c_str();
        int startY = 10; // Starting Y position for the first line
        const int lineSpacing = 3;
        int fontSize = 20;

        // Start drawing
        BeginDrawing();
        ClearBackground(WHITE);
        for(int i = 0; i < 3; i++)
        {
            int textWidth = MeasureText(displayText[i], fontSize);
            int startX = (screenWidth - textWidth) / 2;
            DrawText(displayText[i], startX, startY + i * (fontSize + lineSpacing), fontSize, BLACK);
        }

        // Draw the Chess Board Cells on the Screen...
        int posY = statDisplayHeight;
        int posX = 0;
        for(int i = 0; i < 8; i++)
        {
            for(int j  = 0; j < 8; j++)
            {
                DrawRectangle(posX, posY, tileWidth, tileHeight, tile_colour[colour_idx]);
                if(j < 7)
                    colour_idx = ++colour_idx % 2;
                posX += tileWidth;
            }
            posX = 0; // re-position the x-coordinates for the new set of tiles
            posY += tileHeight; // Set the y-coordinates of the tiles in the next row of the chess board
        }

        placeQueens(queen, best_ind, 0, statDisplayHeight, tileWidth, tileHeight);

        EndDrawing(); // End drawing

        if(number_of_conflicts > 0 && stuck < max_stuck)
            reproduce();
    }
    // Close the window and OpenGL context
    CloseWindow();
    return 0;
}
