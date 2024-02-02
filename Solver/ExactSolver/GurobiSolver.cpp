#include "GurobiSolver.h"
#include <string.h>
#include <algorithm>

GurobiSolver::GurobiSolver(){};

vector<string> splits(const string &s, const char &c)
{
    string buff{""};
    vector<string> v;

    for (auto n : s)
    {
        if (n != c)
            buff += n;
        else if (n == c && buff != "")
        {
            v.push_back(buff);
            buff = "";
        }
    }
    if (buff != "")
        v.push_back(buff);

    return v;
}

vector<int> getBlocksFromEncodedDistrict(string encodedDistrict)
{
    vector<int> blocks = vector<int>();

    for (string blockId : splits(encodedDistrict, ','))
        blocks.push_back(stoi(blockId));

    return blocks;
}

Solution* GurobiSolver::findBestSolution(Instance *instance, map<string, double> districtCosts)
{
    Solution* solution = new Solution(instance);

    try
    {

        // Create an environment
        GRBEnv env = new GRBEnv(true);
        env.set("LogFile", "mip.log");
        env.start();

        // Create an empty model
        GRBModel *model = new GRBModel(env);
        model->set(GRB_IntParam_LogToConsole, 1);
        model->set(GRB_IntParam_NumericFocus, 3);


        int numberOfBlocks = instance->blocks.size();
        int numberOfDistricts = districtCosts.size();

        vector<vector<int>> feasibleDistricts = vector<vector<int>>();
        vector<double> costs = vector<double>();

        for (pair<string, double> districtCost : districtCosts)
        {
            feasibleDistricts.push_back(getBlocksFromEncodedDistrict(districtCost.first));
            costs.push_back(districtCost.second);
        }

        // y of j means that the j-district set is on solution
        vector<GRBVar> y = vector<GRBVar>(numberOfDistricts);

        // Create yj variables
        for (int j = 0; j < numberOfDistricts; j++)
        {
            y[j] = model->addVar(0, 1, 0, GRB_INTEGER, "y_" + to_string(j));
        }

        // Create Constraint (1) - every block must be assigned to exactly one district
        for (int i = 0; i < numberOfBlocks; i++)
        {
            GRBLinExpr constraint = 0;

            int j = -1;

            for (vector<int> feasibleDistrict : feasibleDistricts)
            {
                j++;

                bool isBlockInDistrict = find(feasibleDistrict.begin(), feasibleDistrict.end(), i) != feasibleDistrict.end();
                if (!isBlockInDistrict)
                    continue;

                constraint += y[j];
            }

            model->addConstr(constraint == 1);
        }

        // It should be numDistrict districts
        GRBLinExpr numberDistrictsUsed = 0.;
        for (GRBVar isDistrictUsed : y)
            numberDistrictsUsed += isDistrictUsed;

        model->addConstr(numberDistrictsUsed == instance->numDistricts);


        GRBLinExpr obj = 0.0;

        for(int j = 0; j <costs.size(); j++ )
        {
            obj += costs[j] * y[j];
        }

        model->setObjective(obj, GRB_MINIMIZE);
        model->optimize();

        int numvars = model->get(GRB_IntAttr_NumVars);
        GRBVar *fvars = model->getVars();

        vector<int> positionOfDistricts = vector<int>();

        for (int numVar = 0; numVar < numvars; numVar++)
        {
            GRBVar v = fvars[numVar];
            string nameOfVar = v.get(GRB_StringAttr_VarName);

            int value = round(v.get(GRB_DoubleAttr_X));

            if (nameOfVar.find("y") != std::string::npos && value == 1)
                positionOfDistricts.push_back(stoi(splits(nameOfVar, '_')[1]));
        }

        int counter = -1;
        int districtId = 0;
        double cost = 0.;

        for (int counter = 0; counter < feasibleDistricts.size(); counter++)
        {
            bool isDistrictInSolution = find(positionOfDistricts.begin(), positionOfDistricts.end(), counter) != positionOfDistricts.end();
            if (!isDistrictInSolution)
                continue;

            for (int blockId : feasibleDistricts[counter])
            {
                solution->districts[districtId].insert(blockId);
            }

            districtId++;
            cost += costs[counter];
        }

        solution->cost = cost;

        // solution->exportToGeoJson(instance->instanceName);
        // solution->exportToTxt(instance->instanceName, 0);

        // ofstream outputFile;
        // string sufix = instance->predictorMethod;    
        // string fileName = instance->instanceName + "." + sufix;

        // outputFile.open(fileName);
    
        // if (outputFile.is_open())
        // {
        //     string feasibility = solution->isFeasible() ? "feasible" : "infeasible";

        //     outputFile << instance->instanceName << endl;
        //     outputFile << feasibility << endl;
            
        //     outputFile << "NUMBER ITER " << 0 << endl;
        //     outputFile << "COST " << cost << endl;
        //     outputFile << "EXECUTION TIME " << 0 << endl;

        //     for (vector<int> district : solution->districts)
        //     {
        //         for (int block : district)
        //         {
        //             outputFile << block << '\t';
        //         }
        //         outputFile << endl;
        //     }
        // }
        // outputFile.close();

        return solution;
    }
    catch (GRBException e)
    {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    }
    catch (...)
    {
        cout << "Exception during optimization" << endl;
    }
    return solution;
}