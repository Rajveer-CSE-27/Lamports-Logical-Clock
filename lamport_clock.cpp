#include<bits/stdc++.h>
using namespace std;

string s;

void lamport(multimap<int,string> &totalOrder, vector<vector<string>> &sentMessages, 
                map<string,vector<string>> &instructions, map<string,string> &deadlock,
                vector<string> &processes, int &clock, string curProcess, bool &lock){
    
    int n = processes.size();
    int index; // to switch between processes
    
    while(true){
        
        //index of instructions of process
        index = stoi(instructions[curProcess][0]); 
        if(index >= instructions[curProcess].size()){
            break;
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

        // instruction at 'index'
        s = instructions[curProcess][index];
        index++;

        //stringstream to process the strings
        istringstream ss(s);
        string command;
        ss >> command;
        
        if(command == "send"){
            clock++;
            string list, msg;
            ss >> list >> msg;
            string temp = list.substr(1);
            temp.pop_back();
            istringstream iss(temp);
            while (iss.good()) {
                string receiver;
                getline(iss, receiver, ',');
                sentMessages.push_back({to_string(clock),curProcess,msg,receiver});
            }
            totalOrder.insert(make_pair(clock, "sent "+curProcess+" "+msg+" "+list));
        }
        else if(command == "recv"){
            bool flag = false;
            string sender, msg;
            ss >> sender >> msg;
            if(sentMessages.empty()){
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
                    deadlock[curProcess] = "free";
                    clock = max(stoi(sentMessages[i][0]), clock) + 1;
                    totalOrder.insert(make_pair(clock, "received "+curProcess+" "+msg+" "+sender));
                    flag = true;
                    sentMessages.erase(sentMessages.begin()+i);
                    break;
                }
            }
            if(!flag){
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
            totalOrder.insert(make_pair(clock,"printed "+curProcess+" "+msg));
        }
        else if(command == "end"){
            clock = 0;
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
    int n; // no of processes;

    multimap<int,string> totalOrder; //total order of all events
    vector<vector<string>> sentMessages;// list of messages sent (transit)
    map<string,vector<string>> instructions; //user input
    map<string,string> deadlock;
    
    set<pair<string,string>> send;
    set<pair<string,string>> recv;

    cout << "Enter the number of processes: "; 
    cin >> n;
    getline(cin,s);
    vector<string> processes(n);
    int ind = 0;
    bool proceed = true;

    cout << " enter the operations: \n";
    while(s != "exit"){
        getline(cin,s);
        if(s == "exit")
            break;
        if(s == "")
            continue;
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
        else if(command == "send"){
            send.insert(make_pair(process,s));
        }
        else if(command == "recv"){
            recv.insert(make_pair(process,s));
        }
        else if(command == "print" || command == "end"){
            // do nothing
            if(command == "end"){
                ss >> command;
                if(command != "process")
                {
                    cout << "\nPlease check your input and try again. Follow the input format.\n";
                    return 0;
                }
            }
        }
        else{
            cout << "\nThere is no operation, named " << command << ". Please check your input and try again.\n";
            proceed = false;
            break;
        }
        instructions[process].push_back(s);
    }

    if(instructions.size() != n){
        cout << "Operations for "<< n << " processes were not given in the input. Please check yout input !!" << endl;
        return 0;
    }


    if(proceed){
    
        lamport(totalOrder,sentMessages,instructions,deadlock,processes,clock,curProcess,lock);

        if(lock)
            cout << "Since all the processes are waiting for the resources and no messages are in transit. Hence Deadlocked!!"<<endl;
        else{
            bool flag = false;
            for(auto i : processes){
                int cntr = 0;
                while(deadlock[i] == "blocked"){
                    int clk = stoi(instructions[i][1]);
                    lamport(totalOrder,sentMessages,instructions,deadlock,processes,clk,i,lock);
                    cntr++;
                    if(cntr >= n){
                        lock = true;
                        break;
                    }
                }
            }
            for(auto i : processes){
                int clk = stoi(instructions[i][1]);
                lamport(totalOrder,sentMessages,instructions,deadlock,processes,clk,i,lock);
            }
        }

        cout << "\n\nThis is the total order(so far): \n";
        if(totalOrder.empty())
            cout << "empty\n\n";
        else{
            for(auto i = totalOrder.begin(); i != totalOrder.end();i++){
                cout << i->second << " " << i->first << endl;
            }
        }
        if(lock){
            cout << "\n\nSystem Deadlocked\n\n" << endl;
        }
        else cout << "\n\nNo deadlock\n\n" << endl;
    }
}
