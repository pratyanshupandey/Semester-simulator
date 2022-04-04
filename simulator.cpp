#include "simulator.h"

/*
Input file format
N
C
H
grade_val
fun_val
lr
*/
Game::Game(string config_file)
{
    ifstream fin(config_file, ios::in);
    if (fin.is_open())
    {
        fin >> N;
        fin >> C;
        fin >> H;

        grade_val.resize(N);
        fun_val.resize(N);
        lr.resize(N, vector<float>(C));

        for (uint i = 0; i < N; i++)
            fin >> grade_val[i];

        for (uint i = 0; i < N; i++)
            fin >> fun_val[i];

        for (uint i = 0; i < N; i++)
            for (uint j = 0; j < N; j++)
                fin >> lr[i][j];

        fin.close();
        cout << "Configuration Loaded successfully.\n";

        strategy_space = player_strategies();
        cout << "Strategy Space created successfully of size " << strategy_space.size() << ".\n";
    }
    else
        cerr << "Unable to open config file.\n";
}

// Express H as a sum of C + 1 vars -  first C vars for courses and last for fun time
void Game::hour_distribution(int h, int c, vector<int> temp, vector<vector<int>> &result)
{
    if (c == 0)
    {
        temp.push_back(h);
        result.push_back(temp);
        return;
    }
    for (int i = 0; i <= h; i++)
    {
        temp.push_back(i);
        hour_distribution(h - i, c - 1, temp, result);
        temp.pop_back();
    }
}

// all possible combinations for filling c slots with n items each slot
vector<vector<int>> Game::item_distribution(int c, int n)
{
    // C++ pow gives inaccurate results in some cases
    // so taking power manually
    unsigned long long int dist_count = 1;
    for (uint i = 0; i < c; i++)
        dist_count *= n;

    vector<vector<int>> item_dist(dist_count, vector<int>(c));

    for (unsigned long long int i = 0; i < dist_count; i++)
    {
        unsigned long long int mask = i;
        for (uint j = 0; j < c; j++)
        {
            item_dist[i][j] = mask % n;
            mask /= n;
        }
    }
    return item_dist;
}

/*
Get all the strategies of the players
Each strategy by a student is represented by a vector where each element
of the vector represents strategy of a course.
Each element is a pair of the form (study partner, hours). If the student is studying alone,
then study partner is the student himself.
*/
vector<vector<pair<int, int>>> Game::player_strategies()
{
    vector<vector<pair<int, int>>> strategies;

    vector<vector<int>> hour_dist;
    vector<int> temp;
    hour_distribution(this->H, this->C, temp, hour_dist);
    vector<vector<int>> partner_dist = item_distribution(C, N);

    for (uint i = 0; i < hour_dist.size(); i++)
        for (uint j = 0; j < partner_dist.size(); j++)
        {
            vector<pair<int, int>> strategy;
            for (uint course = 0; course < C; course++)
                strategy.push_back({partner_dist[j][course], hour_dist[i][course]});
            strategies.push_back(strategy);
        }

    return strategies;
}

// Calculate the new learning rate for self when partnering with partner
float Game::new_lr(float lr_self, float lr_partner)
{
    if (lr_self <= lr_partner)
        return (lr_self + lr_partner) / 2;
    return (lr_self + lr_partner) / 2 + (lr_self - lr_partner) / 2;
}

/*
Given marks for a course allocate the grades to the students
using relative grading
*/
vector<float> Game::get_grades(vector<float> marks)
{
    vector<pair<float, int>> marks_player;
    for (uint i = 0; i < marks.size(); i++)
        marks_player.push_back({marks[i], i});

    sort(marks_player.begin(), marks_player.end());

    vector<float> grades(N);
    for (uint i = 0; i < marks_player.size(); i++)
    {
        float mark = marks_player[i].first;
        int student = marks_player[i].second;
        float val = i / N;
        if (mark < 33)
            grades[student] = 0;
        else if (val < 0.05)
            grades[student] = 10;
        else if (val < 0.15)
            grades[student] = 9;
        else if (val < 0.35)
            grades[student] = 8;
        else if (val < 0.65)
            grades[student] = 7;
        else if (val < 0.85)
            grades[student] = 6;
        else if (val < 0.95)
            grades[student] = 5;
        else
            grades[student] = 4;
    }
    return grades;
}

/*
Calculate the utility of each player if they follow
strategy accoring to the given profile
*/
vector<float> Game::utility(vector<int> profile)
{
    vector<float> utility(N);

    // add fun value to utility
    for (uint i = 0; i < N; i++)
    {
        int hour_sum = 0;
        for (uint c = 0; c < C; c++)
            hour_sum += this->strategy_space[profile[i]][c].second;
        utility[i] = this->fun_val[i] * (this->H - hour_sum);
    }

    // preparing course marks and grading according to it
    // then adding grade utility to get the total utility
    vector<float> marks(N);
    vector<float> grades;
    for (uint c = 0; c < C; c++)
    {
        
        for (uint i = 0; i < N; i++)
        {
            float self_lr = lr[i][c];
            int partner = strategy_space[profile[i]][c].first;
            float lr_new;
            // If partner's partner is myself i.e. he too has signed the contract with me
            // for the same number of hours then the learning rate changes otherwise it remains the same.
            if(strategy_space[profile[partner]][c].first == i)
                lr_new = new_lr(self_lr, lr[partner][c]);
            else 
                lr_new = self_lr;

            int my_hours = strategy_space[profile[i]][c].second;
            int partner_hours = strategy_space[profile[partner]][c].second;

            // If different timings, the lr is changed for group studies and remains same rest pf the time
            if(my_hours > partner_hours)
                marks[i] = partner_hours * lr_new + (my_hours - partner_hours) * self_lr;
            else
                marks[i] = my_hours * lr_new;
        }

        grades = get_grades(marks);

        // Equal weightage to every course so for SGPA we just divide by C.
        for (uint i = 0; i < N; i++)
            utility[i] += grade_val[i] * grades[i] / C;
    }

    return utility;
}


// Print the mapping from index to strategy in the strategy space
// Each line is a strategy which is a pair of (partner, no_of_hours) for each course
void Game::print_strategy_map(string output_file)
{
    ofstream fout(output_file, ios::out);
    if(fout.is_open())
    {
        for (uint i = 0; i < strategy_space.size(); i++)
        {
            fout << i << " ->";
            for (uint j = 0; j < C; j++)
                fout << " (" << strategy_space[i][j].first << "," << strategy_space[i][j].second << ")";
            fout << endl;
        }
        fout.close();
        cout << "Strategy Map Saved.\n";
    }
    else
        cerr << "Error in opening output file for saving strategy map" << output_file << endl;
}

// Print the utilities of each player for a given strategy profile
// The format is strategy profile index followed by utility of each player
// Profile index works in a similar way like couting with base |Strategy Space|
void Game::print_utilities(string output_file)
{
    ofstream fout(output_file, ios::out);
    if(fout.is_open())
    {
        for (uint i = 0; i < utilities.size(); i++)
        {
            fout << i;
            for (uint j = 0; j < N; j++)
                fout << " " << utilities[i][j];
            fout << endl;            
        }
        fout.close();
    }
    else
        cerr << "Error in opening output file for saving utilities" << output_file << endl;
}

// Print the strategic form representation of the game in Gambit nfg format
void Game::print_nfg_format(string output_file)
{
    ofstream fout(output_file, ios::out);
    if(fout.is_open())
    {
        fout << N << endl;
        for (uint i = 0; i < N; i++)
            fout << strategy_space.size() << " ";
        fout << endl;

        // convert from normal index to reverse counting index used in nfg
        for (uint nfg_idx = 0; nfg_idx < utilities.size(); nfg_idx++)
        {
            uint idx = 0;
            uint multiplier = 1;
            uint mask = nfg_idx;
            for (uint i = 0; i < N - 1; i++)
                multiplier *= strategy_space.size();
            for (uint i = 0; i < N; i++)
            {
                idx += (mask % strategy_space.size()) * multiplier;
                mask /= strategy_space.size();
                multiplier /= strategy_space.size();
            }
            for (uint i = 0; i < N; i++)
                fout << utilities[idx][i] << " ";
            printf("\rSaved %d out of %d utility values.", nfg_idx + 1, utilities.size());
        }
        fout.close();
        cout << "\nNFG Format Saved.\n";
    }
    else
        cerr << "Error in opening output file for saving nfg format" << output_file << endl;
}

// Calculate utilities for all strategy profiles
void Game::simulate()
{
    unsigned long long int dist_count = 1;
    for (uint i = 0; i < N; i++)
        dist_count *= strategy_space.size();

    vector<int> item_dist(N);

    for (unsigned long long int i = 0; i < dist_count; i++)
    {
        unsigned long long int mask = i;
        for (uint j = 0; j < N; j++)
        {
            item_dist[j] = mask % strategy_space.size();
            mask /= strategy_space.size();
        }
        utilities.push_back(this->utility(item_dist));
        printf("\rProfile %d out of %d calculated.", i + 1, dist_count);
    }
    cout << "\nFinished\n";
}
