#include <iostream>

#include <vector>

#include <fstream>

#include <string>

#include <utility>

#include <stdexcept>

#include <set>

#include <map>


using namespace std;

vector < vector < string > > rewriteRules(string file_text) {

    vector < vector < string > > res;
    res.push_back({});
    string word;
    for (char c: file_text) {
        //cout << "Working with " << c << endl;
        if (c == '\n') {
            if (word != "") {
                res[res.size() - 1].push_back(word);
                res.push_back({});
            }
            word = "";
            //cout << "It's '\ n' \n";
        } else if (c == ' ') {
            if (word != "")
                res[res.size() - 1].push_back(word);
            word = "";
            //cout << "It's ' ' \n";
        } else if (c != '-'
            and c != '>') {
            word = word + c;
        }
    }

    res.pop_back();
    return res;
}

vector < vector < string > > First_k(vector < vector < string > > Rules, int k) {

    map < string, set < string >> First;

    for (const vector < string > & rule: Rules) {
        string nonTerminal = rule[0];
        string production = rule[1];
        if (isalpha(production[0]) && !isupper(production[0])) {
            First[nonTerminal].insert(production.substr(0, 1));
        }
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (const vector < string > & rule: Rules) {
            string nonTerminal = rule[0];
            string production = rule[1];

            for (size_t i = 0; i < production.length(); ++i) {
                string symbol = production.substr(i, 1);

                if (!isupper(symbol[0])) {
                    // If it's a terminal, add it to First[nonTerminal]
                    if (First[nonTerminal].insert(symbol).second) {
                        changed = true;
                    }
                    break;
                } else {
                    // If it's a non-terminal, add First[symbol] to First[nonTerminal]
                    for (const string & firstSymbol: First[symbol]) {
                        if (firstSymbol != "eps") {
                            if (First[nonTerminal].insert(firstSymbol).second) {
                                changed = true;
                            }
                        }
                    }
                    if (!First[symbol].count("eps")) {
                        break;
                    }
                }
            }
        }
    }

    vector < vector < string > > FirstSets;
    for (vector < string > rule: Rules) {
        string nonTerminal = rule[0];
        //FirstSets.push_back(First[nonTerminal]);
    }

    for (size_t i = 0; i < FirstSets.size(); ++i) {
        string nonTerminal = "Non-Terminal " + to_string(i);
        cout << "first(" << nonTerminal << ") = {";

        int symbolIndex = 0;
        for (const string & symbol: FirstSets[i]) {
            cout << symbol;
            if (symbolIndex < FirstSets[i].size() - 1) {
                cout << ", ";
            }
            symbolIndex++;
        }

        cout << "}" << endl;
    }

    return FirstSets;
}

bool isTerminal(string a) {
    return 'A' <= a.at(0) && a.at(0) <= 'Z' && a.size() == 1;
}

bool isIn(string a, vector < string > A) {
    for (int i = 0; i < A.size(); i++)
        if (a == A[i])
            return true;
    return false;
}

vector < string > addVector(vector < string > a, vector < string > b) {
    for (int i = 0; i < b.size(); i++)
        if (!isIn(b[i], a))
            a.push_back(b[i]);

    return a;
}

vector < string > getFirst(vector < vector < string > > first, string a) {
    vector < string > res;
    if (isTerminal(a)) {
        int i = 0;
        while (i < first.size()) {
            if (first[i][0] == a)
                break;
            i++;
        }
        for (int j = 1; j < first[i].size(); j++)
            res.push_back(first[i][j]);
    } else
        res.push_back(a);

    return res;
}

vector < string > concan(vector < string > a, vector < string > b) {
    vector < string > res;
    for (int i = 0; i < a.size(); i++) {
        if (a[i] != "eps")
            res.push_back(a[i]);
        else
            for (int j = 0; j < b.size(); j++)
                res.push_back(b[j]);
    }
    return res;
}

vector < vector < string > > nextStepFollow(vector < vector < string > > Rules, vector < vector < string > > first, vector < vector < string > > follow) {
    for (int i = 0; i < follow.size(); i++) {
        for (int j = 0; j < Rules.size(); j++)
            for (int k = 1; k < Rules[j].size(); k++) {
                if (Rules[j][k] == follow[i][0]) {
                    if (k < Rules[j].size() - 1) {
                        vector < string > temp = getFirst(first, Rules[j][k + 1]);
                        while (isIn("eps", temp)) {
                            i = 2;
                            if (k + i < Rules[j].size()) {
                                temp = concan(temp, getFirst(first, Rules[j][k + i]));
                                i++;
                            } else {
                                temp = concan(temp, getFirst(follow, Rules[j][0]));
                                break;
                            }
                        }
                        follow[i] = addVector(follow[i], temp);
                    } else {
                        follow[i] = addVector(follow[i], getFirst(follow, Rules[j][0]));
                    }
                }
            }
    }

    return follow;
}

vector < vector < string > > Follow_k1(vector < vector < string > > Rules, vector < vector < string > > first) {
    //function follow_k
    //visualisation
    // {A, ......}
    // {B, ......}
    vector < vector < string > > follow;
    for (int i = 0; i < first.size(); i++) {
        follow.push_back({
            first[i][0]
        });
        if (first[i][0] == Rules[0][0])
            follow[i].push_back("eps");
    }

    vector < vector < string > > check;
    while (check != follow) {
        check = follow;
        follow = nextStepFollow(Rules, first, follow);
    }

    return follow;
}

vector < vector < string > > LL_1(vector < vector < string > > first, vector < vector < string > > follow, vector < vector < string >> Rules) {

    vector < vector < string > > ll1;

    for (int i = 0; i < Rules.size(); i++) {
        vector < string > res = {
            "eps"
        };
        for (int j = 1; j < Rules[i].size(); j++) {
            res = concan(res, getFirst(first, Rules[i][j]));
        }
        res = concan(res, getFirst(follow, Rules[i][0]));

        for (int j = 0; j < res.size(); j++)
            ll1.push_back({
                Rules[i][0],
                res[j],
                to_string(i + 1)
            });
    }

    return ll1;
}

int main() {
    vector < vector < string > > first = {
        {
            "E",
            "id",
            "(",
            "eps"
        },
        {
            "D",
            "num",
            "(",
            "*",
            "!"
        },
        {
            "R",
            "num",
            "(",
            "*",
            "!"
        },
        {
            "V",
            "num",
            "eps"
        },
        {
            "L",
            "(",
            "eps"
        },
        {
            "Z",
            "eps"
        }
    };

    ifstream file("text.txt");

    if (!file.is_open()) {
        cout << "File can't be opened" << endl;
        return 0;
    }

    string file_text;
    for (string a; getline(file, a);) {
        file_text += a + "\n";
    }

    vector < vector < string > > Rules = rewriteRules(file_text);
    cout << "Rules: \n";
    for (int i = 0; i < Rules.size(); i++) {
        cout << Rules[i][0] << " -> ";
        for (int j = 1; j < Rules[i].size(); j++)
            cout << Rules[i][j] << ' ';
        cout << endl;
    }

    cout << endl;

    cout << "first: \n";
    for (int i = 0; i < first.size(); i++) {
        cout << first[i][0] << ": ";
        for (int j = 1; j < first[i].size(); j++)
            cout << first[i][j] << ' ';
        cout << endl;
    }

    cout << endl;

    vector < vector < string > > follow = Follow_k1(Rules, first);
    cout << "follow: \n";
    for (int i = 0; i < follow.size(); i++) {
        cout << follow[i][0] << ": ";
        for (int j = 1; j < follow[i].size(); j++)
            cout << follow[i][j] << ' ';
        cout << endl;
    }

    cout << endl;

    vector < vector < string > > ll_1 = LL_1(first, follow, Rules);
    cout << endl << "ll_1: \n";
    for (int i = 0; i < ll_1.size(); i++) {
        for (int j = 0; j < ll_1[i].size(); j++)
            cout << ll_1[i][j] << ' ';
        cout << endl;
    }

    return 0;
}
