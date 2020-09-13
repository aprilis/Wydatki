#include "Manager.h"
#include "Util.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <cereal/archives/xml.hpp>

const string unassigned = "Inne";
const string configFile = "config.xml";
const string defaultFile = "Bilans.xml";

Manager::Manager() {
    auto configDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    auto configPath = QDir(configDir).filePath(QString::fromStdString(configFile));
    ifstream file(configPath.toStdString());
    if(file.good()) {
        cereal::XMLInputArchive archive(file);
        archive(*this);
    }
    if(path.empty()) {
        auto documentsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        path = QDir(documentsDir).filePath(QString::fromStdString(defaultFile)).toStdString();
    }
    file.close();
    load();
}

void Manager::saveConfig() {
    auto configDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    auto configPath = QDir(configDir).filePath(QString::fromStdString(configFile));
    try {
        ofstream file(configPath.toStdString());
        cereal::XMLOutputArchive archive(file);
        archive(*this);
    } catch(...) {
        cerr << "Unexpected error during saving config to " << configPath.toStdString() << endl;
    }
}

void Manager::load() {
    descriptions.clear();
    items.clear();
    categories.clear();
    totalExpenses = totalIncome = 0;

    ifstream file(path);
    vector<Item> loadedItems;
    if(file.good()) {
        try {
            cereal::XMLInputArchive archive(file);
            archive(loadedItems);
            for(auto it: loadedItems)
                addItem(it, true);
        } catch(...) {
            cerr << "Failed to read file " << path << endl;
        }
    }
}

void Manager::save() {
    ofstream file(path);
    if(file.good()) {
        try {
            cereal::XMLOutputArchive archive(file);
            archive(items);
        } catch(...) {
            cerr << "Failed to save file " << path << endl;
        }
    } else {
        cerr << "Failed to open file " << path << " for saving" << endl;
    }
}

void Manager::changeFile(string newPath) {
    path = newPath;
    saveConfig();
    load();
}

void Manager::saveAs(string newPath) {
    path = newPath;
    saveConfig();
    save();
}

void Manager::importPKO(string path)
{
    cerr << "loading from PKO xml file " << path << endl;
    try
    {
        ifstream file(path);
        if(file.bad()) throw string("wrong path");
        string str;
        getline(file, str, (char)EOF);
        unique_ptr<char[]> c_str(new char[str.size() + 1]);
        copy(str.begin(), str.end(), c_str.get());
        c_str[str.size()] = 0;
        xml_document<> doc;
        doc.parse<0>(c_str.get());
        auto node = doc.first_node()->first_node();
        while(node && string(node->name()) != "operations")
            node = node->next_sibling();
        if(!node) throw string("no node \'operations\'");
        for(auto nd = node->first_node(); nd; nd = nd->next_sibling())
            addItem(Item::fromPKO(nd));
    }
    catch(parse_error err)
    {
        cerr << err.what() << " in " << err.where<char>() << endl;
    }
    catch(string err)
    {
        cerr << "error! " << err << endl;
    }
    catch(...)
    {
        cerr << "other error during parsing file " << path << endl;
    }
    sortItems();
    save();
}

void Manager::importBS(string path)
{
    cerr << "loading from Bank Spółdzielczy csv file " << path << endl;
    try
    {
        ifstream file(path);
        if(file.bad()) throw string("wrong path");
        string str;
        while(!file.eof()) {
            getline(file, str);
            if(str.empty()) break;
            auto row = readCSVRow(str);
            if(row.size() != 19) throw string("wrong row length: ") + to_string(row.size());
            Item item;
            item.date = row[0];
            item.description = "Nadawca/odbiorca: " + row[9] + "\n" + row[13];
            item.description = QString::fromLocal8Bit(item.description.c_str())
                    .toStdString();
            replace(row[17].begin(), row[17].end(), ',', '.');
            item.value = stod(row[17]);
            if(row[2] == "W") item.value = -item.value;
            addItem(item);
        }
    }
    catch(string err)
    {
        cerr << "error! " << err << endl;
    }
    /*catch(...)
    {
        cerr << "other error during parsing file " << path << endl;
    }*/
    sortItems();
    save();
}

bool Manager::addItem(const Item &item, bool force) {
    if(descriptions.insert(item.description).second || force)
    {
        items.push_back(item);
        if(item.value < 0) totalExpenses -= item.value;
        else totalIncome += item.value;
        setItemCategory(items.size() - 1, item.category);
        return true;
    }
    else return false;
}

void Manager::setItemCategory(int nr, string str)
{
    Item &it = items[nr];
    auto old = it.assignedCategory;
    if(!old.empty())
    {
        categories[old].second--;
        if(it.value >= 0)
            categories[old].first.first -= it.value;
        else categories[old].first.second += it.value;
    }

    it.assignedCategory = it.category = str;
    if(it.assignedCategory.empty())
    {
        auto desc = QString::fromStdString(it.description);
        for(auto filter: filters)
            if(!filter.first.empty() && desc.contains(QString::fromStdString(filter.first), Qt::CaseInsensitive))
                it.assignedCategory = filter.second;
    }
    if(it.assignedCategory.empty())
        it.assignedCategory = unassigned;

    auto newCat = it.assignedCategory;
    categories[newCat].second++;
    if(it.value >= 0)
        categories[newCat].first.first += it.value;
    else categories[newCat].first.second -= it.value;

    if(categories[old].second == 0) categories.erase(old);
}

bool Manager::hasItemCategory(int nr)
{
    return items[nr].assignedCategory == unassigned;
}

void Manager::updateAll()
{
    for(int i = 0; i < items.size(); i++)
        setItemCategory(i, items[i].category);
}
