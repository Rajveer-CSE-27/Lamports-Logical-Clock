#include<bits/stdc++.h>
using namespace std;

string s;

// Function to simulate Lamport's distributed mutual exclusion algorithm
void lamport(multimap<int,string> &order, vector<vector<string>> &sentMessages, 
                map<string,vector<string>> &instructions, map<string,string> &deadlock,
                vector<string> &processes, int &clock, string curProcess, bool &lock){
    
    int n = processes.size();
    int index; // Index to track the current instruction for the process
    
    while(true){
        
        // Get the index of the current instruction for the process
        index = stoi(instructions[curProcess][0]); 
        if(index >= instructions[curProcess].size())
            break;
        // Get the instruction at the current index
        s = instructions[curProcess][index];
        index++;

        // Use a stringstream to process the instruction string
        istringstream ss(s);
        string command;
        ss >> command;
        
        if(command == "send"){
            clock++;
            string receiver, msg;
            ss >> receiver >> msg;
            // Store the sent message and update the event order
            sentMessages.push_back({to_string(clock),curProcess,msg,receiver});
            order.insert(make_pair(clock, "sent "+curProcess+" "+msg+" "+receiver));
        }
        else if(command == "recv"){
            bool flag = false;
            string sender, msg;
            ss >> sender >> msg;
            if(sentMessages.empty()){
                // Handle deadlock scenario
                deadlock[curProcess] = "blocked";
                instructions[curProcess][0] = to_string(index - 1);
                instructions[curProcess][1] = to_string(clock);
                curProcess = sender;
                index = stoi(instructions[curProcess][0]);
                clock = stoi(instructions[curProcess][1]);
                continue;
            }
            for(int i=0;i<sentMessages.size();i++){
                if(sentMessages[i][1] == sender && sentMessages[i][2] == msg){
                    // Process received message and update the event order
                    deadlock[curProcess] = "free";
                    clock = max(stoi(sentMessages[i][0]), clock) + 1;
                    order.insert(make_pair(clock, "received "+curProcess+" "+msg+" "+sender));
                    flag = true;
                    sentMessages.erase(sentMessages.begin()+i);
                    break;
                }
            }
            if(!flag){
                // Handle deadlock scenario
                deadlock[curProcess] = "blocked";
                instructions[curProcess][0] = to_string(index-1);
                instructions[curProcess][1] = to_string(clock);

                curProcess = sender;
                index = stoi(instructions[curProcess][0]);
                clock = stoi(instructions[curProcess][1]);
                continue;
            }
        }
        else if(command == "print"){
            string msg;
            ss >> msg;
            clock++;
            // Update the event order for printing
            order.insert(make_pair(clock,"printed "+curProcess+" "+msg));
        }
        else if(command == "end"){
            clock = 0;
        }
        int blocked = 0;
        for(auto k:processes){
            if(deadlock[k] == "blocked")
                blocked++;
        }
        if(blocked == n)
        {
            lock = true;
            break;
        }
        if(deadlock[curProcess] != "blocked")
            instructions[curProcess][0] = to_string(index);
        
        int i;
        if(index >=  instructions[curProcess].size())
        {
            for(i=0;i < processes.size() - 1;i++){
                if(processes[i] == curProcess){
                    curProcess = processes[i+1];
                    int r = stoi(instructions[curProcess][0]);
                    if(r >= instructions[curProcess].size())
                        continue; 
                    else
                        break;
                }
            }
            if(i == processes.size() - 1){
                if(processes[i] == curProcess)
                    break;
            }
        }
    }
}

int main(){

    int clock = 0;
    string curProcess = "";
    string process;
    bool lock = false;
    int n; // Number of processes;

    multimap<int,string> order; // Total order of all events
    vector<vector<string>> sentMessages;// List of messages sent (in transit)
    map<string,vector<string>> instructions; // User input
    map<string,string> deadlock;
    
    cout << "Enter the number of processes: "; 
    cin >> n;
    getline(cin,s);
    vector<string> processes(n);
    int ind = 0;

    while(s != "exit"){
        getline(cin,s);
        if(s == "exit")
            break;
        istringstream ss(s);
        string command;
        ss >> command;
        if(command == "begin"){
            string temp,p;
            ss >> temp >> p;
            process = p;
            processes[ind] = p;
            ind++;
            if(curProcess == ""){
                curProcess = p;
            }
            instructions[process].push_back("2");
            instructions[process].push_back("0");
            continue;
        }
        instructions[process].push_back(s);
    }

    if(instructions.size() != n){
        cout << n << " processes are not there in the input. Please check your input !!" << endl;
        return 0;
    }
    
    lamport(order,sentMessages,instructions,deadlock,processes,clock,curProcess,lock);
    
    cout << "Before : \n";
    for(auto i : processes){
        cout << i << " -> " << deadlock[i] << endl;
    }

    if(lock)
        cout << "Since all the processes are waiting for the resources and no messages are in transit. Hence Deadlocked!!"<<endl;
    else{
        for(auto i : processes){
            while(deadlock[i] == "blocked"){
                int clk = stoi(instructions[i][1]);
                lamport(order,sentMessages,instructions,deadlock,processes,clk,i,lock);
            }
        }
    }
    for(auto i = order.begin(); i != order.end();i++){
        cout << i->second << " " << i->first << endl;
    }

    cout << "Messages in transit : \n";
    for(auto i =0;i<sentMessages.size();i++){
        for(auto j : sentMessages[i]){
            cout << j << " ";
        }
        cout << endl;
    }
}
