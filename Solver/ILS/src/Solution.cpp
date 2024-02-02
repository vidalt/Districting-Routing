#include "Solution.h"
#include "utils.h"
#include <bits/stdc++.h>
#include "../lib/nlohmann/json.hpp"
#include <fstream>
#include <iostream>
using json = nlohmann::json;

Solution::Solution(Solution *otherSolution)
{
    this->instance = otherSolution->instance;
    this->districts = vector<set<int>>(otherSolution->districts);
    this->blocksDistrict = vector<int>(otherSolution->blocksDistrict);
    this->costOfDistrict = vector<double>(otherSolution->costOfDistrict);
    this->cost = otherSolution->cost;
}

Solution::Solution(Instance *instance) : instance(instance)
{
    this->districts = vector<set<int>>(instance->numDistricts, set<int>());
    this->blocksDistrict = vector<int>(instance->blocks.size(), -1);
    this->costOfDistrict = vector<double>(instance->numDistricts, INFINITY);
}

bool Solution::isFeasible()
{
    int lowerBound = instance->minSizeDistricts;
    int upperBound = instance->maxSizeDistricts;

    for (set<int> district : this->districts)
    {
        if (district.size() < lowerBound || district.size() > upperBound || !instance->isDistrictContigous(&district))
            return false;
    }

    return true;
}

void Solution::exportToTxt(string outputName, int nbIterations, double executionTime)
{
    ofstream outputFile;
    string sufix = this->instance->predictorMethod;    
    string fileName = outputName + "." + sufix;

   outputFile.open(fileName);
   
    if (outputFile.is_open())
    {
        string feasibility = this->isFeasible() ? "feasible" : "infeasible";

        outputFile << instance->instanceName << endl;
        outputFile << feasibility << endl;
        
        outputFile << "NUMBER ITER " << nbIterations << endl;
        outputFile << "COST " << this->cost << endl;
        outputFile << "EXECUTION TIME " << executionTime << endl;

        for (set<int> district : this->districts)
        {
            for (int block : district)
            {
                outputFile << block << '\t';
            }
            outputFile << endl;
        }
    }
    outputFile.close();
}

void Solution::exportToGeoJson(string outputName)
{
    // Create artificial sorting in order to become easier to compare solutions
    vector<int> orderedDistricts = vector<int>(this->districts.size());
    iota(begin(orderedDistricts), end(orderedDistricts), 0);

    sort(orderedDistricts.begin(), orderedDistricts.end(),
         [&](int districtA, int districtB) -> bool
         {
             int minA = *min_element(this->districts[districtA].begin(), this->districts[districtA].end());
             int minB = *min_element(this->districts[districtB].begin(), this->districts[districtB].end());
             return minA < minB;
         });

    vector<string> colors{"black", "blue", "blueviolet", "brown",
                          "burlywood", "cadetblue", "chartreuse", "chocolate", "coral", "cornflowerblue",
                          "crimson", "darkblue", "darkcyan", "darkgoldenrod", "darkgray",
                          "darkgreen", "darkkhaki", "darkmagenta", "darkolivegreen", "darkorange",
                          "darkorchid", "darkred", "darksalmon", "darkseagreen", "darkslateblue", "darkslategray",
                          "darkslategrey", "darkturquoise", "darkviolet", "deeppink", "deepskyblue", "dimgray",
                          "dimgrey", "dodgerblue", "forestgreen", "fuchsia",
                          "gainsboro", "gold", "goldenrod", "gray", "green", "greenyellow",
                          "grey", "honeydew", "hotpink", "indianred", "indigo", "ivory", "khaki", "lavender",
                          "lavenderblush", "lawngreen", "lemonchiffon", "lightblue", "lightcoral", "lightcyan",
                          "lightgoldenrodyellow", "lightgray", "lightgreen", "lightgrey", "lightpink", "lightsalmon",
                          "lightseagreen", "lightskyblue", "lightslategray", "lightslategrey", "lightsteelblue",
                          "lightyellow", "lime", "limegreen", "linen", "magenta", "maroon", "mediumaquamarine",
                          "mediumblue", "mediumorchid", "mediumpurple", "mediumseagreen", "mediumslateblue",
                          "mediumspringgreen", "mediumturquoise", "mediumvioletred", "midnightblue", "mintcream",
                          "mistyrose", "moccasin", "navy", "oldlace", "olive", "olivedrab", "orange",
                          "orangered", "orchid", "palegoldenrod", "palegreen", "paleturquoise", "palevioletred",
                          "papayawhip", "peachpuff", "peru", "pink", "plum", "powderblue", "purple", "red",
                          "rosybrown", "royalblue", "saddlebrown", "salmon", "sandybrown", "seagreen",
                          "seashell", "sienna", "silver", "skyblue", "slateblue", "slategray", "slategrey",
                          "snow", "springgreen", "steelblue", "tan", "teal", "thistle", "tomato",
                          "turquoise", "violet", "wheat", "yellow", "yellowgreen"};

    ofstream outputFile;

    string sufix = this->instance->predictorMethod;
    outputFile.open(outputName + "." + sufix + ".geojson");

    ifstream instanceFile = utils::openCommonFile(("../../Data/Dataset/" + instance->cityName + ".geojson").c_str());

    if (!instanceFile.is_open()) 
    {
        cout << "Instance not able to open" << "../../Data/Dataset/" << instance->cityName << ".geojson" << endl;
    }

    if(!outputFile.is_open()){
        cout << "Not able to create output geojson " << (outputName + "." + sufix + ".geojson") << endl;
    }

    if (outputFile.is_open() && instanceFile.is_open())
    {
        json j;
        instanceFile >> j;
        instanceFile.close();

        json jsonMetadata = json{
            {"CITY", j.at("metadata").at("CITY")},
            {"NB_BLOCKS", instance->blocks.size()},
        };

        json features = j["features"];
        vector<json> jBlocks = vector<json>();

        for (auto &feature : features)
        {
            if (feature["properties"].contains("ID"))
            {
                feature["properties"].erase("LIST_ADJACENT");

                int blockId = feature["properties"]["ID"];

                if (blockId >= this->instance->blocks.size()) break;
                
                int districtOfBlock = find(orderedDistricts.begin(), orderedDistricts.end(), blocksDistrict[blockId]) - orderedDistricts.begin();

                feature["properties"]["DISTRICT_ID"] = districtOfBlock;

                string color = colors[districtOfBlock % colors.size()];

                feature["properties"]["stroke"] = color;
                feature["properties"]["stroke-width"] = 3;
                feature["properties"]["stroke-opacity"] = 1.25;
                feature["properties"]["fill"] = color;
                feature["properties"]["fill-opacity"] = "0.65";

                jBlocks.push_back(feature);
            }
        }


        string scenarioFileName = "../../Data/Dataset/scenarios/" + instance->instanceName + ".train.json";
        ifstream scenarioFile = utils::openCommonFile(scenarioFileName.c_str());

        if (scenarioFile.is_open())
        {
            json j;
            scenarioFile >> j;
            scenarioFile.close();
            array<double,2> depotPosition = j.at("metadata").at("DEPOT_LONGLAT");

            
            json depotGeometry = {{"coordinates",depotPosition}, {"type", "Point"}};

            json depotProp     = {{"NAME", "DEPOT"}};

            json jDepot =   json{{"type", "Feature"},
                                 {"properties", depotProp},
                                 {"geometry", depotGeometry}
                            };
                          
            jBlocks.push_back(jDepot);
        }
        else
        {
            cout << "Scenario file error " << scenarioFileName << endl;
        }

        outputFile << json{{"type", "FeatureCollection"},
                           {"metadata", jsonMetadata},
                           {"features", jBlocks}};


        outputFile.close();
    }
    else
    {
        string message = "Cannot open instance file: " + instance->instancePath;
        cout << message << endl;
        throw invalid_argument(message);
    }
}

vector<int> Solution::getDistrictsInBorder(int sourceDistrictId, int targetDistrictId)
{
    vector<int> blocksInBorder = vector<int>();
    for (int blockInSourceDistrict : this->districts[sourceDistrictId])
    {
        for (int neighbor : instance->blocks[blockInSourceDistrict].adjacentBlocks)
        {
            if (this->blocksDistrict[neighbor] == targetDistrictId)
            {
                blocksInBorder.push_back(blockInSourceDistrict);
                break;
            }
                
        }
    }
    return blocksInBorder;
}