#ifndef MANAGER_H
#define MANAGER_H

#include <cereal/types/vector.hpp>
#include <cereal/types/utility.hpp>
#include "Item.h"
#include <unordered_set>
#include <map>
#include <algorithm>

class Manager
{
    string path;

    unordered_set<string> descriptions;

    vector<Item> items;

    vector<pair<string, string>> filters;

    map<string, pair<pair<double, double>, int> > categories;

    double totalExpenses = 0, totalIncome = 0;

    void saveConfig();

    void load();

public:
    template <class Archive> void serialize(Archive &ar) {
        ar(path, filters);
    }

    Manager();

    string getPath() const { return path; }
    void changeFile(string newPath);
    void saveAs(string newPath);

    void importPKO(string path);

    void importBS(string path);

    int getItemsCount() const { return items.size(); }

    bool addItem(const Item &item, bool force=false);

    Item& getItem(int nr) { return items[nr]; }

    void save();

    void sortItems() { sort(items.begin(), items.end(), [](Item a, Item b) { return a.date > b.date; }); }

    void setItemCategory(int nr, string str);

    bool hasItemCategory(int nr);

    void addFilter() { filters.emplace_back(); saveConfig(); }

    void removeFilter(int nr) { filters.erase(filters.begin() + nr); updateAll(); saveConfig(); }

    int getFiltersCount() const { return filters.size(); }

    pair<string, string> getFilter(int nr) const { return filters[nr]; }

    void setFilter(int nr, pair<string, string> filter) { filters[nr] = filter; saveConfig(); }

    const map<string, pair<pair<double, double>, int>>& getCategories() const { return categories; }

    void updateAll();

    pair<double, double> getTotal() const { return make_pair(totalExpenses, totalIncome); }
};

#endif // MANAGER_H
