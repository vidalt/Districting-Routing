#include "District.h"

void to_json(json& j, const District& d) 
{
	j = json{
	{"type", "District"},
	{"list-blocks", d.blocks},
	{"scenarios-cost", d.evaluationCosts},
	{"scenarios-nbCustomers", d.evaluationNbCustomers},
	{"average-cost", d.averageCost}
	};
}
