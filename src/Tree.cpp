#include "Tree.h"
#include <algorithm>
#include <set>
#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <queue>

Node* Tree::findLCA(Node* u, Node* v)
{
    if (!u || !v) return nullptr; 

    std::set<Node*> anc;  // tap chua cac node

    // di tu u den goc de luu node
    for (auto cur = u; cur; cur = cur->parent)
        anc.insert(cur);

    // lca to tien chung
    for (auto cur = v; cur; cur = cur->parent)
        if (anc.find(cur) != anc.end())
            return cur;
    return nullptr; // khong co to tien chung
}

// check kethon
bool Tree::canMarry(Node* u, Node* v)
{
    Node* lca = findLCA(u, v);
    if (!lca) return true;  // 

    int d1 = 0;
    for (auto cur = u; cur && cur != lca; cur = cur->parent)
        d1++;  

    int d2 = 0;
    for (auto cur = v; cur && cur != lca; cur = cur->parent)
        d2++;  

    // tong qua 3 thi return ve true
    return (d1 + d2 > 3);
}

int Tree::getGenerationDistance(Node* anc, Node* p1, Node* p2)
{
    if (!anc) return -1;  
    int d1 = 0;
    for (auto cur = p1; cur && cur != anc; cur = cur->parent)
        d1++; 

    int d2 = 0;
    for (auto cur = p2; cur && cur != anc; cur = cur->parent)
        d2++;
    return std::max(d1, d2);
}

// Xử lý input từ chuỗi
void Tree::processInput(std::map<wxString, Node*>& nodes, const wxString& input, Node*& root)
{
    wxStringTokenizer tokenizer(input, wxT("\n\r"), wxTOKEN_STRTOK);
    wxString token;
    int lineCount = 0;
    while (tokenizer.HasMoreTokens()) {
        token = tokenizer.GetNextToken().Trim();
        if (token.IsEmpty()) continue;
        lineCount++;
        wxStringTokenizer lineTokenizer(token, wxT(" \t"), wxTOKEN_STRTOK);
        wxString parentID, childID;
        if (lineTokenizer.HasMoreTokens()) {
            parentID = lineTokenizer.GetNextToken().Trim();
        }
        if (lineTokenizer.HasMoreTokens()) {
            childID = lineTokenizer.GetNextToken().Trim();
        }
        if (!parentID.IsEmpty() && !childID.IsEmpty()) {
            addRelationship(nodes, parentID, childID, root);
        }
        else {
            wxMessageBox(wxString::Format(wxT("Error: Invalid format on line %d"), lineCount), wxT("Loi"), wxICON_ERROR);
        }
    }
}

// add quan he
void Tree::addRelationship(std::map<wxString, Node*>& nodes, const wxString& parentName, const wxString& childName, Node*& root)
{
    if (parentName.IsEmpty() || childName.IsEmpty()) {
        wxMessageBox(wxT("Error: Ten khong duoc de trong"), wxT("Loi"), wxICON_ERROR);
        return;
    }
    Node* parent;
    if (nodes.find(parentName) == nodes.end()) {
        parent = new Node(parentName);
        nodes[parentName] = parent;
        if (!root) root = parent;
    }
    else {
        parent = nodes[parentName];
    }
    Node* child;
    if (nodes.find(childName) == nodes.end()) {
        child = new Node(childName);
        nodes[childName] = child;
    }
    else {
        child = nodes[childName];
        if (child->parent) {
            wxMessageBox(wxString::Format(wxT("Error: %s da co cha roi"), childName), wxT("Loi"), wxICON_ERROR);
            return;
        }
    }
    child->parent = parent;
    parent->children.push_back(child);
}

// load tu file txt
wxString Tree::loadSampleDataFromFile()
{
    wxString fileName = wxT("sample_input.txt");
    wxString fullPath = wxFileName::GetCwd() + wxT("/") + fileName;
    if (!wxFileExists(fullPath)) {
        wxString srcPath = wxT("../src/") + fileName;
        if (wxFileExists(srcPath)) {
            fullPath = srcPath;
        }
        else {
            if (!wxFileExists(fileName)) {
                wxMessageBox(wxT("Khong tim thay file sample_input.txt trong thu muc hien tai"), wxT("Loi"), wxICON_ERROR);
                return wxT("");
            }
            fullPath = fileName;
        }
    }
    wxTextFile textFile(fullPath);
    if (!textFile.Open()) {
        wxMessageBox(wxT("Khong the mo file sample_input.txt"), wxT("Loi"), wxICON_ERROR);
        return wxT("");
    }
    wxString content;
    wxString line;
    bool firstLine = true;
    for (line = textFile.GetFirstLine(); !textFile.Eof(); line = textFile.GetNextLine()) {
        line.Trim();
        if (!line.IsEmpty()) {
            if (!firstLine) {
                content += wxT("\n");
            }
            content += line;
            firstLine = false;
        }
    }
    textFile.Close();
    return content;
}

// postion cal
void Tree::calculatePositions(Node* root, std::map<wxString, Node*>& nodes)
{
    if (!root) return;
    std::map<int, int> genCounter;
    std::queue<Node*> q;
    q.push(root);
    root->generation = 0;
    while (!q.empty()) {
        Node* current = q.front();
        q.pop();
        int gen = current->generation;
        current->y = 80 + gen * 120;
        current->x = 150 + genCounter[gen] * 180;
        genCounter[gen]++;
        for (Node* child : current->children) {
            child->generation = gen + 1;
            q.push(child);
        }
    }
}
