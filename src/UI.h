#ifndef UI_H
#define UI_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include "Tree.h"

// Panel de ve cay gia dinh voi kha nang keo tha
class DrawPanel : public wxPanel {
private:
    std::map<wxString, PersonNode*>* nodes;  // Danh sach tat ca cac nut
    PersonNode* root;                         // Nut goc cua cay
    bool isDragging;                          // Co dang keo khong
    wxPoint lastMousePos;                     // Vi tri chuot cuoi cung
    int offsetX, offsetY;                     // Vi tri dich chuyen
    double scale;                             // Ty le phong to/thu nho
    PersonNode* person1;                      // Nguoi thu nhat trong kiem tra ket hon
    PersonNode* person2;                      // Nguoi thu hai trong kiem tra ket hon
    PersonNode* lca;                          // To tien chung gan nhat
    bool canMarry;                            // Co the ket hon hay khong

public:
    DrawPanel(wxWindow* parent, std::map<wxString, PersonNode*>* n);

    void SetRoot(PersonNode* r);      // Dat nut goc de ve
    void ResetView();                 // Dat lai vi tri va ty le ve mac dinh
    void SetMarriageCheck(PersonNode* p1, PersonNode* p2, PersonNode* lca_node, bool result); // Dat thong tin kiem tra ket hon

    // Cac ham xu ly su kien chuot
    void OnMouseDown(wxMouseEvent& evt);     // Xu ly khi nhan chuot
    void OnMouseUp(wxMouseEvent& evt);       // Xu ly khi tha chuot
    void OnMouseMove(wxMouseEvent& evt);      // Xu ly khi di chuyen chuot
    void OnMouseWheel(wxMouseEvent& evt);    // Xu ly cuon chuot de phong to/thu nho
    void OnPaint(wxPaintEvent& evt);        // Ve cay len man hinh

    void CalculatePositions();      // Tinh toan vi tri cho cac nut
};

// Frame chinh cho ung dung cay gia dinh
class FamilyTreeFrame : public wxFrame {
private:
    wxTextCtrl* inputText;      // O nhap quan he cha-con
    wxButton* drawBtn;          // Nut ve cay
    wxTextCtrl* person1Text;    // O nhap nguoi 1 de kiem tra ket hon
    wxTextCtrl* person2Text;    // O nhap nguoi 2 de kiem tra ket hon
    wxButton* checkBtn;         // Nut kiem tra ket hon
    wxTextCtrl* resultText;     // O hien thi ket qua kiem tra ket hon
    DrawPanel* drawPanel;       // Panel de ve cay

    std::map<wxString, PersonNode*> nodes;  // Danh sach tat ca cac nguoi trong cay
    PersonNode* root;                       // Nguoi goc cua cay (nguoi lon tuoi nhat)

public:
    FamilyTreeFrame();  // Tao giao dien chinh

    // Cac ham xu ly su kien
    void OnDraw(wxCommandEvent& evt);             // Ve cay gia dinh
    void OnResetView(wxCommandEvent& evt);         // Dat lai che do xem
    void OnCheckMarriage(wxCommandEvent& evt);    // Kiem tra kha nang ket hon

    ~FamilyTreeFrame();  // Ham huy giai phong bo nho

private:
    // Helper methods for multi-line input processing
    void ProcessInput(const wxString& input);
    void AddRelationship(const wxString& parentName, const wxString& childName);
    wxString LoadSampleDataFromFile();
};

#endif // UI_H