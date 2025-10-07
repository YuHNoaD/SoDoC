#include "UI.h"
#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <queue>
#include <sstream>

// Constructor cho DrawPanel
DrawPanel::DrawPanel(wxWindow* parent, std::map<wxString, PersonNode*>* n)
    : wxPanel(parent, wxID_ANY), nodes(n), root(nullptr),
    isDragging(false), offsetX(0), offsetY(0), scale(1.0), person1(nullptr), person2(nullptr), lca(nullptr), canMarry(true) {

    SetBackgroundColour(wxColour(245, 245, 250));

    Bind(wxEVT_PAINT, &DrawPanel::OnPaint, this);      // Ve khi can
    Bind(wxEVT_LEFT_DOWN, &DrawPanel::OnMouseDown, this);  // Xu ly nhan chuot
    Bind(wxEVT_LEFT_UP, &DrawPanel::OnMouseUp, this);      // Xu ly tha chuot
    Bind(wxEVT_MOTION, &DrawPanel::OnMouseMove, this);     // Xu ly di chuyen chuot
    Bind(wxEVT_MOUSEWHEEL, &DrawPanel::OnMouseWheel, this); // Xu ly cuon chuot
}

void DrawPanel::SetRoot(PersonNode* r) {
    root = r;
    offsetX = 0;
    offsetY = 0;
    scale = 1.0;
    // Reset highlight state when setting new root
    if (person1) person1->isSelected = false;
    if (person2) person2->isSelected = false;
    person1 = nullptr;
    person2 = nullptr;
    lca = nullptr;
    Refresh();
}

void DrawPanel::ResetView() {
    offsetX = 0;
    offsetY = 0;
    scale = 1.0;
    Refresh();
}

void DrawPanel::OnMouseDown(wxMouseEvent& evt) {
    isDragging = true;
    lastMousePos = evt.GetPosition();
    SetCursor(wxCursor(wxCURSOR_HAND));
}

void DrawPanel::OnMouseUp(wxMouseEvent& evt) {
    isDragging = false;
    SetCursor(wxCursor(wxCURSOR_ARROW));
}

void DrawPanel::OnMouseMove(wxMouseEvent& evt) {
    if (isDragging) {
        wxPoint currentPos = evt.GetPosition();
        offsetX += currentPos.x - lastMousePos.x;
        offsetY += currentPos.y - lastMousePos.y;
        lastMousePos = currentPos;
        Refresh();
    }
}

void DrawPanel::OnMouseWheel(wxMouseEvent& evt) {
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

    // VẼ TẤT CẢ CẠNH
    for (auto& pair : *nodes)
    {
        PersonNode* node = pair.second;
        for (PersonNode* child : node->children)
        {
            int x1 = node->x + offsetX / scale + 50;
            int y1 = node->y + offsetY / scale + 30;
            int x2 = child->x + offsetX / scale + 50;
            int y2 = child->y + offsetY / scale;
            
            // Kiểm tra xem cạnh này có nằm trên đường từ person1 đến person2 không
            bool isOnHighlightPath = false;
            if (lca) {
                // Kiểm tra từ person1 đến LCA
                PersonNode* current = person1;
                while (current && current != lca && current->parent) {
                    if ((current == node && current->parent == child) || 
                        (current->parent == node && current == child)) {
                        isOnHighlightPath = true;
                        break;
                    }
                    current = current->parent;
                }
                
                // Nếu chưa tìm thấy, kiểm tra từ person2 đến LCA
                if (!isOnHighlightPath) {
                    current = person2;
                    while (current && current != lca && current->parent) {
                        if ((current == node && current->parent == child) || 
                            (current->parent == node && current == child)) {
                            isOnHighlightPath = true;
                            break;
                        }
                        current = current->parent;
                    }
                }
            }
            
            // Vẽ cạnh với màu sắc phù hợp
            if (isOnHighlightPath) {
                // Xác định màu sắc dựa trên kết quả kiểm tra kết hôn
                if (canMarry) {
                    dc.SetPen(wxPen(wxColour(0, 150, 0), 3)); // Green if can marry
                } else {
                    dc.SetPen(wxPen(wxColour(200, 0, 0), 3)); // Red if can't marry
                }
            } else {
                dc.SetPen(wxPen(*wxBLACK, 1)); // Cạnh thường
            }
            dc.DrawLine(x1, y1, x2, y2);
        }
    }

    // VẼ TẤT CẢ CÁC NÚT
    for (auto& pair : *nodes)
    {
        PersonNode* node = pair.second;
        int x = node->x + offsetX / scale;
        int y = node->y + offsetY / scale;

        // Kiểm tra xem nút có nằm trên đường từ person1 đến person2 không
        bool isOnPath = false;
        if (lca) {
            // Kiểm tra từ person1 đến LCA
            PersonNode* current = person1;
            while (current && current != lca) {
                if (current == node) {
                    isOnPath = true;
                    break;
                }
                current = current->parent;
            }
            
            // Nếu chưa tìm thấy, kiểm tra LCA
            if (!isOnPath && node == lca) {
                isOnPath = true;
            }
            
            // Nếu chưa tìm thấy, kiểm tra từ person2 đến LCA
            if (!isOnPath) {
                current = person2;
                while (current && current != lca) {
                    if (current == node) {
                        isOnPath = true;
                        break;
                    }
                    current = current->parent;
                }
            }
        }

        // Xác định màu sắc cho nút
        if (node == person1 || node == person2) {
            // Highlight 2 người được chọn
            if (canMarry) {
                dc.SetBrush(wxBrush(wxColour(129, 199, 132))); // Light green if can marry
            } else {
                dc.SetBrush(wxBrush(wxColour(255, 180, 180))); // Light red if can't marry
            }
        }
        else if (isOnPath) {
            // Tô màu tất cả các nút còn lại trên path
            if (canMarry) {
                dc.SetBrush(wxBrush(wxColour(200, 255, 200))); // Light green path if can marry
            } else {
                dc.SetBrush(wxBrush(wxColour(255, 220, 220))); // Light red path if can't marry
            }
        }
        else {
            dc.SetBrush(*wxWHITE_BRUSH);    // Nút bình thường màu trắng
        }

        // Xác định độ đậm viền
        if (node == person1 || node == person2) {
            dc.SetPen(wxPen(*wxBLACK, 3));  // Viền đậm cho 2 người được chọn
        } else {
            dc.SetPen(*wxBLACK_PEN);        // Viền bình thường
        }
        
        dc.DrawRoundedRectangle(x, y, 100, 60, 8);

        // VẼ NHÃN TRÊN NÚT
        wxSize textSize = dc.GetTextExtent(node->name);
        int textX = x + (100 - textSize.x) / 2;
        int textY = y + (60 - textSize.y) / 2;
        dc.DrawText(node->name, textX, textY);
    }
}

void DrawPanel::CalculatePositions() {
    if (!root) return;

    std::map<int, int> genCounter;
    std::queue<PersonNode*> q;
    q.push(root);
    root->generation = 0;

    while (!q.empty()) {
        PersonNode* current = q.front();
        q.pop();

        int gen = current->generation;
        current->y = 80 + gen * 120;
        current->x = 150 + genCounter[gen] * 180;
        genCounter[gen]++;

        for (PersonNode* child : current->children) {
            child->generation = gen + 1;
            q.push(child);
        }
    }
}



void DrawPanel::SetMarriageCheck(PersonNode* p1, PersonNode* p2, PersonNode* lca_node, bool result) {
    // Reset previous selections and path markings
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
    
    // Mark the path from person1 to LCA, and from LCA to person2
    if (lca) {
        // Mark path from person1 to LCA
        PersonNode* current = person1;
        while (current && current != lca) {
            current->isOnPath = true;
            current = current->parent;
        }
        if (current) current->isOnPath = true; // Mark LCA
        
        // Mark path from person2 to LCA
        current = person2;
        while (current && current != lca) {
            current->isOnPath = true;
            current = current->parent;
        }
    }
    
    Refresh();
}



// Constructor cho FamilyTreeFrame
FamilyTreeFrame::FamilyTreeFrame() : wxFrame(nullptr, wxID_ANY, wxT("Quan Ly Cay Gia Pha"),
    wxDefaultPosition, wxSize(1200, 800)), root(nullptr) {

    SetBackgroundColour(wxColour(250, 250, 252));

    wxPanel* mainPanel = new wxPanel(this);
    mainPanel->SetBackgroundColour(wxColour(250, 250, 252));
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // LEFT PANEL
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
        wxT("Nhap quan he (moi quan he tren 1 dong):\nDinh dang: ParentID ChildID"));
    instrText->SetForegroundColour(wxColour(120, 120, 120));
    leftSizer->Add(instrText, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    inputText = new wxTextCtrl(leftPanel, wxID_ANY, wxT(""),
        wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE);
    leftSizer->Add(inputText, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);

    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* fileSampleBtn = new wxButton(leftPanel, wxID_ANY, wxT("Mau"), wxDefaultPosition, wxSize(90, 35));
    fileSampleBtn->SetBackgroundColour(wxColour(76, 175, 80)); // Green color
    fileSampleBtn->SetForegroundColour(*wxWHITE);
    fileSampleBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) {
        wxString fileData = LoadSampleDataFromFile();
        if (!fileData.IsEmpty()) {
            inputText->SetValue(fileData);
            // Automatically trigger the draw function
            wxCommandEvent drawEvt;
            OnDraw(drawEvt);
        }
    });
    
    drawBtn = new wxButton(leftPanel, wxID_ANY, wxT("Ve Cay"), wxDefaultPosition, wxSize(80, 35));
    drawBtn->SetBackgroundColour(wxColour(103, 58, 183));
    drawBtn->SetForegroundColour(*wxWHITE);
    drawBtn->Bind(wxEVT_BUTTON, &FamilyTreeFrame::OnDraw, this);
    
    wxButton* resetBtn = new wxButton(leftPanel, wxID_ANY, wxT("Dat Lai"), wxDefaultPosition, wxSize(80, 35));
    resetBtn->SetBackgroundColour(wxColour(96, 125, 139));
    resetBtn->SetForegroundColour(*wxWHITE);
    resetBtn->Bind(wxEVT_BUTTON, &FamilyTreeFrame::OnResetView, this);

    btnSizer->Add(fileSampleBtn, 0, wxRIGHT, 5); // Add file sample button
    btnSizer->Add(drawBtn, 0, wxRIGHT, 5); // Add draw button
    btnSizer->Add(resetBtn, 0, wxLEFT, 5); // Add reset button instead of clear button
    leftSizer->Add(btnSizer, 0, wxALL | wxEXPAND, 10);

    leftSizer->AddSpacer(20);

    wxStaticText* checkLabel = new wxStaticText(leftPanel, wxID_ANY, wxT("KIEM TRA KET HON"));
    checkLabel->SetFont(sectionFont);
    checkLabel->SetForegroundColour(wxColour(100, 100, 100));
    leftSizer->Add(checkLabel, 0, wxALL, 10);

    wxStaticText* checkInstr = new wxStaticText(leftPanel, wxID_ANY,
        wxT("Luat: Cam ket hon neu <= 3 doi"));
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
    checkBtn->Bind(wxEVT_BUTTON, &FamilyTreeFrame::OnCheckMarriage, this);
    leftSizer->Add(checkBtn, 0, wxALL | wxEXPAND, 10);
    
    // Add a text control for displaying marriage check results
    wxStaticText* resultLabel = new wxStaticText(leftPanel, wxID_ANY, wxT("Ket qua:"));
    resultLabel->SetFont(sectionFont);
    resultLabel->SetForegroundColour(wxColour(100, 100, 100));
    leftSizer->Add(resultLabel, 0, wxLEFT | wxRIGHT, 10);
    
    resultText = new wxTextCtrl(leftPanel, wxID_ANY, wxT("Chua kiem tra"),
        wxDefaultPosition, wxSize(-1, 50), wxTE_MULTILINE | wxTE_READONLY | wxTE_CENTRE);
    resultText->SetBackgroundColour(wxColour(240, 240, 240)); // Default gray background
    resultText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    resultText->SetForegroundColour(wxColour(100, 100, 100));
    leftSizer->Add(resultText, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);

    leftSizer->AddStretchSpacer();
    leftPanel->SetSizer(leftSizer);

    // RIGHT PANEL
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

FamilyTreeFrame::~FamilyTreeFrame() {
    for (auto& pair : nodes) {
        delete pair.second;
    }
}

void FamilyTreeFrame::OnDraw(wxCommandEvent& evt) {
    // Process input from textbox
    wxString input = inputText->GetValue().Trim();
    
    if (input.IsEmpty()) {
        // If no input, clear all data
        for (auto& pair : nodes) {
            delete pair.second;
        }
        nodes.clear();
        root = nullptr;
        drawPanel->SetRoot(nullptr);
        return;
    }
    
    // Clear existing tree data (when there is input)
    for (auto& pair : nodes) {
        delete pair.second;
    }
    nodes.clear();
    root = nullptr;
    
    // Process input with multi-line format support
    ProcessInput(input);
    
    // Find root node (the one without parent)
    // Look for a node that has no parent and is not someone else's child
    for (auto& pair : nodes) {
        if (!pair.second->parent) {
            root = pair.second;
            break;
        }
    }
    
    // If we still don't have a root, try to find a potential root by checking if any node is not a child of another
    if (!root && !nodes.empty()) {
        // Find a node that is not a child of any other node
        std::set<PersonNode*> children_set;
        for (auto& pair : nodes) {
            for (PersonNode* child : pair.second->children) {
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

void FamilyTreeFrame::OnResetView(wxCommandEvent& evt) {
    drawPanel->ResetView();
}

wxString FamilyTreeFrame::LoadSampleDataFromFile() {
    wxString fileName = wxT("sample_input.txt");
    wxString fullPath = wxFileName::GetCwd() + wxT("/") + fileName;
    
    if (!wxFileExists(fullPath)) {
        // Try to find the file in the project src directory
        wxString srcPath = wxT("../src/") + fileName;
        if (wxFileExists(srcPath)) {
            fullPath = srcPath;
        } else {
            // Try current directory without path
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

void FamilyTreeFrame::OnCheckMarriage(wxCommandEvent& evt) {
    wxString name1 = person1Text->GetValue().Trim();
    wxString name2 = person2Text->GetValue().Trim();

    if (name1.IsEmpty() || name2.IsEmpty()) {
        resultText->SetValue(wxT("Vui long nhap day du ten hai nguoi!"));
        resultText->SetBackgroundColour(wxColour(255, 200, 200)); // Light red
        resultText->SetForegroundColour(wxColour(180, 0, 0)); // Dark red text
        resultText->Refresh();
        return;
    }

    if (name1 == name2) {
        resultText->SetValue(wxT("Khong the kiem tra cung mot nguoi!"));
        resultText->SetBackgroundColour(wxColour(255, 200, 200)); // Light red
        resultText->SetForegroundColour(wxColour(180, 0, 0)); // Dark red text
        resultText->Refresh();
        return;
    }

    if (nodes.find(name1) == nodes.end() || nodes.find(name2) == nodes.end()) {
        resultText->SetValue(wxT("Mot hoac ca hai nguoi khong ton tai trong cay!"));
        resultText->SetBackgroundColour(wxColour(255, 200, 200)); // Light red
        resultText->SetForegroundColour(wxColour(180, 0, 0)); // Dark red text
        resultText->Refresh();
        return;
    }

    PersonNode* node1 = nodes[name1];
    PersonNode* node2 = nodes[name2];
    
    // Check marriage eligibility using TreeAlgorithms
    bool canMarry = TreeAlgorithms::canMarry(node1, node2);
    PersonNode* lca = TreeAlgorithms::findLCA(node1, node2);

    wxString resultMessage = wxString::Format(wxT("%s <-> %s:"), name1, name2);
    wxColour bgColour = wxColour(240, 240, 240); // Default gray
    wxColour textColour = wxColour(0, 0, 0); // Default black

    if (canMarry) {
        resultMessage += wxT(" Co the ket hon");
        bgColour = wxColour(200, 255, 200); // Light green
        textColour = wxColour(0, 100, 0); // Dark green text
    }
    else {
        resultMessage += wxT(" Khong the ket hon");
        bgColour = wxColour(255, 200, 200); // Light red
        textColour = wxColour(180, 0, 0); // Dark red text
    }

    resultText->SetValue(resultMessage);
    resultText->SetBackgroundColour(bgColour);
    resultText->SetForegroundColour(textColour);
    resultText->Refresh();
    
    // Highlight the two people in the draw panel
    drawPanel->SetMarriageCheck(node1, node2, lca, canMarry);
}

void FamilyTreeFrame::ProcessInput(const wxString& input) {
    wxStringTokenizer tokenizer(input, wxT("\n\r"), wxTOKEN_STRTOK);
    wxString token;
    int lineCount = 0;

    while (tokenizer.HasMoreTokens()) {
        token = tokenizer.GetNextToken().Trim();
        if (token.IsEmpty()) continue;

        lineCount++;
        
        // Parse line expecting format: "ParentID ChildID"
        wxStringTokenizer lineTokenizer(token, wxT(" \t"), wxTOKEN_STRTOK);
        wxString parentID, childID;
        
        if (lineTokenizer.HasMoreTokens()) {
            parentID = lineTokenizer.GetNextToken().Trim();
        }
        if (lineTokenizer.HasMoreTokens()) {
            childID = lineTokenizer.GetNextToken().Trim();
        }
        
        if (!parentID.IsEmpty() && !childID.IsEmpty()) {
            AddRelationship(parentID, childID);
        } else {
            wxMessageBox(wxString::Format(wxT("Error: Invalid format on line %d"), lineCount), wxT("Loi"), wxICON_ERROR);
        }
    }
}

void FamilyTreeFrame::AddRelationship(const wxString& parentName, const wxString& childName) {
    if (parentName.IsEmpty() || childName.IsEmpty()) {
        wxMessageBox(wxT("Error: Ten khong duoc de trong"), wxT("Loi"), wxICON_ERROR);
        return;
    }

    PersonNode* parent;
    if (nodes.find(parentName) == nodes.end()) {
        parent = new PersonNode(parentName);
        nodes[parentName] = parent;
        if (!root) root = parent;
    }
    else {
        parent = nodes[parentName];
    }

    PersonNode* child;
    if (nodes.find(childName) == nodes.end()) {
        child = new PersonNode(childName);
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