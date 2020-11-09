#include <bits/stdc++.h>

using namespace std;

vector<string> split_string(string);

////////////////////////////////////////////////////////////////////////////////////////
//this is not a binary tree so we make a vector of pointers. We do not specfically have 
//a pointer to the "head" of this node, since it isn't really meaningful in an unrooted tree. 
//Rather, the "head" is included in the vector of pointers. That is why each edge is 
//recorded twice (i.e. we record 1->2 and 2->1, for effectively a double-link)
//Definition of unrooted tree means no cycles, and that there are no isolated nodes  
class unrooted_node {

private:
    int color;                                  //or colour, if located in Australia
    int node_num;
    vector<shared_ptr<unrooted_node>> adjacent_node_list; //not a binary tree, so we make a list of "n" pointers to children
    unordered_map<int, unordered_set<int>> color_distance_cache;    //maps colors seen between this node and any other node (the other node is the key)
    
public:
    unrooted_node() {                           //default constructor
        color = 0;
        node_num = 0;
    };
    unrooted_node(int input_node_num, int input_color) {  //actual constructor used
        node_num = input_node_num;
        color = input_color;  
        
        //initialize color distance to "myself" the node color
        unordered_set<int> first_set;
        first_set.insert(input_color);
        color_distance_cache.insert({input_node_num, first_set});
    };
    ~unrooted_node(){};
    
    void add_child(shared_ptr<unrooted_node> new_child) {
        //We have an asumption of data quality, based on the test instructions.
        //1. No loops/cycles
        //2. No self-pointers
        //3. No orphaned nodes
        //4. No duplicate entries
        //In a real-world situation, we probably add a test for these conditions     
        adjacent_node_list.push_back(new_child);  //add this pointer to the list of "children"
    }
    
    int get_color() {
        return(color);
    };
        
    int get_node_num() {
        return(node_num);
    }
    
    int get_color_distance(int node) {
        return(color_distance_cache[node].size());
    }
        
    vector<shared_ptr<unrooted_node>> get_adjacent_nodes() {
        return(adjacent_node_list);
    }
    
    unordered_map<int, unordered_set<int>> get_color_distance_map_cache(){
        return(color_distance_cache);
    }
    void set_color_distance_map_cache(unordered_map<int, unordered_set<int>> new_vector){
        color_distance_cache = new_vector;
    }
    void add_to_color_distance_map_cache(int other_node_num, unordered_set<int> new_mappings) {
        color_distance_cache.insert({other_node_num, new_mappings});
    }
    
};
vector<shared_ptr<unrooted_node>> nodelist;


///////////////////////////////////////////////////////////////////////////////////////////////////
//recursive program to find tree elements and save
void span_graph_and_count(shared_ptr<unrooted_node> entry_point, int prior_node_num) {
    
    vector<shared_ptr<unrooted_node>> adjacent_nodes = entry_point->get_adjacent_nodes();
   
    //A self-mapping for each node already exists, as it was created at instantiation. 
    //as long as we aren't at the head, find the previous upstream node, clone its color distance vector
    //to this vector, but first adding this node's color before saving each value
    if (prior_node_num > 0){
        unordered_map<int, unordered_set<int>> last_nodes_mapping = nodelist[prior_node_num-1]->get_color_distance_map_cache(); 
 
        for (auto vector_from_upstream_node: last_nodes_mapping) {   
            vector_from_upstream_node.second.insert(entry_point->get_color());       
            entry_point->add_to_color_distance_map_cache(vector_from_upstream_node.first, vector_from_upstream_node.second);
            
            //mappings are symetrical. Therefore the color distance from a->b is identical to the color distance from b->a
            //This code inserts the symetrical mapping
            nodelist[vector_from_upstream_node.first-1]->add_to_color_distance_map_cache(entry_point->get_node_num(), vector_from_upstream_node.second);
        }
    }
    
    //identify color distance for downstream nodes    
    for (shared_ptr<unrooted_node> adjacent_node: adjacent_nodes) {
        if (adjacent_node->get_node_num() != prior_node_num) {    //This exists to prevent loopback, i.e a->b->a
            span_graph_and_count(adjacent_node, entry_point->get_node_num()); 
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//main program
vector<int> solve(int n, vector<int> c, vector<vector<int>> tree) {
 
    shared_ptr<unrooted_node> entry_point;                      //This will be the entry point.
                                                                //see https://en.wikipedia.org/wiki/Unrooted_binary_tree 
                                                                //(although this problem is not defined to be "binary", tree, just a tree)
                                                                //entry_point will be assigned from 1 to n as part of count.
    
    
    //Since we know the number of nodes, we will instantiate the nodes first THEN place them in the tree second.
    //This will ensure that the parent/child nodes exist at the point we create the edges
    //(i.e. there is a danger if the first two edges are 1<->2 and 5<->6, that nodes 5 & 6 may not exist,
    //or will become inaccessible nodes since there is nothing at that point to link them to 1 & 2)
    //This will also be useful later in that 
    //This initial loop is an O(n) operation, so acceptable timewise.
    for (int i=1; i <= n; i++) {   //1-based, not 0-based
        shared_ptr<unrooted_node> newnode{ shared_ptr<unrooted_node>(new unrooted_node(i, c[i-1])) };
        nodelist.push_back(newnode);
    }

    //build tree
    //We are now going to add the edges to each node to build the tree. The note that we need to be careful that the data is 1-based,
    //not 0-based.
    for (vector<int> subtree: tree) {
        int to = subtree[0];
        int from = subtree[1];
     
        //create a double-link 
        nodelist[to-1]->add_child(nodelist[from-1]);
        nodelist[from-1]->add_child(nodelist[to-1]);
    }
    
    //actual recursive vector scanning and counting
    span_graph_and_count(nodelist[0], 0);                                   
    
    vector<int> output_vector;
    //Now we loop through Σd(i,j)
    for (int i = 0; i < n; i++) {
        int colors_seen = 0;
 
        //search the chache-ed data
        for (int j = 0; j < n; j++){
            colors_seen += nodelist[i]->get_color_distance(j+1);
        }

        output_vector.push_back(colors_seen);
    }
    
    return(output_vector); 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));

    int c_count;
    cin >> c_count;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string c_temp_temp;
    getline(cin, c_temp_temp);

    vector<string> c_temp = split_string(c_temp_temp);

    vector<int> c(c_count);

    for (int c_itr = 0; c_itr < c_count; c_itr++) {
        int c_item = stoi(c_temp[c_itr]);

        c[c_itr] = c_item;
    }

    vector<vector<int>> tree(c_count-1);                                    //note in the original code "c_count -1" was listed as "n-1". Bug fixed.
    for (int tree_row_itr = 0; tree_row_itr < c_count-1; tree_row_itr++) {  //note in the original code "c_count -1" was listed as "n-1". Bug fixed

        tree[tree_row_itr].resize(2);

        for (int tree_column_itr = 0; tree_column_itr < 2; tree_column_itr++) {
            cin >> tree[tree_row_itr][tree_column_itr];
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    vector<int> result = solve(c_count, c, tree);

    for (int result_itr = 0; result_itr < result.size(); result_itr++) {
        fout << result[result_itr];

        if (result_itr != result.size() - 1) {
            fout << "\n";
        }
    }

    fout << "\n";

    fout.close();

    return 0;
}

vector<string> split_string(string input_string) {
    string::iterator new_end = unique(input_string.begin(), input_string.end(), [] (const char &x, const char &y) {
        return x == y and x == ' ';
    });

    input_string.erase(new_end, input_string.end());

    while (input_string[input_string.length() - 1] == ' ') {
        input_string.pop_back();
    }

    vector<string> splits;
    char delimiter = ' ';

    size_t i = 0;
    size_t pos = input_string.find(delimiter);

    while (pos != string::npos) {
        splits.push_back(input_string.substr(i, pos - i));

        i = pos + 1;
        pos = input_string.find(delimiter, i);
    }

    splits.push_back(input_string.substr(i, min(pos, input_string.length()) - i + 1));

    return splits;
}

