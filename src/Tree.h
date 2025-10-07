#ifndef TREE_H
#define TREE_H

#include <wx/wx.h>
#include <vector>
#include <set>
#include <map>

// Cau truc nut don gian cho cay gia dinh
struct PersonNode {
    wxString name;           // Ten nguoi
    std::vector<PersonNode*> children;  // Danh sach con cai
    PersonNode* parent;      // Cha me
    int generation;          // The he (0 cho goc, 1 cho con cai goc, v.v.)
    int x, y;                // Vi tri de ve
    bool isSelected;         // Danh dau node duoc chon de kiem tra ket hon
    bool isOnPath;           // Danh dau node nam tren duong ket hon

    // Tao nut voi ten nguoi
    PersonNode(const wxString& n) : name(n), parent(nullptr), generation(0), x(0), y(0), isSelected(false), isOnPath(false) {}
};

class TreeAlgorithms {
public:
    // Tim to tien chung gan nhat cua 2 nut
    static PersonNode* findLCA(PersonNode* u, PersonNode* v);
    
    // Kiem tra xem hai nguoi co the ket hon hay khong
    static bool canMarry(PersonNode* u, PersonNode* v);
    
    // Tinh khoang cach the he giua cac nut
    static int getGenerationDistance(PersonNode* ancestor, PersonNode* p1, PersonNode* p2);
    
    // Tra ve ten moi quan he dua tren khoang cach the he
    static wxString getRelationshipType(int genDiff);
};

#endif // TREE_H