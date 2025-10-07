#ifndef TREE_H
#define TREE_H

#include <wx/wx.h>
#include <vector>
#include <set>
#include <map>

// Cau truc nut don gian cho cay gia dinh
struct Node {
    wxString name;           // Ten nguoi
    std::vector<Node*> children;  // Danh sach con cai
    Node* parent;      // Cha me
    int generation;          // The he (0 cho goc, 1 cho con cai goc, v.v.)
    int x, y;                // Vi tri de ve
    bool isSelected;         // Danh dau node duoc chon de kiem tra ket hon
    bool isOnPath;           // Danh dau node nam tren duong ket hon

    // Tao nut voi ten nguoi
    Node(const wxString& n) : name(n), parent(nullptr), generation(0), x(0), y(0), isSelected(false), isOnPath(false) {}
};

class Tree {
public:
    // Tim to tien chung gan nhat cua 2 nut
    static Node* findLCA(Node* u, Node* v);
    
    // Kiem tra xem hai nguoi co the ket hon hay khong
    static bool canMarry(Node* u, Node* v);
    
    // Tinh khoang cach the he giua cac nut
    static int getGenerationDistance(Node* ancestor, Node* p1, Node* p2);
    
    // Tra ve ten moi quan he dua tren khoang cach the he
    static wxString getRelationshipType(int genDiff);
    
    // Các hàm xử lý quan hệ từ UI chuyển sang
    static void processInput(std::map<wxString, Node*>& nodes, const wxString& input, Node*& root);
    static void addRelationship(std::map<wxString, Node*>& nodes, const wxString& parentName, const wxString& childName, Node*& root);
    static wxString loadSampleDataFromFile();
    static void calculatePositions(Node* root, std::map<wxString, Node*>& nodes);
};

#endif // TREE_H