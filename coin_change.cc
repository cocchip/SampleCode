#include <bits/stdc++.h>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);
vector<string> split(const string &);
vector<long> coins;

/////////////////////////////////////////////////////////////////////////// 
class solutions {
    
    set<multiset<long>> solutions_total;
    
    public:
    solutions() {}
    ~solutions(){}
    
    set<multiset<long>> get_solutions_total_set() {
        return(solutions_total);
    }
    
    long get_solutions_size() {
        return((long) solutions_total.size());
    }
    
    void set_solutions_list(set<multiset<long>> new_solutions_total) {
        solutions_total = new_solutions_total; 
    }
};
map<int, solutions> completed_solutions;    //this is where we cache the solved sets 
/////////////////////////////////////////////////////////////////////////////
//recursive search for solutions, leveraging solution caching into "completed_solutions"
 set<multiset<long>> search_breakdown(int n) {
    set<multiset<long>> popped_solutions;
    
    if (n<=0) {
        printf("Error: Reached an iteration with negative n \n");
        //error, should not happen
    } else {
        for (long coin: coins) {
            
            if (n-coin < 0) {
                //nothing to do for invalid solution
            } else if (completed_solutions[n-coin].get_solutions_size() > 0) {
                //a solution exists for this coin
                set<multiset<long>> returnsets = completed_solutions[n-coin].get_solutions_total_set();

                for (multiset<long> const &returnset : returnsets){
                    ((multiset<long> &) returnset).insert(coin); 
                 }
                popped_solutions.insert(returnsets.begin(), returnsets.end());
            } else if (n-coin == 0) {
                multiset<long> solutionset;
                solutionset.insert(coin);               //in this case, the solution set is one coin

                popped_solutions.insert(solutionset);
            } else {
                set<multiset<long>> returnsets = search_breakdown(n-coin);
                for (multiset<long> const &returnset : returnsets){
                    ((multiset<long> &) returnset).insert(coin); 
                 }
                popped_solutions.insert(returnsets.begin(), returnsets.end());
            } 
        }
    }
    completed_solutions[n].set_solutions_list(popped_solutions);     
    return(popped_solutions);     
 }
 
///////////////////////////////////////////////////////////////////////////

long getWays(int n, vector<long> c) {
    long num_solutions;
    coins = c;
 
    set<multiset<long>> final_list_solutions = search_breakdown(n);  
    num_solutions = completed_solutions[n].get_solutions_size();

    return(num_solutions);
}

///////////////////////////////////////////////////////////////////////////

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));

    string first_multiple_input_temp;
    getline(cin, first_multiple_input_temp);

    vector<string> first_multiple_input = split(rtrim(first_multiple_input_temp));

    int n = stoi(first_multiple_input[0]);

    int m = stoi(first_multiple_input[1]);

    string c_temp_temp;
    getline(cin, c_temp_temp);

    vector<string> c_temp = split(rtrim(c_temp_temp));

    vector<long> c(m);

    for (int i = 0; i < m; i++) {
        long c_item = stol(c_temp[i]);

        c[i] = c_item;
    }

    // Print the number of ways of making change for 'n' units using coins having the values given by 'c'

    long ways = getWays(n, c);

    fout << ways << "\n";

    fout.close();

    return 0;
}

string ltrim(const string &str) {
    string s(str);

    s.erase(
        s.begin(),
        find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace)))
    );

    return s;
}

string rtrim(const string &str) {
    string s(str);

    s.erase(
        find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(),
        s.end()
    );

    return s;
}

vector<string> split(const string &str) {
    vector<string> tokens;

    string::size_type start = 0;
    string::size_type end = 0;

    while ((end = str.find(" ", start)) != string::npos) {
        tokens.push_back(str.substr(start, end - start));

        start = end + 1;
    }

    tokens.push_back(str.substr(start));

    return tokens;
}
