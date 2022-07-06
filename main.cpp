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

// m, n, K denote the number of rows, columns, and cars, respectively
int m, n, K;

// Function to determine whether cars are overlapped
bool isOverlap(vector<int> placementOfCar) {

}

// Function to generate the solution space
ZBDD feasibleSolution(vector<vector<int>> carInfo) {
    vector<vector<int>> predPlacementsOfCars;

    for (int k = 0; k < K; k++) {
        // All placements of all cars
        vector<vector<int>> placementsOfCars;

        // Placement of a car
        vector<int> placementCar(carInfo.at(k).at(1));

        if (carInfo.at(k).at(0) == 0) {
            // If the car is horizontal
            for (int i = 0; i < m * n; i++) {
                for (int j = 0; j < placementCar.size(); j++) {
                    // The case that a car is split
                    if (j > 0 && i + j % n == 0) continue;
                    // The case that a car overflows the field
                    if (i + j >= m * n) continue;

                    placementCar.at(j) = (i + j + 1) + m * n * k;
                }
            }
        } else if (carInfo.at(k).at(0) == 1) {
            // If the car is vertical
        } else {
            // TODO: Output error
        }

        predPlacementsOfCars = placementsOfCars;
    }
}

int main() {
    cin >> m >> n >> K;

    // Hold the direction and length of cars
    // HACK: The method of specifying the size of a vector is (maybe) not good
    vector<vector<int>> carInfo(K, vector<int>(2));
    for (int k = 0; k < K; k++) {
        cin >> carInfo.at(k).at(0) >> carInfo.at(k).at(1);
    }

    // Memory allocation
    BDD_Init(1024, 1024 * 1024 * 1024);

    // Allocate variables to be used
    for (int i = 0; i < m * n * K; i++) {
        BDD_NewVar();
    }

    // Construct initial ZDD
    vector<int> zVec;
    // NOTE: Rows are denoted as i, columns as j, so j is on the outside
    // NOTE: The given initial state must be a feasible solution
    // TODO: Change to detect when input is not a feasible solution
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            string square;
            cin >> square;

            if (square == "-") {
                continue;
            } else {
                // Calculate the element number from the square and car number
                // NOTE: Node numbers begin with 1
                zVec.push_back((i + 1) + m * j + m * n * (stoi(square) - 1));
            }
        }
    }
    ZBDD z = getSingleSet(zVec);


    // FIXME: Delete below
    // // test
    // for (int v : zVec) {
    //     cout << v << endl;
    // }


    // ZBDD z1 = ZBDD(1); // representing {{}}
    // z1 = z1.Change(1); // representing {{1}}

    // // SAPPOROBDD helper functions
    // ZBDD z2 = getSingleton(1); // representing {{1}}
    // ZBDD z3 = getChild1(z2); // representing {{}}

    // // tdzdd functions
    // IntRange range(2, 2); // size just 2
    // SizeConstraint sc(3, range); // (require including spec/SizeConstraint.hpp)
    // DdStructure<2> dd1(sc); // representing {{1, 2}, {1, 3}, {2, 3}}

    // // translate tdzdd to SAPPOROBDD (require including eval/ToZBDD.hpp)
    // ZBDD z4 = dd1.evaluate(ToZBDD());

    // // translate SAPPOROBDD to tdzdd (require including spec/SapporoZdd.hpp)
    // DdStructure<2> dd2 = DdStructure<2>(SapporoZdd(z4));

    // bool b = (z1 == z2 && isConstant(z3) && dd1.zddCardinality() == "3"
    //           && z4.Card() == 3 && dd2.zddCardinality() == "3");
    // std::cout << "program " << (b ? "works" : "does not work")
    //           << " correctly" << std::endl;

    return 0;
}
