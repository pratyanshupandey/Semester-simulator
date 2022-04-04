#ifndef __SIMULATOR__
#define __SIMULATOR__

#include <bits/stdc++.h>
#define ll long long int
#define ld long double

using namespace std;

/*
The class represents the game that is built on the given configurations.
The class simulates every strategy profile of the players and stores all 
calculated utilities in a new output file.
N = Number of students
C = Number of Courses
H = Total number of hours available to each student.
grade_val = value attached by a student to his/her SGPA
fun_val = value attached by a student to the amount of fun
lr = learning rate
*/

class Game
{
private:
    int N; 
    int C;
    int H;
    vector<float> grade_val;
    vector<float> fun_val;
    vector <vector<float>> lr;
    vector<vector<pair<int, int>>> strategy_space;
    vector<vector<float>> utilities;

public:
    Game(string config_file);
    void simulate();
    vector <vector <pair<int,int>>> player_strategies();
    void hour_distribution(int h, int c, vector<int> temp, vector<vector<int>> &result);
    vector <vector <int>> item_distribution(int c, int n);
    vector <float> utility(vector<int> profile);
    float new_lr(float lr_self, float lr_partner);
    vector <float> get_grades(vector <float> marks);
    void print_strategy_map(string output_file);
    void print_utilities(string output_file);
    void print_nfg_format(string output_file);
};

#endif