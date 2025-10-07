#ifndef UI_H
#define UI_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include "Tree.h"

class DrawPanel : public wxPanel {
private:
    std::map<wxString, Node*>* nodes;  
    Node* root;                        
    bool isDragging;                          
    wxPoint lastMousePos;                    
    int offsetX, offsetY;                    
    double scale;                             
    Node* person1;                     
    Node* person2;                      
    Node* lca;                         
    bool canMarry;                           

public:
    DrawPanel(wxWindow* parent, std::map<wxString, Node*>* n);

    void SetRoot(Node* r);      
    void ResetView();            
    void SetMarriageCheck(Node* p1, Node* p2, Node* lca_node, bool result);

    void OnMouseDown(wxMouseEvent& evt);    
    void OnMouseUp(wxMouseEvent& evt);       
    void OnMouseMove(wxMouseEvent& evt);       
    void OnMouseWheel(wxMouseEvent& evt);    
    void OnPaint(wxPaintEvent& evt);        

    void CalculatePositions();      // tinh vi tri cho cac nut
};

class FamilyTree : public wxFrame {
private:
    wxTextCtrl* inputText;   
    wxButton* drawBtn;         
    wxTextCtrl* person1Text;    
    wxTextCtrl* person2Text;    
    wxButton* checkBtn;         
    wxTextCtrl* resultText;     
    DrawPanel* drawPanel;       

    std::map<wxString, Node*> nodes; 
    Node* root;                     

public:
    FamilyTree(); 
    void OnDraw(wxCommandEvent& evt);            
    void OnResetView(wxCommandEvent& evt);    
    void OnCheckMarriage(wxCommandEvent& evt);  

    ~FamilyTree(); 

private:
    wxString LoadSampleDataFromFile();
};

#endif