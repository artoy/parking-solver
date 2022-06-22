#include "SAPPOROBDD/include/ZBDD.h"
#include "sbdd_helper/SBDD_helper.h"

// Delete commentout if necessary.
//#include "tdzdd/DdEval.hpp"
#include "tdzdd/DdSpec.hpp"
//#include "tdzdd/DdSpecOp.hpp"
#include "tdzdd/DdStructure.hpp"

// Comment out if your program does not need the following headers.
#include "tdzdd/spec/SizeConstraint.hpp"
#include "tdzdd/eval/ToZBDD.hpp"
#include "tdzdd/spec/SapporoZdd.hpp"

#include <vector>
#include <algorithm>

using namespace tdzdd;
using namespace sbddh;
using namespace std;

// Function to determine if a given state is in the solution space
bool is_feasible(vector<vector<int>> y_vectors) {
    
}

// The given initial state must be a feasible solution
int main() {
    int m, n, K;
    cin >> m >> n >> K;

    // Memory allocation
    BDD_Init(1024, 1024 * 1024 * 1024);

    // Allocate variables to be used
    for (int i = 0; i < m * n * K; i++) {
        BDD_NewVar();
    }




    ZBDD z1 = ZBDD(1); // representing {{}}
    z1 = z1.Change(1); // representing {{1}}

    // SAPPOROBDD helper functions
    ZBDD z2 = getSingleton(1); // representing {{1}}
    ZBDD z3 = getChild1(z2); // representing {{}}

    // tdzdd functions
    IntRange range(2, 2); // size just 2
    SizeConstraint sc(3, range); // (require including spec/SizeConstraint.hpp)
    DdStructure<2> dd1(sc); // representing {{1, 2}, {1, 3}, {2, 3}}

    // translate tdzdd to SAPPOROBDD (require including eval/ToZBDD.hpp)
    ZBDD z4 = dd1.evaluate(ToZBDD());

    // translate SAPPOROBDD to tdzdd (require including spec/SapporoZdd.hpp)
    DdStructure<2> dd2 = DdStructure<2>(SapporoZdd(z4));

    bool b = (z1 == z2 && isConstant(z3) && dd1.zddCardinality() == "3"
              && z4.Card() == 3 && dd2.zddCardinality() == "3");
    std::cout << "program " << (b ? "works" : "does not work")
              << " correctly" << std::endl;

    return 0;
}
