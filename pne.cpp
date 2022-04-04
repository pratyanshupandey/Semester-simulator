#include<iostream>
#include<vector>

using namespace std;

double max_ele(vector <double> &v)
{
    double ele = v[0];
    for (int i = 0; i < v.size(); i++)
        ele = max(ele, v[i]);
    return ele;
}

// get the index for the start of the utilities given a strategy_vector
void get_indices(int profile_index, int n_players, vector <int> &s_count, vector <int> &cumulative_s_count, vector <int> &strategy, vector <int> &profile_indices)
{
    int idx = profile_index;
    for (int i = n_players - 1; i >= 0; --i)
    {
        strategy[i] = idx / cumulative_s_count[i];
        idx = idx % cumulative_s_count[i];
    }
    
    for (int i = 0; i < n_players; i++)
        profile_indices[i] = (profile_index % cumulative_s_count[i]) + (profile_index - (profile_index % cumulative_s_count[i]) - (strategy[i] * cumulative_s_count[i])) / (s_count[i]);
    
}

// intersect 2 vectors and return the result in a new vector
vector <int> intersection(vector <int> &v1, vector <int> &v2)
{
    int i = 0, j = 0;
    vector <int> result;
    while(i < v1.size() && j < v2.size())
    {
        if(v1[i] == v2[j])
        {
            result.push_back(v1[i]);
            ++i;
            ++j;
        }
        else if (v1[i] > v2[j])
            ++j;
        else
            ++i;
    }
    return result;
}

int main()
{
    // take input
    int n_players;
    cin >> n_players;

    vector <int> strategy_count(n_players);
    int count = 1;

    for (int i = 0; i < n_players; i++)
    {
        cin >> strategy_count[i];
        count *= strategy_count[i];
    }


    // for each agent buid a 2d matrix where each row has utilities for a given theta_-i and each column has utilities when
    // i plays theta_i
    
    vector <vector<vector<double>>> utilities;
    vector <vector <double>> maxes;

    vector <int> profile_indices(n_players, 0);
    vector <int> strategy(n_players, 0);

    vector <int> cumulative_s_count(n_players);
    cumulative_s_count[0] = 1;
    for (int i = 1; i < n_players; ++i)
        cumulative_s_count[i] = cumulative_s_count[i - 1] * strategy_count[i - 1];
    

    for (int i = 0; i < n_players; i++)
    {
        utilities.push_back(vector<vector<double>>(count/strategy_count[i], vector<double>(strategy_count[i], 0)));
        maxes.push_back(vector <double> (count/strategy_count[i], 0));
    }
    
    for (int i = 0; i < count; i++)
    {
        get_indices(i, n_players, strategy_count, cumulative_s_count, strategy, profile_indices);
        for (int player = 0; player < n_players; player++)
            cin >> utilities[player][profile_indices[player]][strategy[player]];
    }

    for (int player = 0; player < n_players; ++player)
        for (int j = 0; j < utilities[player].size(); ++j)
            maxes[player][j] = max_ele(utilities[player][j]); 
        
    
    vector <vector<int>> psnes;
    bool is_psne;
    for (int i = 0; i < count; i++)
    {
        get_indices(i, n_players, strategy_count, cumulative_s_count, strategy, profile_indices);
        is_psne = true;
        for (int player = 0; player < n_players; player++)
        {
            if(utilities[player][profile_indices[player]][strategy[player]] != maxes[player][profile_indices[player]])
            {
                is_psne = false;
                break;
            }
        }

        if(is_psne)
            psnes.push_back(strategy);
        
    }

    cout << psnes.size() << endl;
    for (int i = 0; i < psnes.size(); i++)
    {
        for (int j = 0; j < psnes[i].size(); j++)
            cout << psnes[i][j] << " ";
        cout << endl;        
    }


    // Code for finding very weak DS 
    for (int player = 0; player < n_players; player++)
    {
        vector <vector <int>> maxval_indices(count/strategy_count[player]);
        for (int i = 0; i < utilities[player].size(); i++)
        {
            for (int j = 0; j < utilities[player][i].size(); j++)
            {
                if(utilities[player][i][j] == maxes[player][i])
                    maxval_indices[i].push_back(j);
            }
            
        }

        vector <int> result(maxval_indices[0].begin(), maxval_indices[0].end());
        for (int i = 1; i < utilities[player].size(); i++)
            result = intersection(result, maxval_indices[i]);
        
        cout << result.size() << " ";
        for (int i = 0; i < result.size(); i++)
            cout << result[i] << " ";
        cout << endl;
    }
    

    return 0;
}