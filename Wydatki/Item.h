#ifndef ITEM_H
#define ITEM_H

#include <cereal/types/string.hpp>
#include <cereal/cereal.hpp>
#include <rapidxml/rapidxml.hpp>
using namespace std;
using namespace rapidxml;

struct Item
{
    string date;

    string description;

    double value;

    string category;

    string assignedCategory;

    Item() = default;

    static Item fromPKO(xml_node<> *node)
    {
        Item item;
        for(auto nd = node->first_node(); nd; nd = nd->next_sibling())
        {
            string name = nd->name();
            if(name == "order-date") item.date = nd->value();
            else if(name == "description") item.description = nd->value();
            else if(name == "amount") item.value = strtod(nd->value(), NULL);
        }
        return item;
    }

    template<class Archive> void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(date), CEREAL_NVP(value), CEREAL_NVP(description), CEREAL_NVP(category));
    }
};

#endif // ITEM_H
