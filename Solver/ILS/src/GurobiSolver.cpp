#include "GurobiSolver.h"

GurobiSolver::GurobiSolver(){};

Solution *GurobiSolver::createInitialSolution(Instance *instance)
{
    try
    {
        // Create an environment
        GRBEnv env = GRBEnv(true);
        env.set("LogFile", "mip.log");
        env.start();

        // Create an empty model
        GRBModel model = GRBModel(env);

        // Uncomment this to a more verbose execution
        // model.set(GRB_IntParam_LogToConsole, 0);

        // Definition of useful variables
        int numberOfBlocks = instance->blocks.size();
        int k = instance->numDistricts;
        int upperBound = instance->maxSizeDistricts;
        int lowerBound = instance->minSizeDistricts;

        vector<vector<GRBVar>> y = vector<vector<GRBVar>>(numberOfBlocks + k, vector<GRBVar>(numberOfBlocks + k));
        vector<vector<GRBVar>> f = vector<vector<GRBVar>>(numberOfBlocks + k, vector<GRBVar>(numberOfBlocks + k));

        // total flow entering some node i -> f(delta^-(i))
        vector<GRBLinExpr> inflow = vector<GRBLinExpr>(numberOfBlocks + k, 0.0);
        // total flow entering some node i -> f(delta^+(i))
        vector<GRBLinExpr> outflow = vector<GRBLinExpr>(numberOfBlocks + k, 0.0);

        // number of arcs coming into node i -> y(delta^-(i))
        vector<GRBLinExpr> arcsComing = vector<GRBLinExpr>(numberOfBlocks + k, 0.0);
        // number of arcs leaving node i -> y(delta^+(i))
        vector<GRBLinExpr> arcsLeaving = vector<GRBLinExpr>(numberOfBlocks + k, 0.0);

        // Create variables y_a and f_a (inflow and outflow are useful at constraints)
        for (Block currentBlock : instance->blocks)
        {
            for (int neighbor : currentBlock.adjacentBlocks)
            {
                y[currentBlock.id][neighbor] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y_" + to_string(currentBlock.id) + "_" + to_string(neighbor));
                f[currentBlock.id][neighbor] = model.addVar(0.0, upperBound, 0.0, GRB_INTEGER, "f_" + to_string(currentBlock.id) + "_" + to_string(neighbor));

                inflow[neighbor] += f[currentBlock.id][neighbor];
                outflow[currentBlock.id] += f[currentBlock.id][neighbor];

                arcsComing[neighbor] += y[currentBlock.id][neighbor];
                arcsLeaving[currentBlock.id] += y[currentBlock.id][neighbor];
            }

            for (int i = 0; i < k; i++)
            {
                y[i + numberOfBlocks][currentBlock.id] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y_" + to_string(i + numberOfBlocks) + "_" + to_string(currentBlock.id));
                f[i + numberOfBlocks][currentBlock.id] = model.addVar(0.0, upperBound, 0.0, GRB_INTEGER, "f_" + to_string(i + numberOfBlocks) + "_" + to_string(currentBlock.id));

                outflow[i + numberOfBlocks] += f[i + numberOfBlocks][currentBlock.id];
                inflow[currentBlock.id] += f[i + numberOfBlocks][currentBlock.id];

                arcsComing[currentBlock.id] += y[i + numberOfBlocks][currentBlock.id];
                arcsLeaving[i + numberOfBlocks] += y[i + numberOfBlocks][currentBlock.id];
            }
        }

        // Constraint f(\delta^+(i)) /leq f(\delta^+(i+1)) for i in [k-1]
        // Order the sources in order to avoid simetry
        for (int i = 0; i < k - 1; i++)
            model.addConstr(outflow[numberOfBlocks + i] <= outflow[numberOfBlocks + i + 1]);

        // Constraint f(\delta^-(v)) - f(\delta^+(v)) = 1, \forall v \in V
        // Each block must consume one unit -> inflow minus outflow is 1
        for (Block currentBlock : instance->blocks)
            model.addConstr(inflow[currentBlock.id] - outflow[currentBlock.id] == 1);

        // Constraint f_a \leq upperBound * y_a
        // Every flow is limited by upperbound and also it can only have some value if y_a is 1
        for (Block currentBlock : instance->blocks)
        {
            for (int neighbor : currentBlock.adjacentBlocks)
                model.addConstr(f[currentBlock.id][neighbor] <= upperBound * y[currentBlock.id][neighbor]);

            for (int i = 0; i < k; i++)
                model.addConstr(f[numberOfBlocks + i][currentBlock.id] <= upperBound * y[numberOfBlocks + i][currentBlock.id]);
        }

        // Constraint y(delta^+(s_i)) \leq 1, for every i \in [k]
        // Every source must have at most one outflow
        for (int i = 0; i < k; i++)
            model.addConstr(arcsLeaving[numberOfBlocks + i] <= 1);

        // Constraint y(delta^-(v)) \leq 1, for every v in V
        // every block must have at most one incoming node
        for (int block = 0; block < numberOfBlocks; block++)
            model.addConstr(arcsComing[block] <= 1);

        // New bound constraint f(delta^+(i)) \geq lowerBound for i in [k]
        // Every source must have outflow limited by bounds -> upper bound is respected always, evaluting lower bound is enought
        for (int i = 0; i < k; i++)
            model.addConstr(outflow[numberOfBlocks + i] >= lowerBound);

        GRBLinExpr obj = 0.0;

        model.setObjective(obj, GRB_MINIMIZE);
        model.optimize();

        // Create flow matrix in order to recreate original solutions
        vector<vector<int>> flows = vector<vector<int>>(numberOfBlocks + k, vector<int>(numberOfBlocks + k));

        int numvars = model.get(GRB_IntAttr_NumVars);
        GRBVar *fvars = model.getVars();

        for (int j = 0; j < numvars; j++)
        {
            GRBVar v = fvars[j];

            string nameOfVar = v.get(GRB_StringAttr_VarName);
            vector<string> splitedNameOfVar = utils::splitString(nameOfVar, '_');

            if (splitedNameOfVar[0] != "f")
                continue;

            int nodeI = stoi(splitedNameOfVar[1]);
            int nodeJ = stoi(splitedNameOfVar[2]);

            int flow = v.get(GRB_DoubleAttr_X);
            flows[nodeI][nodeJ] = flow;
        }

        return getSolutionFromFlow(flows, instance);
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
    return new Solution(instance);
}

Solution *GurobiSolver::getSolutionFromFlow(vector<vector<int>> flows, Instance *instance)
{
    Solution *solution = new Solution(instance);
    int numberOfDistricts = instance->numDistricts;
    int numberOfBlocks = instance->blocks.size();

    // Iterate over sources
    for (int i = 0; i < numberOfDistricts; i++)
    {
        int currentNode = i + numberOfBlocks;

        vector<int> neighbors = vector<int>();
        neighbors.push_back(currentNode);

        // while is has found some neighbor with flow
        while (!neighbors.empty())
        {
            currentNode = neighbors.back();
            neighbors.pop_back();

            // Look for the next neighbor with flow
            for (int neighbor = 0; neighbor < numberOfBlocks; neighbor++)
            {
                if (flows[currentNode][neighbor] > 0)
                {
                    // Add node into solution
                    solution->districts[i].insert(neighbor);
                    solution->blocksDistrict[neighbor] = i;
                    neighbors.push_back(neighbor);
                }
            }
        };
    }
    return solution;
}
