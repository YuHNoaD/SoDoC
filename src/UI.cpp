#include "UI.h"
#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <queue>
#include <sstream>

DrawPanel::DrawPanel(wxWindow* parent, std::map<wxString, Node*>* n)
    : wxPanel(parent, wxID_ANY), nodes(n), root(nullptr),
    isDragging(false), offsetX(0), offsetY(0), scale(1.0), person1(nullptr), person2(nullptr), lca(nullptr), canMarry(true)
{
    SetBackgroundColour(wxColour(245, 245, 250));
    Bind(wxEVT_PAINT, &DrawPanel::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &DrawPanel::OnMouseDown, this);
    Bind(wxEVT_LEFT_UP, &DrawPanel::OnMouseUp, this);
    Bind(wxEVT_MOTION, &DrawPanel::OnMouseMove, this);
    Bind(wxEVT_MOUSEWHEEL, &DrawPanel::OnMouseWheel, this);
}

void DrawPanel::SetRoot(Node* r)
{
    root = r;
    offsetX = 0;
    offsetY = 0;
    scale = 1.0;
    if (person1) person1->isSelected = false;
    if (person2) person2->isSelected = false;
    person1 = nullptr;
    person2 = nullptr;
    lca = nullptr;
    Refresh();
}

void DrawPanel::ResetView()
{
    offsetX = 0;
    offsetY = 0;
    scale = 1.0;
    Refresh();
}

void DrawPanel::OnMouseDown(wxMouseEvent& evt)
{
    isDragging = true;
    lastMousePos = evt.GetPosition();
    SetCursor(wxCursor(wxCURSOR_HAND));
}

void DrawPanel::OnMouseUp(wxMouseEvent& evt)
{
    isDragging = false;
    SetCursor(wxCursor(wxCURSOR_ARROW));
}

void DrawPanel::OnMouseMove(wxMouseEvent& evt)
{
    if (isDragging) {
        wxPoint currentPos = evt.GetPosition();
        offsetX += currentPos.x - lastMousePos.x;
        offsetY += currentPos.y - lastMousePos.y;
        lastMousePos = currentPos;
        Refresh();
    }
}

void DrawPanel::OnMouseWheel(wxMouseEvent& evt)
{
    double delta = evt.GetWheelRotation() > 0 ? 0.1 : -0.1;
    scale = std::max(0.3, std::min(2.0, scale + delta));
    Refresh();
}

void DrawPanel::OnPaint(wxPaintEvent& evt)
{
    wxPaintDC dc(this);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    if (!root || nodes->empty()) {
        dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.SetTextForeground(wxColour(150, 150, 150));
        dc.DrawText(wxT("Chua co du lieu. Nhap quan he vao o ben trai va nhan 'Ve cay'."), 20, 20);
        dc.DrawText(wxT("Sau do nhan 'Ve cay' de hien thi cay gia pha."), 20, 45);
        dc.DrawText(wxT("Co the keo chuot de di chuyen, lan chuot de zoom."), 20, 70);
        return;
    }
    dc.SetUserScale(scale, scale);
    CalculatePositions();
    
    std::set<std::pair<Node*, Node*>> highlightEdges;
    if (lca && person1 && person2) { 
        Node* current = person1;
        Node* prev = nullptr;
        while (current && current != lca) {
            prev = current;
            current = current->parent;
            if (current && prev) {
                highlightEdges.insert({current, prev});
                highlightEdges.insert({prev, current});
            }
        }
        
        current = person2;
        prev = nullptr;
        while (current && current != lca) {
            prev = current;
            current = current->parent;
            if (current && prev) {
                std::pair<Node*, Node*> edge1 = {current, prev};
                std::pair<Node*, Node*> edge2 = {prev, current};
                if (highlightEdges.count(edge1) == 0 && highlightEdges.count(edge2) == 0) {
                    highlightEdges.insert(edge1);
                    highlightEdges.insert(edge2);
                }
            }
        }
    }
    
    for (auto& pair : *nodes) {
        Node* node = pair.second;
        for (Node* child : node->children) {
            int x1 = node->x + offsetX / scale + 50;
            int y1 = node->y + offsetY / scale + 30;
            int x2 = child->x + offsetX / scale + 50;
            int y2 = child->y + offsetY / scale;
            bool isOnHighlightPath = highlightEdges.count({node, child}) > 0 || 
                                   highlightEdges.count({child, node}) > 0;
            
            if (isOnHighlightPath) {  // highlight path cua 2 nguoi
                if (canMarry) {
                    dc.SetPen(wxPen(wxColour(0, 150, 0), 2));
                }
                else {
                    dc.SetPen(wxPen(wxColour(200, 0, 0), 3));
                }
            }
            else {
                dc.SetPen(wxPen(*wxBLACK, 1)); 
            }
            dc.DrawLine(x1, y1, x2, y2);
        }
    }
    std::set<Node*> pathNodes;
    if (lca && person1 && person2) {
        Node* current = person1;
        while (current && current != lca) {
            pathNodes.insert(current);
            current = current->parent;
        }
        pathNodes.insert(lca); 
        current = person2;
        while (current && current != lca) {
            if (pathNodes.count(current) == 0) {
                pathNodes.insert(current);
            }
            current = current->parent;
        }
    }

    // tuongtu cai tren nma cho nut
    for (auto& pair : *nodes) {
        Node* node = pair.second;
        int x = node->x + offsetX / scale;
        int y = node->y + offsetY / scale;
        bool isOnPath = pathNodes.count(node) > 0;
        
        if (node == person1 || node == person2) {
            if (canMarry) {
                dc.SetBrush(wxBrush(wxColour(129, 199, 132)));
            }
            else {
                dc.SetBrush(wxBrush(wxColour(255, 180, 180)));
            }
        }
        else if (isOnPath) {
            dc.SetBrush(wxBrush(wxColour(220, 220, 220)));
        }
        else {
            dc.SetBrush(*wxWHITE_BRUSH);
        }
        dc.DrawRoundedRectangle(x, y, 100, 60, 8);


        wxSize textSize = dc.GetTextExtent(node->name);
        int textX = x + (100 - textSize.x) / 2;
        int textY = y + (60 - textSize.y) / 2;
        dc.DrawText(node->name, textX, textY);
    }
}

void DrawPanel::CalculatePositions()
{
    Tree::calculatePositions(root, *nodes);
}

void DrawPanel::SetMarriageCheck(Node* p1, Node* p2, Node* lca_node, bool result)
{
    for (auto& pair : *nodes) {
        pair.second->isSelected = false;
        pair.second->isOnPath = false;
    }
    person1 = p1;
    person2 = p2;
    lca = lca_node;
    canMarry = result;
    if (person1) person1->isSelected = true;
    if (person2) person2->isSelected = true;
    if (lca) {
        Node* current = person1;
        while (current && current != lca) {
            current->isOnPath = true;
            current = current->parent;
        }
        lca->isOnPath = true;
        current = person2;
        while (current && current != lca) {
            if (!current->isOnPath) {
                current->isOnPath = true;
            }
            current = current->parent;
        }
    }
    Refresh();
}

FamilyTree::FamilyTree() : wxFrame(nullptr, wxID_ANY, wxT("Quan Ly Cay Gia Pha"),
    wxDefaultPosition, wxSize(1200, 800)), root(nullptr)
{
    SetBackgroundColour(wxColour(250, 250, 252));
    wxPanel* mainPanel = new wxPanel(this);
    mainPanel->SetBackgroundColour(wxColour(250, 250, 252));
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxPanel* leftPanel = new wxPanel(mainPanel);
    leftPanel->SetBackgroundColour(*wxWHITE);
    leftPanel->SetMinSize(wxSize(300, -1));
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticText* titleText = new wxStaticText(leftPanel, wxID_ANY, wxT("CAY GIA PHA"));
    wxFont titleFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    titleText->SetFont(titleFont);
    titleText->SetForegroundColour(wxColour(63, 81, 181));
    leftSizer->Add(titleText, 0, wxALL | wxALIGN_CENTER, 15);
    leftSizer->AddSpacer(10);
    wxStaticText* inputLabel = new wxStaticText(leftPanel, wxID_ANY, wxT("NHAP QUAN HE CHA - CON"));
    wxFont sectionFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    inputLabel->SetFont(sectionFont);
    inputLabel->SetForegroundColour(wxColour(100, 100, 100));
    leftSizer->Add(inputLabel, 0, wxALL, 10);
    wxStaticText* instrText = new wxStaticText(leftPanel, wxID_ANY,
        wxT("Nhap quan he (moi quan he tren 1 dong):\nDinh dang: Cha Con"));
    instrText->SetForegroundColour(wxColour(120, 120, 120));
    leftSizer->Add(instrText, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    inputText = new wxTextCtrl(leftPanel, wxID_ANY, wxT(""),
        wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE);
    leftSizer->Add(inputText, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* fileSampleBtn = new wxButton(leftPanel, wxID_ANY, wxT("Mau"), wxDefaultPosition, wxSize(90, 35));
    fileSampleBtn->SetBackgroundColour(wxColour(76, 175, 80)); // mau xanh
    fileSampleBtn->SetForegroundColour(*wxWHITE);
    fileSampleBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) {
        wxString fileData = Tree::loadSampleDataFromFile();
        if (!fileData.IsEmpty()) {
            inputText->SetValue(fileData);
            wxCommandEvent drawEvt;
            OnDraw(drawEvt);
        }
        });
    drawBtn = new wxButton(leftPanel, wxID_ANY, wxT("Ve Cay"), wxDefaultPosition, wxSize(80, 35));
    drawBtn->SetBackgroundColour(wxColour(103, 58, 183));
    drawBtn->SetForegroundColour(*wxWHITE);
    drawBtn->Bind(wxEVT_BUTTON, &FamilyTree::OnDraw, this);
    wxButton* resetBtn = new wxButton(leftPanel, wxID_ANY, wxT("Dat Lai"), wxDefaultPosition, wxSize(80, 35));
    resetBtn->SetBackgroundColour(wxColour(96, 125, 139));
    resetBtn->SetForegroundColour(*wxWHITE);
    resetBtn->Bind(wxEVT_BUTTON, &FamilyTree::OnResetView, this);
    btnSizer->Add(fileSampleBtn, 0, wxRIGHT, 5);
    btnSizer->Add(drawBtn, 0, wxRIGHT, 5);
    btnSizer->Add(resetBtn, 0, wxLEFT, 5);
    leftSizer->Add(btnSizer, 0, wxALL | wxEXPAND, 10);
    leftSizer->AddSpacer(20);
    wxStaticText* checkLabel = new wxStaticText(leftPanel, wxID_ANY, wxT("KIEM TRA KET HON"));
    checkLabel->SetFont(sectionFont);
    checkLabel->SetForegroundColour(wxColour(100, 100, 100));
    leftSizer->Add(checkLabel, 0, wxALL, 10);
    wxStaticText* checkInstr = new wxStaticText(leftPanel, wxID_ANY,
        wxT("Luat: Cho phep ket hon neu ngoai 3 doi"));
    checkInstr->SetForegroundColour(wxColour(120, 120, 120));
    leftSizer->Add(checkInstr, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    wxStaticText* p1Label = new wxStaticText(leftPanel, wxID_ANY, wxT("Nguoi 1:"));
    leftSizer->Add(p1Label, 0, wxLEFT | wxRIGHT, 10);
    person1Text = new wxTextCtrl(leftPanel, wxID_ANY, wxT(""),
        wxDefaultPosition, wxSize(-1, 30));
    leftSizer->Add(person1Text, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    wxStaticText* p2Label = new wxStaticText(leftPanel, wxID_ANY, wxT("Nguoi 2:"));
    leftSizer->Add(p2Label, 0, wxLEFT | wxRIGHT, 10);
    person2Text = new wxTextCtrl(leftPanel, wxID_ANY, wxT(""),
        wxDefaultPosition, wxSize(-1, 30));
    leftSizer->Add(person2Text, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    checkBtn = new wxButton(leftPanel, wxID_ANY, wxT("Kiem tra"),
        wxDefaultPosition, wxSize(-1, 35));
    checkBtn->SetBackgroundColour(wxColour(33, 150, 243));
    checkBtn->SetForegroundColour(*wxWHITE);
    checkBtn->Bind(wxEVT_BUTTON, &FamilyTree::OnCheckMarriage, this);
    leftSizer->Add(checkBtn, 0, wxALL | wxEXPAND, 10);
    wxStaticText* resultLabel = new wxStaticText(leftPanel, wxID_ANY, wxT("Ket qua:"));
    resultLabel->SetFont(sectionFont);
    resultLabel->SetForegroundColour(wxColour(100, 100, 100));
    leftSizer->Add(resultLabel, 0, wxLEFT | wxRIGHT, 10);
    resultText = new wxTextCtrl(leftPanel, wxID_ANY, wxT("Chua kiem tra"),
        wxDefaultPosition, wxSize(-1, 50), wxTE_MULTILINE | wxTE_READONLY | wxTE_CENTRE);
    resultText->SetBackgroundColour(wxColour(240, 240, 240));
    resultText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    resultText->SetForegroundColour(wxColour(100, 100, 100));
    leftSizer->Add(resultText, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
    leftSizer->AddStretchSpacer();
    leftPanel->SetSizer(leftSizer);
    wxPanel* rightPanel = new wxPanel(mainPanel);
    rightPanel->SetBackgroundColour(wxColour(250, 250, 252));
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    drawPanel = new DrawPanel(rightPanel, &nodes);
    drawPanel->SetMinSize(wxSize(600, 400));
    rightSizer->Add(drawPanel, 1, wxALL | wxEXPAND, 10);
    rightPanel->SetSizer(rightSizer);
    mainSizer->Add(leftPanel, 0, wxEXPAND);
    mainSizer->Add(rightPanel, 1, wxEXPAND);
    mainPanel->SetSizer(mainSizer);
    Centre();
}

FamilyTree::~FamilyTree()
{
    for (auto& pair : nodes) {
        delete pair.second;
    }
}

void FamilyTree::OnDraw(wxCommandEvent& evt)
{
    wxString input = inputText->GetValue().Trim();
    if (input.IsEmpty()) {   // khongcogitronginputthixoadi
        for (auto& pair : nodes) {
            delete pair.second;
        }
        nodes.clear();
        root = nullptr;
        drawPanel->SetRoot(nullptr);
        return;
    }

    for (auto& pair : nodes) {
        delete pair.second;
    }
    nodes.clear();
    root = nullptr;
    Tree::processInput(nodes, input, root);
    for (auto& pair : nodes) {
        if (!pair.second->parent) {
            root = pair.second;
            break;
        }
    }
    if (!root && !nodes.empty()) {
        std::set<Node*> children_set;
        for (auto& pair : nodes) {
            for (Node* child : pair.second->children) {
                children_set.insert(child);
            }
        }
        for (auto& pair : nodes) {
            if (children_set.find(pair.second) == children_set.end()) {
                root = pair.second;
                break;
            }
        }
    }
    if (!root) {
        wxMessageBox(wxT("Khong tim thay nut goc! Vui long kiem tra du lieu nhap."),
            wxT("Loi"), wxICON_ERROR);
        return;
    }
    drawPanel->SetRoot(root);
}

void FamilyTree::OnResetView(wxCommandEvent& evt)
{
    drawPanel->ResetView();
}

wxString FamilyTree::LoadSampleDataFromFile()
{
    return Tree::loadSampleDataFromFile();
}

void FamilyTree::OnCheckMarriage(wxCommandEvent& evt)
{
    wxString name1 = person1Text->GetValue().Trim();
    wxString name2 = person2Text->GetValue().Trim();
    if (name1.IsEmpty() || name2.IsEmpty()) {
        resultText->SetValue(wxT("Vui long nhap day du ten hai nguoi!"));
        resultText->SetBackgroundColour(wxColour(255, 200, 200));
        resultText->SetForegroundColour(wxColour(180, 0, 0));
        resultText->Refresh();
        return;
    }
    if (name1 == name2) {
        resultText->SetValue(wxT("Khong the kiem tra cung mot nguoi!"));
        resultText->SetBackgroundColour(wxColour(255, 200, 200));
        resultText->SetForegroundColour(wxColour(180, 0, 0));
        resultText->Refresh();
        return;
    }
    if (nodes.find(name1) == nodes.end() || nodes.find(name2) == nodes.end()) {
        resultText->SetValue(wxT("Mot hoac ca hai nguoi khong ton tai trong cay!"));
        resultText->SetBackgroundColour(wxColour(255, 200, 200));
        resultText->SetForegroundColour(wxColour(180, 0, 0));
        resultText->Refresh();
        return;
    }
    Node* node1 = nodes[name1];
    Node* node2 = nodes[name2];
    bool canMarry = Tree::canMarry(node1, node2);
    Node* lca = Tree::findLCA(node1, node2);
    wxString resultMessage = wxString::Format(wxT("%s <-> %s:"), name1, name2);
    wxColour bgColour = wxColour(240, 240, 240);
    wxColour textColour = wxColour(0, 0, 0);
    if (canMarry) {
        resultMessage += wxT(" Co the ket hon");
        bgColour = wxColour(200, 255, 200);
        textColour = wxColour(0, 100, 0);
    }
    else {
        resultMessage += wxT(" Khong the ket hon");
        bgColour = wxColour(255, 200, 200);
        textColour = wxColour(180, 0, 0);
    }
    resultText->SetValue(resultMessage);
    resultText->SetBackgroundColour(bgColour);
    resultText->SetForegroundColour(textColour);
    resultText->Refresh();
    drawPanel->SetMarriageCheck(node1, node2, lca, canMarry);
}



